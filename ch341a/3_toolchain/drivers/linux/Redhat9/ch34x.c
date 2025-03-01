/*
	ch34x USB to serial adaptor driver
	http://www.wch.cn
	http://www.winchiphead.cn  
*/

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/serial.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include <linux/usb.h>

#ifdef CONFIG_USB_SERIAL_DEBUG
	static int debug=1;
#else
	static int debug;
#endif

#include "usb-serial.h"

/*
* Version Information
*/
#define DRIVER_VERSION "v1.0"
#define DRIVER_DESC "NanJing QinHeng ch34x USB to serial adaptor driver"
#define FALSE -1

#define ch34x_VENDOR_ID_1  0x4348
#define ch34x_VENDOR_ID_2  0x1a86
#define ch34x_PRODUCT_ID 0x5523
#define CONTROL_DTR      0x20
#define CONTROL_RTS      0x40
#define BREAK_ON	     0xffff
#define BREAK_OFF	     0x0000
#define VENDOR_READ_REQUEST_TYPE	0xc0
#define VENDOR_READ_REQUEST		    0x01
#define VENDOR_WRITE_REQUEST_TYPE	0x40
#define VENDOR_WRITE_REQUEST		0x01
static struct usb_device_id id_table [] = 
{
	{ USB_DEVICE(ch34x_VENDOR_ID_1, ch34x_PRODUCT_ID) },
	{ USB_DEVICE(ch34x_VENDOR_ID_2, ch34x_PRODUCT_ID) },
	{ }					/* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, id_table);

static int  ch34x_open (struct usb_serial_port *port, struct file *filp);
static void ch34x_close (struct usb_serial_port *port, struct file *filp);
static void ch34x_set_termios (struct usb_serial_port *port,struct termios *old);
static int  ch34x_ioctl (struct usb_serial_port *port, struct file *file,unsigned int cmd, unsigned long arg);
static void ch34x_read_int_callback (struct urb *urb);
static void ch34x_read_bulk_callback (struct urb *urb);
static void ch34x_write_bulk_callback (struct urb *urb);
static int  ch34x_write (struct usb_serial_port *port, int from_user,const unsigned char*buf, int count);
static void ch34x_break_ctl(struct usb_serial_port *port,int break_state);
static int  ch34x_startup (struct usb_serial *serial);
static void ch34x_shutdown (struct usb_serial *serial);
static int SetBaudrate( unsigned int value, struct usb_serial_port *port);

static struct usb_serial_device_type ch34x_device = {
		.owner =		        THIS_MODULE,
		.name =		         	"ch34x",
		.id_table =	        	id_table,
		.num_interrupt_in =     NUM_DONT_CARE,
		.num_bulk_in =       	1,
		.num_bulk_out =	    	1,
		.num_ports =	    	1,
		.open =		            ch34x_open,
		.close =		        ch34x_close,
		.write =		        ch34x_write,
		.ioctl =		        ch34x_ioctl,
		.break_ctl =	    	ch34x_break_ctl,
		.set_termios =	    	ch34x_set_termios,
		.read_bulk_callback =   ch34x_read_bulk_callback,
		.read_int_callback  =   ch34x_read_int_callback,
		.write_bulk_callback =	ch34x_write_bulk_callback,
		.startup =		        ch34x_startup,
		.shutdown =             ch34x_shutdown,
};

struct ch34x_private 
{
	u8 line_control;
	u8 termios_initialized;
	u8 driverType;
};

static int set_control_lines (struct usb_device *dev, u8 value)
{   
    dbg("set_control_lines.");
	int retval;
	retval = usb_control_msg (dev, usb_sndctrlpipe (dev, 0),0xA4, 0x40,value, 0, NULL, 0, 100);
	dbg("%s - value = %x, retval = %d", __FUNCTION__, value, retval);
	return retval;
}

static int  SetBaudrate( unsigned int  mBaudRate, struct usb_serial_port *port)
{      
	dbg("SetBaudrate.");
	struct usb_serial *serial = port->serial;
	unsigned char mTimerCtrl, mTimerCount;
	unsigned short mValue, mIndex;
	mValue = 0; mIndex = 0;
	dbg ("0x40:1:0:1  %d", mBaudRate);
	int i;
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
	i = usb_control_msg (serial->dev, usb_sndctrlpipe(serial->dev,0),0xA1,0x40,mValue,mIndex,NULL,0,100);
	return 1;
}

static int ch34x_startup (struct usb_serial *serial)
{       
    dbg("ch34x_startup.");
	struct ch34x_private *priv;
	int i;
    for (i = 0; i < serial->num_ports; ++i) 
	{
		priv = kmalloc (sizeof (struct ch34x_private), GFP_KERNEL);
		if (!priv)
			return -ENOMEM;
		memset (priv, 0x00, sizeof (struct ch34x_private));
		serial->port[i].private = priv;
	}
	return 0;
}

static int ch34x_open (struct usb_serial_port *port, struct file *filp)
{        
    dbg("ch34x_open.");
	struct termios tmp_termios;
	struct usb_serial *serial = port->serial;
	int result;
	struct ch34x_private *priv = port->private;
	if (port_paranoia_check (port, __FUNCTION__))
		return -ENODEV;
	dbg("%s -  port %d", __FUNCTION__, port->number);
	/* Setup termios */
	if (port->tty)
	{
		ch34x_set_termios (port, &tmp_termios);
	}
	//FIXME: need to assert RTS and DTR if CRTSCTS off
	priv = port->private;
	priv->line_control = 0x9F;
	set_control_lines (port->serial->dev,priv->line_control);
	priv->line_control = 0xFF;
	dbg("%s - submitting read urb", __FUNCTION__);
	port->read_urb->dev = serial->dev;
	result = usb_submit_urb (port->read_urb);
	if (result) 
	{
		err("%s - failed submitting read urb, error %d", __FUNCTION__, result);
		ch34x_close (port, NULL);
		return -EPROTO;
	}
	dbg("%s - submitting interrupt urb", __FUNCTION__);
	port->interrupt_in_urb->dev = serial->dev;
	result = usb_submit_urb (port->interrupt_in_urb);
	if (result) 
	{
		err("%s - failed submitting interrupt urb, error %d", __FUNCTION__, result);
		ch34x_close (port, NULL);
		return -EPROTO;
	}
	return 0;
}

static int ch34x_write (struct usb_serial_port *port, int from_user,  const unsigned char *buf, int count)
{    
	dbg("ch34x_write.");    
	int result;
	dbg("%s - port %d, %d bytes", __FUNCTION__, port->number, count);
	if (port->write_urb->status == -EINPROGRESS)
	{
		dbg("%s - already writing", __FUNCTION__);
		return 0;
	}
	count = (count > port->bulk_out_size) ? port->bulk_out_size : count;
	if (from_user) 
	{
		if (copy_from_user (port->write_urb->transfer_buffer, buf, count))
			return -EFAULT;
	} 
	else 
	{
		memcpy (port->write_urb->transfer_buffer, buf, count);
	}
	usb_serial_debug_data (__FILE__, __FUNCTION__, count, port->write_urb->transfer_buffer);
	port->write_urb->transfer_buffer_length = count;
	port->write_urb->dev = port->serial->dev;
	result = usb_submit_urb (port->write_urb);
	if (result)
		err("%s - failed submitting write urb, error %d", __FUNCTION__, result);
	else
		result = count;
	return result;
}

static void ch34x_set_termios (struct usb_serial_port *port, struct termios *old_termios)
{        
	dbg("set_termios.\n");
	unsigned int cflag;
    	dbg("%s -  port %d, initialized = %d", __FUNCTION__, port->number, ((struct ch34x_private *) port->private)->termios_initialized);
	if ((!port->tty) || (!port->tty->termios))
	{
		dbg("%s - no tty structures", __FUNCTION__);
		return;
	}
	if (!(((struct ch34x_private *) port->private)->termios_initialized)) 
	{
		*(port->tty->termios) = tty_std_termios;
		port->tty->termios->c_cflag = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
		((struct ch34x_private *) port->private)->termios_initialized = 1;
	}
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

/*close the serial*/
static void ch34x_close (struct usb_serial_port *port, struct file *filp)
{   
	dbg("ch34x_close.\n");
	struct usb_serial *serial;
	struct ch34x_private *priv;
	unsigned int c_cflag;
	int result;
	if (port_paranoia_check (port, __FUNCTION__))
		return;
	serial = get_usb_serial (port, __FUNCTION__);
	if (!serial)
		return;
	dbg("%s - port %d", __FUNCTION__, port->number);
	if (serial->dev) 
	{
		if (port->tty)
		{
			c_cflag = port->tty->termios->c_cflag;
			if (c_cflag & HUPCL) 
			{
				// drop DTR and RTS 
				priv = port->private;
				priv->line_control = 0x7F;
				set_control_lines (port->serial->dev,
					priv->line_control);
			}
		}
		/* shutdown our urbs */
		dbg("%s - shutting down urbs", __FUNCTION__);
		result = usb_unlink_urb (port->write_urb);
		if (result)
			dbg("%s - usb_unlink_urb (write_urb)"
			" failed with reason: %d", __FUNCTION__,
			result);
		result = usb_unlink_urb (port->read_urb);
		if (result)
			dbg("%s - usb_unlink_urb (read_urb) "
			"failed with reason: %d", __FUNCTION__,
			result);
		result = usb_unlink_urb (port->interrupt_in_urb);
		if (result)
			dbg("%s - usb_unlink_urb (interrupt_in_urb)"
			" failed with reason: %d", __FUNCTION__,
			result);
	}
}

static int set_modem_info (struct usb_serial_port *port, unsigned int cmd, unsigned int *value)
{
	struct ch34x_private *priv = port->private;
	unsigned int arg;
	if (copy_from_user(&arg, value, sizeof(int)))
	{
		return -EFAULT;
	}
	dbg ("0x40:0x1:0x0:0x41  %d", arg);
	switch (cmd) 
	{
	case TIOCMBIS:
		printk("TIOCMBIS.\n");
		if (arg & TIOCM_RTS)
			priv->line_control |= CONTROL_RTS;
		if (arg & TIOCM_DTR)
			priv->line_control |= CONTROL_DTR;
		break;
		
	case TIOCMBIC:
		printk("TIOCMBIC.\n");
		if (arg & TIOCM_RTS)
			priv->line_control &= ~CONTROL_RTS;
		if (arg & TIOCM_DTR)
			priv->line_control &= ~CONTROL_DTR;
		break;
		
	case TIOCMSET:
		// turn off RTS and DTR and then only turn
		//  on what was asked to 
		printk("TIOCMSET.\n");                             //CONTROL_RTS=0x40,CONTROL_DTR=0x20
		priv->line_control &= ~(CONTROL_RTS | CONTROL_DTR);//TIOCM_RTS=0x004,TIOCM_DTR=0x002
		priv->line_control |= ((arg & TIOCM_RTS) ? CONTROL_RTS : 0);
		priv->line_control |= ((arg & TIOCM_DTR) ? CONTROL_DTR : 0);
		break;
	}
	return set_control_lines (port->serial->dev, priv->line_control);
}

static int ch34x_ioctl (struct usb_serial_port *port, struct file *file, unsigned int cmd, unsigned long arg)
{       
	dbg("ch34x_ioctl.\n");
	dbg("%s (%d) cmd = 0x%04x", __FUNCTION__, port->number, cmd);
	switch (cmd) 
	{
	case TIOCMGET:
	case TIOCMBIS:
	case TIOCMBIC:
	case TIOCMSET:
		dbg("%s (%d) TIOCMSET/TIOCMBIC/TIOCMSET", __FUNCTION__,  port->number);
		return set_modem_info(port, cmd, (unsigned int *) arg);
	default:
		dbg("%s not supported = 0x%04x", __FUNCTION__, cmd);
		break;
	}
	return -ENOIOCTLCMD;
}

static void ch34x_break_ctl (struct usb_serial_port *port, int break_state)
{       
    dbg("ch34x_break_ctl.\n");
}

static void ch34x_shutdown (struct usb_serial *serial)
{        
    dbg("ch34x_shutdown.\n");
	int i;
	dbg("%s", __FUNCTION__);
	for (i = 0; i < serial->num_ports; ++i)
		kfree (serial->port[i].private);
}

static void ch34x_read_int_callback (struct urb *urb)
{       
	dbg("ch34x_read_int_callback.\n");
	struct usb_serial_port *port = (struct usb_serial_port *) urb->context;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);
	//ints auto restart...
	if (!serial)
	{
		return;
	}
	if (urb->status) 
	{
		urb->status = 0;
		return;
	}
	usb_serial_debug_data (__FILE__, __FUNCTION__, urb->actual_length, urb->transfer_buffer);
#if 0
	//FIXME need to update state of terminal lines variable
#endif
	return;
}

static void ch34x_read_bulk_callback (struct urb *urb)
{       
    printk("ch34x_read_bulk_callback.\n");
	struct usb_serial_port *port = (struct usb_serial_port *) urb->context;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);
	struct tty_struct *tty;
	unsigned char *data = urb->transfer_buffer;
	int i;
	int result;
	if (port_paranoia_check (port, __FUNCTION__))
		return;
	dbg("%s - port %d", __FUNCTION__, port->number);
	if (!serial) 
    {
		dbg("%s - bad serial pointer, exiting", __FUNCTION__);
		return;
	}
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
			dbg("%s - caught -EPROTO, resubmitting the urb", __FUNCTION__);
			urb->status = 0;
			urb->dev = serial->dev;
			result = usb_submit_urb(urb);
			if (result)
				err("%s - failed resubmitting read urb, error %d", __FUNCTION__, result);
			return;
		}
		dbg("%s - unable to handle the error, exiting.", __FUNCTION__);
		return;
	}
	usb_serial_debug_data (__FILE__, __FUNCTION__, urb->actual_length, data);
	tty = port->tty;
	if (tty && urb->actual_length)
    {
		for (i = 0; i < urb->actual_length; ++i)
        {
			if (tty->flip.count >= TTY_FLIPBUF_SIZE) 
            {
				tty_flip_buffer_push(tty);
			}
			tty_insert_flip_char (tty, data[i], 0);
		}
		tty_flip_buffer_push (tty);
	}
	// Schedule the next read _if_ we are still open 
	if (port->open_count)
	{
		urb->dev = serial->dev;
		result = usb_submit_urb(urb);
		if (result)
			err("%s - failed resubmitting read urb, error %d", __FUNCTION__, result);
	}
	return;
}

static void ch34x_write_bulk_callback (struct urb *urb)
{        
    dbg("ch34x_write_bulk_callback.\n");
	struct usb_serial_port *port = (struct usb_serial_port *) urb->context;
	int result;
	
	if (port_paranoia_check (port, __FUNCTION__))
		return;
	dbg("%s - port %d", __FUNCTION__, port->number);
	if (urb->status) 
	{
		/* error in the urb, so we have to resubmit it */
		if (serial_paranoia_check (port->serial, __FUNCTION__)) 
		{
			return;
		}
		dbg("%s - Overflow in write", __FUNCTION__);
		dbg("%s - nonzero write bulk status received: %d", __FUNCTION__, urb->status);
		port->write_urb->transfer_buffer_length = 1;
		port->write_urb->dev = port->serial->dev;
		result = usb_submit_urb (port->write_urb);
		if (result)
			err("%s - failed resubmitting write urb, error %d", __FUNCTION__, result);
		return;
	}
	queue_task(&port->tqueue, &tq_immediate);
	mark_bh(IMMEDIATE_BH);
	return;
	
}

static int __init ch34x_init (void)
{      
	dbg("ch34x_init.");
	usb_serial_register (&ch34x_device);
	info(DRIVER_DESC " " DRIVER_VERSION);
	return 0;
}

static void __exit ch34x_exit (void)
{       
	dbg("ch34x_exit.");
	usb_serial_deregister (&ch34x_device);
}

module_init(ch34x_init);
module_exit(ch34x_exit);

MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

MODULE_PARM(debug, "i");
MODULE_PARM_DESC(debug, "Debug enabled or not");

