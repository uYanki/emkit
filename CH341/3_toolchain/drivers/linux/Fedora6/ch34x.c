/*
	* CH34x USB to serial adaptor driver
*/   
  	
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/serial.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>

/*
 * Version Information
 */
#define DRIVER_VERSION "v0.1"
#define DRIVER_DESC "NanJing QinHeng CH34x USB to serial adaptor driver"

#define ch34x_VENDOR_ID_1  0x4348
#define ch34x_VENDOR_ID_2  0x1a86
#define ch34x_PRODUCT_ID 	0x5523

static int debug;

#define ch34x_CLOSING_WAIT	(30*HZ)

#define ch34x_BUF_SIZE		1024
#define ch34x_TMP_BUF_SIZE	1024

static DECLARE_MUTEX(ch34x_tmp_buf_sem);

struct ch34x_buf {
	unsigned int	buf_size;
	char		*buf_buf;
	char		*buf_get;
	char		*buf_put;
};

static struct usb_device_id id_table [] = {
	{ USB_DEVICE(ch34x_VENDOR_ID_1, ch34x_PRODUCT_ID) },
	{ USB_DEVICE(ch34x_VENDOR_ID_2, ch34x_PRODUCT_ID) },
	{ }					/* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, id_table);

static struct usb_driver ch34x_driver = {
	.name =		"ch34x",
	.probe =	usb_serial_probe,
	.disconnect =	usb_serial_disconnect,
	.id_table =	id_table,
	.no_dynamic_id=1,
};

#define SET_LINE_REQUEST_TYPE		0x21
#define SET_LINE_REQUEST		0x20

#define SET_CONTROL_REQUEST_TYPE	0x21
#define SET_CONTROL_REQUEST		0x22
#define CONTROL_DTR			0x01
#define CONTROL_RTS			0x02

#define BREAK_REQUEST_TYPE		0x21
#define BREAK_REQUEST			0x23	
#define BREAK_ON			0xffff
#define BREAK_OFF			0x0000

#define GET_LINE_REQUEST_TYPE		0xa1
#define GET_LINE_REQUEST		0x21

#define VENDOR_WRITE_REQUEST_TYPE	0x40
#define VENDOR_WRITE_REQUEST		0x01

#define VENDOR_READ_REQUEST_TYPE	0xc0
#define VENDOR_READ_REQUEST		0x01

#define UART_STATE			0x08
#define UART_STATE_TRANSIENT_MASK	0x74
#define UART_DCD			0x01
#define UART_DSR			0x02
#define UART_BREAK_ERROR		0x04
#define UART_RING			0x08
#define UART_FRAME_ERROR		0x10
#define UART_PARITY_ERROR		0x20
#define UART_OVERRUN_ERROR		0x40
#define UART_CTS			0x80

/* function prototypes for a ch34x serial converter */
static int ch34x_open (struct usb_serial_port *port, struct file *filp);
static void ch34x_close (struct usb_serial_port *port, struct file *filp);
static void ch34x_set_termios (struct usb_serial_port *port,struct termios *old);
static int ch34x_ioctl (struct usb_serial_port *port, struct file *file,unsigned int cmd, unsigned long arg);
static void ch34x_read_int_callback (struct urb *urb, struct pt_regs *regs);
static void ch34x_read_bulk_callback (struct urb *urb, struct pt_regs *regs);
static void ch34x_write_bulk_callback (struct urb *urb, struct pt_regs *regs);
static int ch34x_write (struct usb_serial_port *port,const unsigned char *buf, int count);
static void ch34x_send (struct usb_serial_port *port);
static int ch34x_write_room(struct usb_serial_port *port);
static int ch34x_chars_in_buffer(struct usb_serial_port *port);
static void ch34x_break_ctl(struct usb_serial_port *port,int break_state);
static int ch34x_tiocmget (struct usb_serial_port *port, struct file *file);
static int ch34x_tiocmset (struct usb_serial_port *port, struct file *file,unsigned int set, unsigned int clear);
static int ch34x_startup (struct usb_serial *serial);
static void ch34x_shutdown (struct usb_serial *serial);
static struct ch34x_buf *ch34x_buf_alloc(unsigned int size);
static void ch34x_buf_free(struct ch34x_buf *pb);
static void ch34x_buf_clear(struct ch34x_buf *pb);
static unsigned int ch34x_buf_data_avail(struct ch34x_buf *pb);
static unsigned int ch34x_buf_space_avail(struct ch34x_buf *pb);
static unsigned int ch34x_buf_put(struct ch34x_buf *pb, const char *buf,unsigned int count);
static unsigned int ch34x_buf_get(struct ch34x_buf *pb, char *buf,unsigned int count);

static struct usb_serial_driver ch34x_device={
	.driver={
		.owner =		THIS_MODULE,
		.name =			"ch34x",
	},
	.id_table =		id_table,
	.num_interrupt_in =	NUM_DONT_CARE,
	.num_bulk_in =		1,
	.num_bulk_out =	1,
	.num_ports =		1,
	.open =			ch34x_open,
	.close =		ch34x_close,
	.write =		ch34x_write,
	.ioctl =		ch34x_ioctl,
	.break_ctl =		ch34x_break_ctl,
	.set_termios =		ch34x_set_termios,
	.tiocmget =		ch34x_tiocmget,
	.tiocmset =		ch34x_tiocmset,
	.read_bulk_callback =	ch34x_read_bulk_callback,
	.read_int_callback =	ch34x_read_int_callback,
	.write_bulk_callback =	ch34x_write_bulk_callback,
	.write_room =		ch34x_write_room,
	.chars_in_buffer =	ch34x_chars_in_buffer,
	.attach =		ch34x_startup,
	.shutdown =		ch34x_shutdown,
};


struct ch34x_private {
	spinlock_t lock;
	struct ch34x_buf *buf;
	int write_urb_in_use;
	wait_queue_head_t delta_msr_wait;
	u8 line_control;
	u8 line_status;
	u8 termios_initialized;
};


static int ch34x_startup (struct usb_serial *serial)
{
	struct ch34x_private *priv;
	int i;
	dbg("ch34x_startup\n");
	dbg("serial->num_ports=%d\n",serial->num_ports);
	
	for (i = 0; i < serial->num_ports; ++i) 
	{
		priv = kmalloc (sizeof (struct ch34x_private), GFP_KERNEL);
		if (!priv)
			goto cleanup;
		memset (priv, 0x00, sizeof (struct ch34x_private));
		spin_lock_init(&priv->lock);
		priv->buf = ch34x_buf_alloc(ch34x_BUF_SIZE);
		if (priv->buf == NULL) 
		{
			kfree(priv);
			goto cleanup;
		}
		init_waitqueue_head(&priv->delta_msr_wait);
		usb_set_serial_port_data(serial->port[i], priv);
	}
	return 0;

cleanup:
	for (--i; i>=0; --i) 
	{
		priv = usb_get_serial_port_data(serial->port[i]);
		ch34x_buf_free(priv->buf);
		kfree(priv);
		usb_set_serial_port_data(serial->port[i], NULL);
	}
	return -ENOMEM;
}

static int set_control_lines (struct usb_device *dev, u8 value)
{
	int retval;
	
	retval = usb_control_msg (dev, usb_sndctrlpipe (dev, 0),
				  SET_CONTROL_REQUEST, SET_CONTROL_REQUEST_TYPE,
				  value, 0, NULL, 0, 100);
	dbg("%s - value = %d, retval = %d", __FUNCTION__, value, retval);
	return retval;
}
 
static int ch34x_write (struct usb_serial_port *port,  const unsigned char *buf, int count)
{
	
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	dbg("ch34x_write\n");
	dbg("%s - port %d, %d bytes", __FUNCTION__, port->number, count);

	if (!count)
		return count;

	spin_lock_irqsave(&priv->lock, flags);
	count = ch34x_buf_put(priv->buf, buf, count);
	spin_unlock_irqrestore(&priv->lock, flags);

	ch34x_send(port);

	return count;
}

static void ch34x_send(struct usb_serial_port *port)
{
	int count, result;
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;

	dbg("%s - port %d", __FUNCTION__, port->number);

	spin_lock_irqsave(&priv->lock, flags);

	if (priv->write_urb_in_use) 
	{
		spin_unlock_irqrestore(&priv->lock, flags);
		return;
	}

	count = ch34x_buf_get(priv->buf, port->write_urb->transfer_buffer,port->bulk_out_size);

	if (count == 0) 
	{
		spin_unlock_irqrestore(&priv->lock, flags);
		return;
	}

	priv->write_urb_in_use = 1;

	spin_unlock_irqrestore(&priv->lock, flags);

	usb_serial_debug_data(debug, &port->dev, __FUNCTION__, count, port->write_urb->transfer_buffer);

	port->write_urb->transfer_buffer_length = count;
	port->write_urb->dev = port->serial->dev;
	result = usb_submit_urb (port->write_urb, GFP_ATOMIC);
	if (result) 
	{
		dev_err(&port->dev, "%s - failed submitting write urb, error %d\n", __FUNCTION__, result);
		priv->write_urb_in_use = 0;
		// TODO: reschedule ch34x_send
	}

	schedule_work(&port->work);
}

static int ch34x_write_room(struct usb_serial_port *port)
{
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	int room = 0;
	unsigned long flags;

	dbg("%s - port %d", __FUNCTION__, port->number);

	spin_lock_irqsave(&priv->lock, flags);
	room = ch34x_buf_space_avail(priv->buf);
	spin_unlock_irqrestore(&priv->lock, flags);

	dbg("%s - returns %d", __FUNCTION__, room);
	return room;
}


static int ch34x_chars_in_buffer(struct usb_serial_port *port)
{
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	int chars = 0;
	unsigned long flags;

	dbg("%s - port %d", __FUNCTION__, port->number);

	spin_lock_irqsave(&priv->lock, flags);
	chars = ch34x_buf_data_avail(priv->buf);
	spin_unlock_irqrestore(&priv->lock, flags);

	dbg("%s - returns %d", __FUNCTION__, chars);
	return chars;
}


static int SetBaudrate( unsigned int  mBaudRate, struct usb_serial_port *port)
{      
	
	struct usb_serial *serial = port->serial;
	unsigned char mTimerCtrl, mTimerCount;
	unsigned short mValue, mIndex;
	int i;
	mValue = 0; mIndex = 0;
	dbg("SetBaudrate.\n");
	dbg ("0x40:1:0:1  %d", mBaudRate);
	switch (mBaudRate) 
	{ 
	case  2225: { mTimerCtrl = 0; mTimerCount = 0x16; break; }//50
	case  2226: { mTimerCtrl = 0; mTimerCount = 0x64; break; }//75
	case  2227: { mTimerCtrl = 0; mTimerCount = 0x96; break; }//110
	case  2228: { mTimerCtrl = 0; mTimerCount = 0xa9; break; }//135
	case  2229: { mTimerCtrl = 0; mTimerCount = 0xb2; break; }//150
	case  2231: { mTimerCtrl = 0; mTimerCount = 0xd9; break; }//300
	case  2232: { mTimerCtrl = 1; mTimerCount = 0x64; break; }//600
	case  2233: { mTimerCtrl = 1; mTimerCount = 0xb2; break; }//1200
	case  2234: { mTimerCtrl = 1; mTimerCount = 0xcc; break; }//1800
	case  2235: { mTimerCtrl = 1; mTimerCount = 0xd9; break; }//2400
	case  2236: { mTimerCtrl = 2; mTimerCount = 0x64; break; }//4800
	case  2237: { mTimerCtrl = 2; mTimerCount = 0xb2; break; }//9600
	case  2238: { mTimerCtrl = 2; mTimerCount = 0xd9; break; }//19200
	case  2239: { mTimerCtrl = 3; mTimerCount = 0x64; break; }//38400
	case  6321: { mTimerCtrl = 3; mTimerCount = 0x98; break; }//57600
	case  6322: { mTimerCtrl = 3; mTimerCount = 0xcc; break; }//115200
	case  6323: { mTimerCtrl = 3; mTimerCount = 0xe6; break; }//230400
	case  6324: { mTimerCtrl = 3; mTimerCount = 0xf3; break; }//460800
	case  6325: { mTimerCtrl = 3; mTimerCount = 0xf4; break; }//500000
	case  6326:
	case  6327: { mTimerCtrl = 7; mTimerCount = 0xf3; break; }//921600
	case  6328: { mTimerCtrl = 3; mTimerCount = 0xfa; break; }//1000000
	case  6329:
	case  6330:
	case  6331: { mTimerCtrl = 3; mTimerCount = 0xfd; break; }//2000000
	case  6332:
	case  6333: { mTimerCtrl=3;mTimerCount=0xfe; break; }//3000000	
	default: { return( -EPROTO ); break; }
	}
	mValue |= 0x04; mIndex |= mTimerCtrl;
	mValue |= 0x08; mIndex |= (unsigned short)mTimerCount << 8;
	i = usb_control_msg (serial->dev, usb_sndctrlpipe (serial->dev,0),
		0xA1,0x40,mValue,mIndex,NULL,0,100);
	dbg ("0x40:1:0:1  %d", i);
	return 0;
}


static void ch34x_set_termios (struct usb_serial_port *port, struct termios *old_termios)
{

	struct ch34x_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int cflag;
	dbg("ch34x_set_termios\n");
	dbg("%s -  port %d", __FUNCTION__, port->number);

	if ((!port->tty) || (!port->tty->termios)) 
	{
		dbg("%s - no tty structures", __FUNCTION__);
		return;
	}

	spin_lock_irqsave(&priv->lock, flags);
	if (!priv->termios_initialized) 
	{
		*(port->tty->termios) = tty_std_termios;
		port->tty->termios->c_cflag = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
		priv->termios_initialized = 1;
	}
	spin_unlock_irqrestore(&priv->lock, flags);

	cflag = port->tty->termios->c_cflag;
	/* check that they really want us to change something */
	if (old_termios) 
	{
		if ((cflag == old_termios->c_cflag) &&
		    (RELEVANT_IFLAG(port->tty->termios->c_iflag) == RELEVANT_IFLAG(old_termios->c_iflag))) 
		{
		    dbg("%s - nothing to change...", __FUNCTION__);
		    return;
		}
	}
	SetBaudrate(cflag,port);
}

static int ch34x_open (struct usb_serial_port *port, struct file *filp)
{
	struct termios tmp_termios;
	struct usb_serial *serial = port->serial;
	int result;
	dbg("ch34x_open!\n");
	dbg("%s -  port %d", __FUNCTION__, port->number);

	usb_clear_halt(serial->dev, port->write_urb->pipe);
	usb_clear_halt(serial->dev, port->read_urb->pipe);

	if (port->tty) 
	{
		ch34x_set_termios (port, &tmp_termios);
	}

	//FIXME: need to assert RTS and DTR if CRTSCTS off

	dbg("%s - submitting read urb", __FUNCTION__);
	port->read_urb->dev = serial->dev;
	result = usb_submit_urb (port->read_urb, GFP_KERNEL);
	if (result) 
	{
		dev_err(&port->dev, "%s - failed submitting read urb, error %d\n", __FUNCTION__, result);
		ch34x_close (port, NULL);
		return -EPROTO;
	}

	dbg("%s - submitting interrupt urb", __FUNCTION__);
	port->interrupt_in_urb->dev = serial->dev;
	result = usb_submit_urb (port->interrupt_in_urb, GFP_KERNEL);
	if (result) 
	{
		dev_err(&port->dev, "%s - failed submitting interrupt urb, error %d\n", __FUNCTION__, result);
		ch34x_close (port, NULL);
		return -EPROTO;
	}

	return 0;
}

static void ch34x_close (struct usb_serial_port *port, struct file *filp)
{
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int c_cflag;
	int bps;
	long timeout;
	wait_queue_t wait;
	dbg("ch34x_close\n");
	dbg("%s - port %d", __FUNCTION__, port->number);

	/* wait for data to drain from the buffer */
	spin_lock_irqsave(&priv->lock, flags);
	timeout = ch34x_CLOSING_WAIT;
	init_waitqueue_entry(&wait, current);
	add_wait_queue(&port->tty->write_wait, &wait);
	for (;;) 
	{
		set_current_state(TASK_INTERRUPTIBLE);
		if (ch34x_buf_data_avail(priv->buf) == 0
		|| timeout == 0 || signal_pending(current)
		|| !usb_get_intfdata(port->serial->interface))	/* disconnect */
			break;
		spin_unlock_irqrestore(&priv->lock, flags);
		timeout = schedule_timeout(timeout);
		spin_lock_irqsave(&priv->lock, flags);
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&port->tty->write_wait, &wait);
	/* clear out any remaining data in the buffer */
	ch34x_buf_clear(priv->buf);
	spin_unlock_irqrestore(&priv->lock, flags);

	/* wait for characters to drain from the device */
	/* (this is long enough for the entire 256 byte */
	/* ch34x hardware buffer to drain with no flow */
	/* control for data rates of 1200 bps or more, */
	/* for lower rates we should really know how much */
	/* data is in the buffer to compute a delay */
	/* that is not unnecessarily long) */
	bps = tty_get_baud_rate(port->tty);
	if (bps > 1200)
		timeout = max((HZ*2560)/bps,HZ/10);
	else
		timeout = 2*HZ;
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(timeout);

	/* shutdown our urbs */
	dbg("%s - shutting down urbs", __FUNCTION__);
	usb_kill_urb(port->write_urb);
	usb_kill_urb(port->read_urb);
	usb_kill_urb(port->interrupt_in_urb);

	if (port->tty) 
	{
		c_cflag = port->tty->termios->c_cflag;
		if (c_cflag & HUPCL)
		{
			/* drop DTR and RTS */
			spin_lock_irqsave(&priv->lock, flags);
			priv->line_control = 0;
			spin_unlock_irqrestore (&priv->lock, flags);
			set_control_lines (port->serial->dev, 0);
		}
	}
}


static int ch34x_tiocmset (struct usb_serial_port *port, struct file *file,unsigned int set, unsigned int clear)
{
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	u8 control;

	spin_lock_irqsave (&priv->lock, flags);
	if (set & TIOCM_RTS)
		priv->line_control |= CONTROL_RTS;
	if (set & TIOCM_DTR)
		priv->line_control |= CONTROL_DTR;
	if (clear & TIOCM_RTS)
		priv->line_control &= ~CONTROL_RTS;
	if (clear & TIOCM_DTR)
		priv->line_control &= ~CONTROL_DTR;
	control = priv->line_control;
	spin_unlock_irqrestore (&priv->lock, flags);

	return set_control_lines (port->serial->dev, control);
}

static int ch34x_tiocmget (struct usb_serial_port *port, struct file *file)
{
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int mcr;
	unsigned int status;
	unsigned int result;

	dbg("%s (%d)", __FUNCTION__, port->number);

	spin_lock_irqsave (&priv->lock, flags);
	mcr = priv->line_control;
	status = priv->line_status;
	spin_unlock_irqrestore (&priv->lock, flags);

	result = ((mcr & CONTROL_DTR)		? TIOCM_DTR : 0)
		  | ((mcr & CONTROL_RTS)	? TIOCM_RTS : 0)
		  | ((status & UART_CTS)	? TIOCM_CTS : 0)
		  | ((status & UART_DSR)	? TIOCM_DSR : 0)
		  | ((status & UART_RING)	? TIOCM_RI  : 0)
		  | ((status & UART_DCD)	? TIOCM_CD  : 0);

	dbg("%s - result = %x", __FUNCTION__, result);

	return result;
}


static int wait_modem_info(struct usb_serial_port *port, unsigned int arg)
{
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int prevstatus;
	unsigned int status;
	unsigned int changed;

	spin_lock_irqsave (&priv->lock, flags);
	prevstatus = priv->line_status;
	spin_unlock_irqrestore (&priv->lock, flags);

	while (1) 
	{
		interruptible_sleep_on(&priv->delta_msr_wait);
		/* see if a signal did it */
		if (signal_pending(current))
			return -ERESTARTSYS;
		
		spin_lock_irqsave (&priv->lock, flags);
		status = priv->line_status;
		spin_unlock_irqrestore (&priv->lock, flags);
		
		changed=prevstatus^status;
		
		if (((arg & TIOCM_RNG) && (changed & UART_RING)) ||
		    ((arg & TIOCM_DSR) && (changed & UART_DSR)) ||
		    ((arg & TIOCM_CD)  && (changed & UART_DCD)) ||
		    ((arg & TIOCM_CTS) && (changed & UART_CTS)) ) {
			return 0;
		}
		prevstatus = status;
	}
	/* NOTREACHED */
	return 0;
}


static int ch34x_ioctl (struct usb_serial_port *port, struct file *file, unsigned int cmd, unsigned long arg)
{
	dbg("%s (%d) cmd = 0x%04x", __FUNCTION__, port->number, cmd);

	switch (cmd) 
	{
		case TIOCMIWAIT:
			dbg("%s (%d) TIOCMIWAIT", __FUNCTION__,  port->number);
			return wait_modem_info(port, arg);

		default:
			dbg("%s not supported = 0x%04x", __FUNCTION__, cmd);
			break;
	}

	return -ENOIOCTLCMD;
}


static void ch34x_break_ctl (struct usb_serial_port *port, int break_state)
{
	struct usb_serial *serial = port->serial;
	u16 state;
	int result;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (break_state == 0)
		state = BREAK_OFF;
	else
		state = BREAK_ON;
	dbg("%s - turning break %s", __FUNCTION__, state==BREAK_OFF ? "off" : "on");

	result = usb_control_msg (serial->dev, usb_sndctrlpipe (serial->dev, 0),
				  BREAK_REQUEST, BREAK_REQUEST_TYPE, state, 
				  0, NULL, 0, 100);
	if (result)
		dbg("%s - error sending break = %d", __FUNCTION__, result);
}



static void ch34x_shutdown (struct usb_serial *serial)
{
	int i;
	struct ch34x_private *priv;
	dbg("ch34x_shutdown\n");
	dbg("%s", __FUNCTION__);

	for (i = 0; i < serial->num_ports; ++i) 
	{
		priv = usb_get_serial_port_data(serial->port[i]);
		if (priv) 
		{
			ch34x_buf_free(priv->buf);
			kfree(priv);
			usb_set_serial_port_data(serial->port[i], NULL);
		}
	}		
}


static void ch34x_read_int_callback (struct urb *urb, struct pt_regs *regs)
{
	struct usb_serial_port *port = (struct usb_serial_port *) urb->context;
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	unsigned char *data = urb->transfer_buffer;
	unsigned long flags;
	int status;
	u8 uart_state;

	dbg("%s (%d)", __FUNCTION__, port->number);

	switch (urb->status) 
	{
	case 0:
		/* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* this urb is terminated, clean up */
		dbg("%s - urb shutting down with status: %d", __FUNCTION__, urb->status);
		return;
	default:
		dbg("%s - nonzero urb status received: %d", __FUNCTION__, urb->status);
		goto exit;
	}


	usb_serial_debug_data(debug, &port->dev, __FUNCTION__, urb->actual_length, urb->transfer_buffer);

	if (urb->actual_length < UART_STATE)
		goto exit;

	/* Save off the uart status for others to look at */
	uart_state = data[UART_STATE];
	spin_lock_irqsave(&priv->lock, flags);
	uart_state |= (priv->line_status & UART_STATE_TRANSIENT_MASK);
	priv->line_status = uart_state;
	spin_unlock_irqrestore(&priv->lock, flags);
		
exit:
	status = usb_submit_urb (urb, GFP_ATOMIC);
	if (status)
		dev_err(&urb->dev->dev, "%s - usb_submit_urb failed with result %d\n",
			__FUNCTION__, status);
}


static void ch34x_read_bulk_callback (struct urb *urb, struct pt_regs *regs)
{
	struct usb_serial_port *port = (struct usb_serial_port *) urb->context;
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	struct tty_struct *tty;
	unsigned char *data = urb->transfer_buffer;
	unsigned long flags;
	int i;
	int result;
	u8 status;
	char tty_flag;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (urb->status) 
	{
		dbg("%s - urb->status = %d", __FUNCTION__, urb->status);
		if (!port->open_count) 
		{
			dbg("%s - port is closed, exiting.", __FUNCTION__);
			return;
		}
		if (urb->status == -EPROTO) 
		{
			/* ch34x mysteriously fails with -EPROTO reschedule the read */
			dbg("%s - caught -EPROTO, resubmitting the urb", __FUNCTION__);
			urb->status = 0;
			urb->dev = port->serial->dev;
			result = usb_submit_urb(urb, GFP_ATOMIC);
			if (result)
				dev_err(&urb->dev->dev, "%s - failed resubmitting read urb, error %d\n", __FUNCTION__, result);
			return;
		}
		dbg("%s - unable to handle the error, exiting.", __FUNCTION__);
		return;
	}

	usb_serial_debug_data(debug, &port->dev, __FUNCTION__, urb->actual_length, data);

	/* get tty_flag from status */
	tty_flag = TTY_NORMAL;

	spin_lock_irqsave(&priv->lock, flags);
	status = priv->line_status;
	priv->line_status &= ~UART_STATE_TRANSIENT_MASK;
	spin_unlock_irqrestore(&priv->lock, flags);
	wake_up_interruptible (&priv->delta_msr_wait);

	/* break takes precedence over parity, */
	/* which takes precedence over framing errors */
	if (status & UART_BREAK_ERROR )
		tty_flag = TTY_BREAK;
	else if (status & UART_PARITY_ERROR)
		tty_flag = TTY_PARITY;
	else if (status & UART_FRAME_ERROR)
		tty_flag = TTY_FRAME;
	dbg("%s - tty_flag = %d", __FUNCTION__, tty_flag);

	tty = port->tty;
	if (tty && urb->actual_length) {
		/* overrun is special, not associated with a char */
		if (status & UART_OVERRUN_ERROR)
			tty_insert_flip_char(tty, 0, TTY_OVERRUN);

		for (i = 0; i < urb->actual_length; ++i) {
		//	if (tty->flip.count >= TTY_FLIPBUF_SIZE) {
		//		tty_flip_buffer_push(tty);
		//	}
			tty_insert_flip_char (tty, data[i], tty_flag);
		}
		tty_flip_buffer_push (tty);
	}

	/* Schedule the next read _if_ we are still open */
	if (port->open_count) {
		urb->dev = port->serial->dev;
		result = usb_submit_urb(urb, GFP_ATOMIC);
		if (result)
			dev_err(&urb->dev->dev, "%s - failed resubmitting read urb, error %d\n", __FUNCTION__, result);
	}

	return;
}



static void ch34x_write_bulk_callback (struct urb *urb, struct pt_regs *regs)
{
	struct usb_serial_port *port = (struct usb_serial_port *) urb->context;
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	int result;

	dbg("%s - port %d", __FUNCTION__, port->number);

	switch (urb->status) 
	{
	case 0:
		/* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* this urb is terminated, clean up */
		dbg("%s - urb shutting down with status: %d", __FUNCTION__, urb->status);
		priv->write_urb_in_use = 0;
		return;
	default:
		/* error in the urb, so we have to resubmit it */
		dbg("%s - Overflow in write", __FUNCTION__);
		dbg("%s - nonzero write bulk status received: %d", __FUNCTION__, urb->status);
		port->write_urb->transfer_buffer_length = 1;
		port->write_urb->dev = port->serial->dev;
		result = usb_submit_urb (port->write_urb, GFP_ATOMIC);
		if (result)
			dev_err(&urb->dev->dev, "%s - failed resubmitting write urb, error %d\n", __FUNCTION__, result);
		else
			return;
	}

	priv->write_urb_in_use = 0;

	/* send any buffered data */
	ch34x_send(port);
}


/*
 * ch34x_buf_alloc
 *
 * Allocate a circular buffer and all associated memory.
 */

static struct ch34x_buf *ch34x_buf_alloc(unsigned int size)
{

	struct ch34x_buf *pb;
	if (size == 0)
		return NULL;
	pb = (struct ch34x_buf *)kmalloc(sizeof(struct ch34x_buf), GFP_KERNEL);
	if (pb == NULL)
		return NULL;
	pb->buf_buf = kmalloc(size, GFP_KERNEL);
	if (pb->buf_buf == NULL) 
	{
		kfree(pb);
		return NULL;
	}
	pb->buf_size = size;
	pb->buf_get = pb->buf_put = pb->buf_buf;
	return pb;
}


/*
 * ch34x_buf_free
 *
 * Free the buffer and all associated memory.
 */

static void ch34x_buf_free(struct ch34x_buf *pb)
{
	if (pb != NULL) 
	{
		if (pb->buf_buf != NULL)
			kfree(pb->buf_buf);
		kfree(pb);
	}
}


/*
 * ch34x_buf_clear
   *
 * Clear out all data in the circular buffer.
 */

static void ch34x_buf_clear(struct ch34x_buf *pb)
{
	if (pb != NULL)
		pb->buf_get = pb->buf_put;
		/* equivalent to a get of all data available */
}


/*
 * ch34x_buf_data_avail
 *
 * Return the number of bytes of data available in the circular
 * buffer.
 */

static unsigned int ch34x_buf_data_avail(struct ch34x_buf *pb)
{
	if (pb != NULL)
		return ((pb->buf_size + pb->buf_put - pb->buf_get) % pb->buf_size);
	else
		return 0;
}


/*
 * ch34x_buf_space_avail
 *
 * Return the number of bytes of space available in the circular
 * buffer.
 */

static unsigned int ch34x_buf_space_avail(struct ch34x_buf *pb)
{
	if (pb != NULL)
		return ((pb->buf_size + pb->buf_get - pb->buf_put - 1) % pb->buf_size);
	else
		return 0;
}


/*
 * ch34x_buf_put
 *
 * Copy data data from a user buffer and put it into the circular buffer.
 * Restrict to the amount of space available.
 *
 * Return the number of bytes copied.
 */

static unsigned int ch34x_buf_put(struct ch34x_buf *pb, const char *buf, unsigned int count)
{

	unsigned int len;
	if (pb == NULL)
		return 0;

	len  = ch34x_buf_space_avail(pb);
	dbg("len=%d\n",len);
	if (count > len)
		count = len;

	if (count == 0)
		return 0;

	len = pb->buf_buf + pb->buf_size - pb->buf_put;
	if (count > len) 
	{
		memcpy(pb->buf_put, buf, len);
		memcpy(pb->buf_buf, buf+len, count - len);
		pb->buf_put = pb->buf_buf + count - len;
	} 
	else
	{
		memcpy(pb->buf_put, buf, count);
		if (count < len)
			pb->buf_put += count;
		else /* count == len */
			pb->buf_put = pb->buf_buf;
	}

	return count;

}


/*
 * ch34x_buf_get
 *
 * Get data from the circular buffer and copy to the given buffer.
 * Restrict to the amount of data available.
 *
 * Return the number of bytes copied.
 */

static unsigned int ch34x_buf_get(struct ch34x_buf *pb, char *buf,unsigned int count)
{
	unsigned int len;
	if (pb == NULL)
		return 0;
	len = ch34x_buf_data_avail(pb);
	if (count > len)
		count = len;
	if (count == 0)
		return 0;
	len = pb->buf_buf + pb->buf_size - pb->buf_get;
	if (count > len) 
	{
		memcpy(buf, pb->buf_get, len);
		memcpy(buf+len, pb->buf_buf, count - len);
		pb->buf_get = pb->buf_buf + count - len;
	}
	else 
	{
		memcpy(buf, pb->buf_get, count);
		if (count < len)
			pb->buf_get += count;
		else /* count == len */
			pb->buf_get = pb->buf_buf;
	}
	return count;
}

//加载驱动
static int __init ch34x_init (void)
{
	int retval;
	retval = usb_serial_register(&ch34x_device);//注册设备，成功返回0
	if (retval)
	{	
		dbg("%s - failed usb_register_device\n",__FUNCTION__);
		goto failed_usb_serial_register;
	}
	else
	{
		dbg("%s - success usb_register_device\n",__FUNCTION__);
		retval = usb_register(&ch34x_driver);//注册驱动，成功返回0
		if (retval)
		{
			dbg("%s - failed usb_register_driver\n",__FUNCTION__);
			goto failed_usb_register;
		}
		else
		{
			dbg("%s - success usb_register_driver\n",__FUNCTION__);
			info(DRIVER_DESC " " DRIVER_VERSION);
		}
	}	
	return 0;
failed_usb_register:
	usb_serial_deregister(&ch34x_device);
failed_usb_serial_register:
	return retval;
}


//卸载驱动
static void __exit ch34x_exit (void)
{
	dbg("%s - ch34x_exit\n",__FUNCTION__);
	usb_deregister (&ch34x_driver);
	usb_serial_deregister (&ch34x_device);
}

//加载模块
module_init(ch34x_init);
//卸载模块
module_exit(ch34x_exit);

MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL");

module_param(debug, bool, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Debug enabled or not");

