[TOC]

---

https://kalumajs.org/docs/

---

# ADC

The `adc` module provides the ADC class which supports analog-digital conversion. Use `require('adc')` to access this module.

## Class: ADC

An instances of `ADC` represents a ADC object.

### new ADC(pin)

- **`pin`** `<number>` The pin number which can support ADC function.
- **Returns:** `<object>` The return value is `ADC` object.

```javascript
const { ADC } = require('adc');
var a = new ADC(26);
```

### adc.read()

- **Returns:** `<number>` The return value is ADC object.

This method returns the analog value read from the pin. A `RangeError` will be thrown if **`adc.pin`** does not support ADC function.

```javascript
const { ADC } = require('adc');
var a = new ADC(26);
var value = a.read(); // Read the ADC value at the pin 26.
```

### adc.pin

- `<number>` The pin number which can support ADC function.

The pin number of the ADC object.

```javascript
const { ADC } = require('adc');
var a = new ADC(26);
console.log(a.pin); // 26
```

# Analog I/O

## analogRead(pin)

- **`pin`** `<number>` The pin number which can support ADC function.
- **Returns:** `<number>`, the value is in between `0.0` ~ `1.0`

Read and return analog signal value from the ADC **`pin`**. A `RangeError` will be thrown if the **`pin`** does not support ADC function.

```javascript
// Read analog value on pin 26.
var pin = 26;
var value = analogRead(pin);
```

## analogWrite(pin[, duty[, frequency]])

- **`pin`** `<number>` The pin number which can support PWM function.
- **`duty`** `<number>` The PWM duty cycle between `0.0` and `1.0`. **Default:** `0.5`
- **`frequency`** `<number>` The PWM frequency in Hz. **Default:** `490`Hz

Generate PWM signal with specific **`frequency`** and **`duty`** to the PWM **`pin`**. A `RangeError` will be thrown if **`pin`** does not support PWM function.

```javascript
// Generate 490 Hz 50% duth PWM signal.
var pin = 0;
analogWrite(pin, 0.5);
```

## tone(pin[, frequency[, options]])

- **`pin`** `<number>` The pin number which can support PWM function.
- **`frequency`** `<number>` The PWM frequency in Hz. **Default:** `261.626`Hz (C Key)
- **`options`** `<object>`
  - **`duration`** `<number>` The duration in milliseconds if a duration is not 0, duration 0 is forever. **Default:** `0`
  - **`duty`** `<number>` The PWM duty cycle between `0.0` and `1.0`. **Default:** `0.5`
  - **`inversion`** `<number>` The pin number where an inverted signal to be generated. **Default:** `-1`.

Generate sound tone(PWM) on the PWM **`pin`** for **`duration`**. A **`frequency`** and **`duty`** can be set. A `RangeError` will be thrown if **`pin`** does not support PWM function.

```javascript
// Generate 300 Hz tone on the pin 0 for 1sec.
tone(0, 300, { duration: 1000 }); // Generate 300Hz tone on the pin 0 for 1sec.
```

You can generate an inverted signal of the original PWM signal on another pin. Using this you can generate louder sound on a speaker by wiring two wires of the speaker to the two pins of a same PWM slice instead of wiring to one of PWM pin and GND.

> Note that the inversion pin should be the different channel of the same slice. For example, pin 9 can be used for inversion for the pin 8. See [here](https://kalumajs.org/docs/boards/raspberry-pi-pico.md#pwm) for more about PWM slide and channels.

```javascript
// Generate 300 Hz tone on the pin 8 and the inverted signal on the pin 9.
tone(8, 300, { inversion: 9 });
```

## noTone(pin)

- **`pin`** `<number>` The pin number which can support PWM function.

Stop the tone one the PWM **`pin`**. A `RangeError` will be thrown if **`pin`** does not support PWM function.

```javascript
// Generate 200 Hz tone to the pin 0 and stop the tone after 1sec.
tone(0, 200); // Generate 200Hz tone on the pin 0.
delay(1000); // Wait for 1000ms (1sec)
noTone(0); // Stop the tone on the pin 0
```

# AT Command

> **THIS IS EXPERIMENTAL AND SUBJECT OF CHANGE**

The `at` module supports sending AT commands and handling the responses. This module is useful for the devices controlled by AT command such as Wi-Fi, GSM, Bluetooth modules. Use `require('at')` to access this module.

## Class: ATCommand

An instances of `ATCommand` represents a AT command handler. This class is a subclass of [`EventEmitter`](https://kalumajs.org/docs/api/events).

### new ATCommand(uart[, options])

- **`uart`** `<UART>` UART object where a device is connected which controlled by AT command.
- **`options`** `<object>`
  - **`debug`** `<boolean>` Print all data received from UART if `true`.

Create a AT command handler.

> Note that data received from `uart` is converted to an ASCII string (1 byte to 1 char) for easy manipulation.

```javascript
// Create an UART instance
const { UART } = require('uart');
const serial = new UART(0, { baudrate: 115200, bufferSize: 4096 });

// Create ATCommand instance
const { ATCommand } = require('at');
const at = new ATCommand(serial, { debug: true });

at.send('AT');
// ...
```

### at.send(cmd[, cb[, waitFor[, options]])

- **`cmd`** `<string|Uint8Array>` AT command (or data) to send. If `Uint8Array` type is given, it send as data.
- **`cb`** `<function(result:string)>` Callback called when expected response is arrived.
  - **`result`** `<string>` Result status. One of array items of `match` parameter or `'TIMEOUT'` or `undefined`.
- **`waitFor`** `<number|Array<string>>` Indicates what is expected response of the AT command. If a number of given, waits for the given time in milliseconds. **Default:** `['OK', 'ERROR', 'FAIL']`
- **`options`** `<object>`
  - **`timeout`** `<number>` Timeout for waiting response. **Default:** `10000` (10 sec). When timeout, callback is called with `'TIMEOUT'` for the second parameter.
  - **`sendAsData`** `<boolean>` If `true`, `cmd` is sent as data without appending `'\r\n'` in the end. **Default:** `false`.

Sends AT command and wait for expected response. If you didn't provide parameters other than `cmd`, it sends AT command and then process the next command in the queue without waiting for expected response.

```javascript
at.send('AT+XXX'); // send AT command and do not wait for response
```

If you need to get response, provide `callback` and `match` parameters. If a number is given for `match` parameter, `callback` is called with the response just after waiting response for the given number of time (milliseconds).

```javascript
at.send(
  'AT+XXX',
  () => {
    console.log();
  },
  1000
); // wait 1 sec for response
```

If an array of string is given for the `match` parameter, `callback` is called with the response and the matched item in the `match` parameter when there is a line matched with the `match` parameter in the response buffer.

```javascript
at.send(
  'AT+XXX',
  (r) => {
    if (r === 'OK') {
      console.log('DONE');
    } else {
      console.error('ERROR');
    }
  },
  ['OK', 'ERROR', 'FAIL']
);
```

### at.addHandler(match, handler)

- **`match`** `<string>` A match string.
- **`handler`** `<Function>` Handler function.
  - `line` `<string>` The matched line.
  - `buffer` `<string>` The response buffer.

Add a handler. If there is a line starts with the `match` parameter in the response buffer, the handler function will be called. The data from start to the matched line are removed from the response buffer. If you want to keep the response buffer without any changes, then just return `false`.

```javascript
at.addHandler('WIFI DISCONNECTED', (line) => {
  console.log('Wifi disconnected');
});
```

If the handler is not enough to handle just the matched line, you can manipulate the entire response buffer by returning the handled data.

```javascript
// Handle +IPD response of ESP8266
// e.g.) "+IPD,11:Hello,World"
at.addHandler('+IPD,', (line, buffer) => {
  if (...) { // if received all data in buffer
    var data = buffer.substr(...);
    var remained = buffer.substr(...);
    return remained;
  }
  return false; // if not received all data in buffer
});
```

### at.removeHandler(match)

- **`match`** `{string}`

Remove a line handler.

```javascript
at.addLineHandler('WIFI DISCONNECTED', (line) => {
  console.log('Wifi disconnected');
});

at.removeLineHandler('WIFI DISCONNECTED');
```

### at.buffer

- `{string}`

Response buffer for the responses of AT commands.

# Board

The `board` object provide the board specific properties which you are currently using.

## Object: board

### board.name

- `<string>`

The ID of the target board. ex) `pico`, ...

```javascript
console.log(board.name); // e.g.) 'pico' for Raspberry Pi Pico.
```

For more properties, please check the page for each board.

### board.LED

The pin number of the on-board LED.

### board.gpio(pin[, mode])

Returns an instance of [GPIO](https://kalumajs.org/docs/api/gpio) class. All arguments are passed to the constructor.

```javascript
var gpio = board.gpio(0, OUTPUT);
gpio.high();
```

### board.led(pin)

Returns an instance of [LED](https://kalumajs.org/docs/api/led) class. All arguments are passed to the constructor.

```javascript
var led = board.led(25);
led.on();
```

### board.button(pin[, options])

Returns an instance of [Button](https://kalumajs.org/docs/api/button) class. All arguments are passed to the constructor.

```javascript
var btn0 = board.button(0);
btn0.on('click', () => {
  console.log('button clicked');
});
```

### board.pwm(pin[, frequency[, duty]])

Returns an instance of [PWM](https://kalumajs.org/docs/api/pwm) class. All arguments are passed to the constructor.

```javascript
var pwm1 = board.pwm(1, 100, 0.4);
pwm1.start();
```

### board.adc(pin)

Returns an instance of [ADC](https://kalumajs.org/docs/api/adc) class. All arguments are passed to the constructor.

```javascript
var adc3 = board.adc(26);
adc.read(); // Read analog value from pin 26.
```

### board.i2c(bus[, options])

Returns an instance of [I2C](https://kalumajs.org/docs/api/i2c) class. All arguments are passed to the constructor.

```javascript
var i2c0 = board.i2c(0);
i2c0.write(new Uint8Array([0x6b, 0x00]), 0x68);
i2c0.close();
```

### board.spi(bus[, options])

Returns an instance of [SPI](https://kalumajs.org/docs/api/spi) class. All arguments are passed to the constructor.

```javascript
var spi0 = board.spi(0);
spi0.send(new Uint8Array([0x6b, 0x00]));
spi0.close();
```

### board.uart(port[, options])

Returns an instance of [UART](https://kalumajs.org/docs/api/uart) class. All arguments are passed to the constructor.

```javascript
var serial0 = board.uart(0);
serial0.write('Hello, world\n');
serial0.close();
```

# Button

The `button` module supports button. Use `require('button')` to access this module.

## Class: Button

An instances of `Button` represents a button object. This class is a subclass of [`EventEmitter`](https://kalumajs.org/docs/api/events).

### new Button(pin[, options])

- **`pin`** `<number>` The pin number which can support button function.
- **`options`** `<object>`
  - **`mode`** `<number>` The pin mode. Default: `INPUT_PULLUP`.
  - **`event`** `<number>` The the event of the **`pin`**. There are three events, `FALLING (0)`, `RISING (1)`, and `CHANGE (2)`. **Default:** `FALLING`.
  - **`debounce`** `<number>` debounce time in ms(milliseconds). **Default:** `50`ms

Create an instances of the `Button` class. Note that this class uses `setWatch()` function internally.

```javascript
// Create the Button instance and print the message when button is pressed.
const { Button } = require('button');
const pin = 0; // Pin number for button
const btn0 = new Button(pin);
btn0.on('click', function () {
  console.log('button 1 clicked');
});
```

### button.read()

- **Returns:** `<number>` The return value is `HIGH (1)` or `LOW (0)`

```javascript
// Create the Button instance and close it.
const { Button } = require('button');
const pin = 0; // Pin number for button
const btn0 = new Button(pin);
// ...
btn0.close();
```

### button.close()

This method closes the I/O watcher for the button.

```javascript
// Create the Button instance and close it.
const { Button } = require('button');
const pin = 0; // Pin number for button
const btn0 = new Button(pin);
btn0.on('click', function () {
  console.log('button 1 clicked');
});
// ...
btn0.close();
```

### Event: 'click'

The `click` event is emitted when the button is pressed down.

```javascript
// Create the Button instance and toggle LED when the button is pressed.
const { Button } = require('button');
const pin = 0; // Pin number for button
const btn0 = new Button(pin);
const led = 25; // LED
pinMode(led, OUTPUT);
btn.on('click', function () {
  digitalToggle(led);
});
```

# Console

The `console` object provide the simple debugging console which provide a log and error message.

## Object: console

Console object is a global object.

### console.log([...data])

- **`...data`** `<any>`Data which is shown in the console.

Print out the data. to the console. The all data are converted to String object automatically so there's no need to change it to String object.

```javascript
// print out "100 string value true 1,2,3"
var number = 100;
var result = true;
var arr = [1, 2, 3];
console.log(number, 'string value', result, arr);
```

### console.error([...data])

- **`...data`** `<any>`Data which is shown in the console.

Print out the data with red color to the console. The all data and arguments are converted to String object automatically so there's no need to change it to String object.

```javascript
// print out "100 string value true 1,2,3"
var number = 100;
var result = true;
var arr = [1, 2, 3];
console.error(number, 'string value', result, arr);
```

# PICO-CYW43

> **THIS IS EXPERIMENTAL AND SUBJECT OF CHANGE**

The `pico_cyw43` module is for the Raspberry Pi Pico W board.

## Class: PicoCYW43

An instance of `PicoCYW43` represents a Pico-W WiFi module.

### new PicoCYW43()

Create an instance of `PicoCYW43` class.

```javascript
const { PicoCYW43 } = require('pico_cyw43');
const pico_cyw43 = new PicoCYW43();
```

### PicoCYW43.getGpio(gpio)

- **`gpio`**`<number>` CYW43 chip gpio number
- **Returns**: `<boolean>` true if the GPIO is high, false if the GPIO is low.

### PicoCYW43.putGpio(gpio, value)

- **`gpio`**`<number>` CYW43 chip gpio number
- **`value`**`<boolean>` true to set the GPIO to high, false to set the GPIO to low.

```javascript
const { PicoCYW43 } = require('pico_cyw43');
const pico_cyw43 = new PicoCYW43();

// Blink the Pico-W's on-board LED
setInterval(() => {
  if (pico_cyw43.getGpio(0)) {
    pico_cyw43.putGpio(0, false); // turn-off LED
  } else {
    pico_cyw43.putGpio(0, true); // turn-on LED
  }
}, 1000);
```

## Class: PicoCYW43WIFI

> **In Pico-W board, this driver is automatically registered as a default IEEE80211 driver. User don't need to register manually.**

This class implements the [IEEE802.11 device driver](https://kalumajs.org/docs/api/device-driver/#ieee-80211-wifi-device-driver) for the CYW43 network module.

In Pico-W this driver is registered as a default IEEE802.11 device driver, but you can also register it manually as below:

```js
const { PicoCYW43WIFI } = require('pico_cyw43');
global.__ieee80211dev = new PicoCYW43WIFI();
```

This class also has a WIFI AP (Access Pointer) function. So Pico-W board can be used to WIFI AP.

### pico_cyw43.wifiApMode(apInfo, [callback])

- **`apInfo`** `{object}`
  - **`ssid`** `{string}` SSID.
  - **`password`** `{string}` password of the WIFI AP.
  - **`gateway`** `{string}` Gateway of the WIFI AP. Default: `192.168.4.1`
  - **`subnetMask`** `{string}` Subnet Maks of the WIFI AP. Default: `255.255.255.0`

Start WIFI AP mode.

### pico_cyw43.disableWifiApMode()

Disable WIFI AP mode.

### pico_cyw43.getWifiApClients()

- Returns: `<{Array<string>}>`
  - **`bssid`** `{string}` _**_BSSID. (Typically MAC address)

Get the list of the clients's bssid (MAC address).

```js
const { PicoCYW43WIFI } = require('pico_cyw43');
const picoWifi = new PicoCYW43WIFI();

var apCallBack = function(err) {
    console.log("WIFI ap is enabled - err: " + err);
}

var apOptions = {
  // WIFI AP options
  ssid: "raspberry_pi_ap",
  password: "12345678",
  gateway: "192.168.4.1",
  subnet_mask: "255.255.0.0"
};

picoWifi.wifiApMode(apOptions, apCallBack);
```

## Class: PicoCYW43Network

> **In Pico-W board, this driver is automatically registered as a default network driver. User don't need to register manually**

This class implements the [network device driver](https://kalumajs.org/docs/api/device-driver#network-device-driver) for CYW43 network module.

> This module support TCP protocol only.

In Pico-W this driver is registered as a default network driver, but you can also register it manually as below:

```javascript
const { PicoCYW43Network } = require('pico_cyw43');
global.__netdev  = new PicoCYW43Network();
```

# Device Driver

> **THIS IS EXPERIMENTAL AND SUBJECT OF CHANGE**

To provide device independent API, device driver should be provided for a particular hardware device.

For example, if you want to use networking API with ESP8266 module externally connected via UART, you need to provide ESP8266 device driver.

Below is supported device driver types:

- `global.__netdev` : Network device driver. The [`net`](https://kalumajs.org/docs/api/net), `dgram`, [`http`](https://kalumajs.org/docs/api/http) modules require this device driver.
- `global.__ieee80211dev` : IEEE 802.11 (WiFi) device driver. The [`wifi`](https://kalumajs.org/docs/api/wifi) module requires this device driver.

## Network Device Driver

The interface of the network device driver is very similar to the linux socket interface.

```typescript
// Interface of a socket object
interface socket {
  fd: number; // socket file descriptor
  ptcl: string; // 'STREAM' || 'DGRAM'
  state: number; // 0=closed, 1=bind, 2=connected, 3=listening
  laddr: string;
  lport: number;
  raddr: string;
  rport: number;
  connect_cb: () => void;
  close_cb: () => void;
  read_cb: (data: string) => void;
  accept_cb: (fd: number) => void;
  shutdown_cb: () => void;
}

// Interface of a network device driver
interface netdev {
  errno: number;
  mac: string; // mac address
  ip: string; // local ip address
  socket(domain: string, protocol: string): number;
  get(fd: number): socket;
  connect(
    fd: number,
    addr: string,
    port: number,
    callback: (err: number) => void
  ): void;
  write(fd: number, data: string, callback: (err: number) => void): void;
  close(fd: number, callback: (err: number) => void): void;
  shutdown(fd: number, how: number, callback: (err: number) => void): void;
  bind(
    fd: number,
    addr: string,
    port: number,
    callback: (err: number) => void
  ): void;
  listen(fd: number, callback: (err: number) => void): void;
}
```

### netdev.errno

- `{number}`

The numeric representation of error code.

### netdev.mac

- `{string}`

The string representation of mac address.

### netdev.ip

- `{string}`

The string representation of IP address.

### netdev.socket(domain, protocol)

- **`domain`** `{string}` Communication domain.
- **`protocol`** `{string}` Protocol `'STREAM'` or `'DGRAM'`.
- Returns: `{number}` A socket file descriptor. `-1` on error. Error code is set to `errno`.

### netdev.get(fd)

- **`fd`** `{number}` A socket file descriptor.

- Returns: `{Object}` A socket object.

  - **`fd`** `{number}` socket file descriptor.
  - **`ptcl`** `{string}` Protocol. `'STREAM'` or `'DGRAM'`.
  - **`state`** `{number}` The state of socket. `0` is closed, `1` is bound, `2` is connected, and `3` is listening.
  - **`laddr`** `{string}` Local address.
  - **`lport`** `{number}` Local port.
  - **`raddr`** `{string}` Remote address.
  - **`rport`** `{number}` Remote port.
  - **`connect_cb`** `{Function}` Called when connected.
  - **`close_cb`** `{Function}` Called when closed.
  - **`read_cb`** `{Function}` Called when a chunk of data received.
    - **`data`** `{Uint8Array}` Received data.
  - **`accept_cb`** `{Function}` Called when accepted a new connection
    - **`fd`** `{number}` the socket file descriptor of the new connection.
  - **`shutdown_cb`** `{Function}` Called when shutdown.

Returns the socket object correspond to the given `fd`.

### netdev.connect(fd, addr, port[, callback])

- **`fd`** `{number}` socket file descriptor.

- **`addr`** `{string}` Host address.

- **`port`** `{number}` Port number.

- **`callback`** `{Function}`

  - **`err`** `{number}` `0` on success, `-1` on error. Error code is set to `errno`

Establishes a socket connect. When successfully connected, the socket's `connect_cb` is called.

### netdev.write(fd, data[, callback])

- **`fd`** `{number}` socket file descriptor.

- **`data`** `{Uint8Array|string}` Data to send.

- **`callback`** `{Function}` Called when the data is written.

  - **`err`** `{number}` `0` on success, `-1` on error. Error code is set to `errno`

Sends data on the socket connection.

### netdev.close(fd[, callback])

- **`fd`** `{number}` A socket file descriptor.

- **`callback`** `{Function}`  Called when the socket is closed.

  - **`err`** `{number}` `0` on success, `-1` on error. Error code is set to `errno`

Closes the socket connection. When successfully closed, the socket's `close_cb` is called.

### netdev.shutdown(fd, how[, callback])

- **`fd`** `{number}` A socket file descriptor.

- **`how`** `{number}` `0` is to shut read, `1` is to shut write, and `2` is to shut both.

- **`callback`** `{Function}`  Called when the socket is shutdown.

  - **`err`** `{number}` `0` on success, `-1` on error. Error code is set to `errno`

Shutdown the socket. When successfully shutdown, the socket's `shutdown_cb` is called.

### netdev.bind(fd, addr, port[, callback])

- **`fd`** `{number}` A socket file descriptor.

- **`addr`** `{string}` Host address.

- **`port`** `{number}` Port number.

- **`callback`** `{Function}` Called when the socket is bound.

  - **`err`** `{number}` `0` on success, `-1` on error. Error code is set to `errno`

Bind the socket to the given address and port.

### netdev.listen(fd[, callback])

- **`fd`** `{number}` A socket file descriptor.

- **`callback`** `{Function}`  Called when listening started.

  - **`err`** `{number}` `0` on success, `-1` on error. Error code is set to `errno`

Starts to listening for connections. When a new connection is accepted, the socket's `accept_cb` is called.

## IEEE 802.11 (WiFi) Device Driver

The IEEE 802.11 (Wi-Fi) device driver should provide the interface described in this section.

```typescript
// Interface for IEEE 802.11 device driver
interface ieee80211dev {
  errno: number;
  assoc_cb: () => void;
  connect_cb: () => void;
  disconnect_cb: () => void;
  reset(cb: (err: number) => void): void;
  scan(cb: (err: number, scan_info: Array<scan_info>) => void): void;
  connect(cb: (err: number) => void): void;
  disconnect(cb: (err: number) => void): void;
  get_connection(cb: (err: number, conn: connection_info) => void): void;
}

interface scan_info {
  bssid: string;
  ssid: string;
  security: string;
  rssi: number;
  channel: number;
}

interface connection_info {
  ssid: string;
  password: string;
  security: string;
}
```

### ieee80211dev.assoc_cb

- `{Function}`

This callback is called when wireless network is associated.

### ieee80211dev.connect_cb

- `{Function}`

This callback is called when wireless network is connected (IP assigned).

### ieee80211dev.disconnect_cb

- `{Function}`

This callback is called when current wireless network is disconnected.

### ieee80211dev.reset([callback])

- **`callback`** `{Function}`  Called when reset complete.

  - **`err`** `{number}` `0` on success, `-1` on error. Error code is set to `errno`

Reset the device.

### ieee80211dev.scan([callback])

- **`callback`** `{Function}`  A callback function to be called when scan complete.

  - **`err`** `{number}` `0` on success, `-1` on error. Error code is set to `errno`

  - **`scan_results`** `{Array<Object>}`

    - **`security`** `{string}` Security. `OPEN`, or multiple of `WEP`, `WPA`, `PSK`, `WPA2`, `WPA2-EAP`.
    - **`ssid`** `{string}` SSID.
    - **`rssi`** `{number}` Received signal strength indication.
    - **`bssid`** `{string}` _**_BSSID. (Typically MAC address)
    - **`channel`** `{number}` Channel.

Scan wireless networks.

### ieee80211dev.connect(connectInfo[, callback])

- **`connectInfo`** `{object}`  Information of the wireless network to connect .

  - **`ssid`** `{string}` SSID.
  - **`password`** `{string}` Password. Optional.
  - **`security`** `{string}` Security. `OPEN`, `WPA2_WPA_PSK`, `WPA2_PSK`, `WPA_PSK`, `WEP_PSK`. Optional. The defaule value is `OPEN` if password is not set or length of the password is less than 8 characters, or `WPA2_WPA_PSK` if length of the password is greater or equal to 8 characters.

- **`callback`** `{Function}` A callback function.

  - **`err`** `{number}` `0` on success, `-1` on error. Error code is set to `errno`

Connect to a wireless network. The callback `onConnected` should be called when successfully disconnected.

### ieee80211dev.disconnect([callback])

- **`callback`** `{Function}`

  - **`err`** `{number}` `0` on success, `-1` on error. Error code is set to `errno`

Disconnect from the currently connected wireless network. The callback `onDisconnected` should be called when successfully disconnected.

### ieee80211dev.get_connection([callback])

- **`callback`** `{Function}`

  - **`err`** `{number}` `0` on success, `-1` on error. Error code is set to `errno`

  - **`connectionInfo`** `{object}` Information of the connected wireless network.

    - **`ssid`** `{string}`
    - **`bssid`** `{string}`

Request connection information.

# Digital I/O

## pinMode(pin[, mode])

- **`pin`** `<number|number[]>` The pin number (or array of pin numbers) which can support GPIO function.

- **`mode`** `<number>` The pin mode `INPUT (0)` or `OUTPUT (1)`or `INPUT_PULLUP (2)` **** or `INPUT_PULLDOWN (3)`**.**

  . **Default:** `INPUT`

Set the mode of the GPIO **`pin`** to `INPUT` , `OUTPUT`, `INPUT_PULLUP`, or `INPUT_PULLDOWN`. A `RangeError` will be thrown if **`pin`** does not support GPIO function. Use `INPUT_PULLUP`when you want to enable internal pull-up for this **`pin`** and use `INPUT_PULLDOWN` when you want to enable internal pull-down.

```javascript
// Set pin 1 to output mode.
pinMode(1, OUTPUT); // Set the pin 1 to output mode.
digitalWrite(1, HIGH); // Set the pin 1 to HIGH.
```

You can set mode for multiple GPIO pins at once by passing an array of pin numbers.

```javascript
pinMode([0, 1, 2, 3], OUTPUT);

// equivalent to
pinMode(0, OUTPUT);
pinMode(1, OUTPUT);
pinMode(2, OUTPUT);
pinMode(3, OUTPUT);
```

## digitalRead(pin)

- **`pin`** `<number>` The pin number which can support GPIO function.
- **Returns:** `<number>` The return value is `HIGH (1)` or `LOW (0)`

Read the digital input from the GPIO INPUT **`pin`**. A `RangeError` will be thrown if **`pin`** does not support GPIO function.

```javascript
// Read digital value from the pin 1.
pinMode(1, INPUT); // Set the pin 1 to input mode
var value = digitalRead(1); // Read the value from the pin 1.
```

## digitalWrite(pin[, value])

- **`pin`** `<number|number[]>` The pin number (or array of pin numbers) which can support GPIO function.
- **`value`** `<number>` The value could be `HIGH (1)` or `LOW (0)`. If an array of pin numbers are given a number (greater than 1) can be used. **Default:** `LOW`

Set the GPIO OUTPUT **`pin`** to `HIGH` or `LOW`. A `RangeError` will be thrown if **`pin`** does not support GPIO function.

```javascript
// Set the pin 1 to HIGH.
pinMode(1, OUTPUT); // Set the pin 1 to output mode
digitalWrite(1, HIGH); // Set the pin 1 to HIGH
```

You can set multiple GPIO pins at once by passing an array of pin numbers. The last element in the array corresponds to the last significant bit (LSB) of the value.

```javascript
var pins = [0, 1, 2, 3, 4, 5, 6, 7];
digitalWrite(pins, 0b11001010);

// equivalent to
digitalWrite(0, HIGH);
digitalWrite(1, HIGH);
digitalWrite(2, LOW);
digitalWrite(3, LOW);
digitalWrite(4, HIGH);
digitalWrite(5, LOW);
digitalWrite(6, HIGH);
digitalWrite(7, LOW);
```

## digitalToggle(pin)

- **`pin`** `<number>` The pin number which can support GPIO function.

Set the GPIO OUTPUT **`pin`** to the reverse state of the current state. Set to `HIGH` if the current state is `LOW`. Set to `LOW` if the current state is `HIGH`. A `RangeError` will be thrown if **`pin`** does not support GPIO function.

```javascript
// Set the pin 1 to HIGH and toggle (Set to LOW)
pinMode(1, OUTPUT); // Set the pin 1 to GPIO OUTPUT
digitalWrite(1, HIGH); // Set the pin 1 to HIGH
digitalToggle(1); // Toggle the pin 1, change the pin state to LOW from HIGH.
```

## setWatch(callback, pin[, events[, debounce]])

- **`callback`** `<function>` The function is called when the `event`  is triggered on the `pin.`

  - **`pin`** `<number>` The pin number is passed as the first argument to the callback.

- **`pin`** `<number>` The pin number which can support GPIO function.

- **`events`** `<number>` set the events of the `pin`. There are five events, `LOW_LEVEL (1)`, `HIGH_LEVEL (2)` `FALLING (4)`, `RISING (8)`, and `CHANGE (12)`. **Default:** `CHANGE`.

- **`debounce`** `<number>` debounce time in ms (milliseconds). **Default:** `0`ms

- **Returns:** `<number>` the ID of the watcher.

Run the **`callback`** function when the **`events`** is triggered on the **`pin`**. There are five events. The `FALLING` event is triggered when the **`pin`** state is changed from `HIGH` to `LOW`. The `LOW_LEVEL (1)`event is triggered when the **`pin`** is `LOW` state and The `HIGH_LEVEL (2)` event is triggered when the **`pin`** is `HIGH` state. The `RISING (1)` event is triggered when the **`pin`** state is changed from `LOW` to `HIGH`. The `CHANGE` event is triggered when the **`pin`** state is changed to any states, which means the `CHANGE` event is the same as the `FALLING` + `RISING` events. The **`debounce`** time can be set when you can see the bouncing on the GPIO **`pin`**. A `RangeError` will be thrown if **`pin`** does not support GPIO function.

Before calling this function, you have to set the pin mode as `INPUT` , `INPUT_PULLUP` or `INPUT_PULLDOWN`.

```javascript
// Print out 'click' string to the terminal when user press the button.
var pin = 0; // Pin to watch
pinMode(pin, INPUT_PULLDOWN); // Set the pin mode to INPUT_PULLDOWN.
var id = setWatch(
  function () {
    console.log('click'); // Print out the 'click' to the terminal.
  },
  pin,
  RISING,
  10
); // Set falling event with 10ms debouncing time.
```

## clearWatch(id)

- **`id`** `<number>` The ID of the watcher which is the return value of the `setWatch()` function.

Stop watching the event which is set by `setWatch()` function.

```javascript
// Print out 'click' string to the terminal when user press the button.
var pin = 0; // Pin to watch
pinMode(pin, INPUT_PULLDOWN); // Set the pin mode to INPUT_PULLDOWN.
var id = setWatch(
  function () {
    console.log('click'); // Print out the 'click' to the terminal.
  },
  pin,
  RISING,
  10
); // Set falling event with 10ms debouncing time.
clearWatch(id); // Stop watching the on-board pin event.
```

## pulseRead(pin, count[, options])

- **`pin`** `<number>` The pin number which can support GPIO function.

- **`count`** `<number>` The number of pulse to read.

- **`options`** `<object>` 

  - **`timeout`** `<number>` timeout in us(microseconds). **Default:** `1000000`

  - **`startState`** `<number>` Start to read the pulse from this state. **Default:** `undefined`

  - **`mode` ** `<number>` Pin mode to read pulse.

  - **`trigger`** `<object>`

    - **`pin`** `<number>` Pin number to trigger. **Default:** same with `pin`
    - **`startState`** `<number>` Start state for trigger. **Default:** `LOW`
    - **`interval`** `<number[]>` Pulse timing in microseconds. Refer to `pulseWrite`.

- **Returns:** `<number[]>` Array of the pulse timing. It returns `null` if the pin state is not changed until **`timeout`**

Read the **`pin`** state change timing from **`startState`**. It returns the microseconds state changing timing if the state is changed **`count`** times or for **`timeout`** microseconds.

```javascript
// Read 10 pulse timing on the pin 0
pinMode(0, INPUT);
var pulse = pulseRead(0, 10);
console.log(pulse);
```

If **`startState`** is `HIGH (1)` the system wait until the pin state to `HIGH` and start to measure pulse timing. The If **`startState`** is`undefined`, the system start to measure pulse timing immediately and user can't know which pulse timing is `HIGH` or `LOW`. Assume that the pulses are given as below timing diagram, you can get `[30, 20, 50]` with the below example code.

```javascript
// Read 10 pulse timing with 50ms timeout.
pinMode(0, INPUT);
// Wait until HIGH state detection
var pulse = pulseRead(0, 3, { timeout: 50000, startState: HIGH });
console.log(pulse); // [30, 20, 50]
```

![A pulse example](.assest/api/doc-digital-io-pulse-read.png)

If **`trigger`** options is given, it generates trigger pulse just before to read pulse. The fields of trigger object is corresponds to the parameter of `pulseWrite()`. It is useful to avoid the delay time between triggering and pulse reading.

```javascript
// 10us trigger pulse before reading
var pulse = pulseRead(0, 10, {
  mode: INPUT,
  trigger: {
    pin: 1,
    startState: LOW,
    interval: [2, 10];
  }
});
console.log(pulse);
```

You should be consider the CPU process time in the very first output array data. This means that the very first timing data is less than your expected timing due to CPU process time.

## pulseWrite(pin, startState, interval)

- **`pin`** `<number>` The pin number which can support GPIO function.
- **`startState`** `<number>` Start state.
- **`interval`** `<number[]>` Pulse timing in microseconds.
- **Returns:** `<number>` Length of the written pulse, it's the same as the length of the **`interval`** array.

Generates the digital pulse on the **`pin`** with microseconds timing. The state change timing from **`startState`**. It returns the number of the written pulse.

```javascript
// Generates pulse on the pin 0
pinMode(0, OUTPUT);
pulseWrite(0, LOW, [60, 30, 20, 50, 20]);
```

![Pulse generated](.assest/api/doc-digital-io-pulse-write.png)

# Errors

## Class: SystemError

- Extends: <Error>

This class represents internal system error. Please check all error codes in [err.h](https://github.com/kameleon-project/kameleon/blob/master/include/err.h).

### error.errno

- `<number>`

An error code defined in [err.h](https://github.com/kameleon-project/kameleon/blob/master/include/err.h).

### error.code

- `<string>`

A string representation of `errno`.

# Events

- The `events` module supports events. Use `require('events')` to access this module.

## Class: EventEmitter

An instance of `EventEmitter` class will emits events. If you need to define a class emitting events, you can defined a class by extending from `EventEmitter`.

```javascript
const { EventEmitter } = require('events');

class MyEmitter extends EventEmitter {}

var myEmitter = new MyEmitter();

myEmitter.on('event', () => {
  console.log('event occurred!');
});

myEmitter.emit('event');
```

### emitter.addListener(eventName, listener)

- **`eventName`** `{string}` The name of event
- **`listener`** `{function}` The callback function

Add the `listener` function to the end of the listener array for the event named `eventName`.

```javascript
server.addListener('connect', function () {
  // equivalent to server.on
  console.log('server connected');
});
```

### emitter.emit(eventName[, ...args])

- **`eventName`** `{string}` The name of event
- **`...args`** `{any}` Arguments to be passed to the listener functions.

Synchronously calls each of the listeners registered for the event named `eventName`, in the order they were registered, passing the supplied arguments to each.

```javascript
server.on('data', function (data) {
  console.log(data); // "data received"
});

var data = 'data recived';
server.emit('data', data);
```

### emitter.on(eventName, listener)

- **`eventName`** `{string}` The name of event.
- **`listener`** `{function}` The callback function.

Alias for `emitter.addListener(eventName, listener)`.

```javascript
server.on('connect', function () {
  console.log('server connected');
});
```

### emitter.once(eventName, listener)

- **`eventName`** `{string}` The name of event.
- **`listener`** `{function}` The callback function.

Add one-time `listener` for the event named `eventName`. When the event is triggered, the `listener` function is called and then removed from the listener array of the event.

```javascript
server.once('connect', function () {
  console.log('connected');
});

server.emit('connect'); // 'connected' printed
server.emit('connect'); // nothing printed
```

### emitter.removeListener(eventName, listener)

- **`eventName`** `{string}` The name of event.
- **`listener`** `{function}` The callback function.

Remove the `listener` function from the listener array of the event name `eventName`.

```javascript
function connectListener () {
  console.log('connected');
}

server.addListener('connect', connectListener); // eq to server.on
server.emit('connect'); // 'connected' printed

server.removeListener('connect' connectListener); // eq to server.off
server.emit('connect'); // nothing printed
```

### emitter.removeAllListeners([eventName])

- **`eventName`** `{string}`

Remove all listeners of the event named `eventName`.

### emitter.off(eventName, listener)

- **`eventName`** `{string}` The name of event.
- **`listener`** `{function}` The callback function

Alias for `emitter.removeListener(eventName, listener)`.

### emitter.listeners(eventName)

- **`eventName`** `{string}` The name of event.
- **Returns:** `{Array<Function>}`

Return all listeners of the given event.

### emitter.listenerCount(eventName)

- **`eventName`** `{string}` The name of event.
- **Returns:** `{number}`

Return the number of listeners of the given event.

# File system

> **THIS IS EXPERIMENTAL AND SUBJECT OF CHANGE**

## File system

The `fs` module supports file system API. Use `require('fs')` to access this module.

```javascript
const fs = require('fs');
```

> All file I/O functions are synchronous. Currently do not support asynchronous file I/O.

### fs.register(fstype, fsctr)

- **`fstype`** `<string>` File system type.
- **`fsctr`** `<constructor>` Virtual file system constructor. See [Virtual File System](https://kalumajs.org/docs/api/file-system#virtual-file-system).

Register a file system type.

```javascript
const fs = require('fs');
const {VFSLittleFS} = require('vfs_lfs');

// register lfs filesystem type
fs.register('lfs', VFSLittleFS);

// then you can mkfs or mount with a block device
const blkdev = ...
fs.mount('/', blkdev, 'lfs');
```

### fs.unregister(fstype)

- **`fstype`** `<string>` File system type.

Unregister a file system type.

### fs.mkfs(blkdev, fstype)

- **`blkdev`** `<BlockDevice>` Block device.
- **`fstype`** `<string>` File system type.

Make a file system (format) on the block device.

> Note that calling`mkfs()` will delete all files if there were already existed.

```javascript
const fs = require('fs');
const {VFSLittleFS} = require('vfs_lfs');

// register lfs filesystem type
fs.register('lfs', VFSLittleFS);

// make lfs file system (format)
const blkdev = ...
fs.mkfs(blkdev, 'lfs');
```

### fs.mount(path, blkdev, fstype[, mkfs])

- **`path`** `<string>` A path where the block device to mount.
- **`blkdev`** `<BlockDevice>` Block device.
- **`fstype`** `<string>` File system type.
- **`mkfs`** `<boolean>` Optional. Try to call `mkfs()` first when mount failed. Default: `false`.

Mount a block device to the given path. When mount is failed, try to call `mkfs()` before mount retry if you pass `true` to `mkfs` parameter.

```javascript
const fs = require('fs');
const {VFSLittleFS} = require('vfs_lfs');

// register lfs filesystem type
fs.register('lfs', VFSLittleFS);

// mount lfs with mkfs try
const blkdev = ...
fs.mount('/', blkdev, 'lfs', true);
```

### fs.unmount(path)

- **`path`** `<string>` A path where the block device to mount.

Unmount the block device on the given path.

### fs.cwd()

- **Returns**: `<number>`

Return the current working directory.

### fs.chdir(path)

**`path`** `<string>` A path to change.

Change the current working directory.

### fs.open(path[, flags[, mode]])

- **`path`** `<string>` A file path to openBinary data to encode.
- **`flags`** `<string>` File system flags. **Default:** `'r'`.
- **`mode`** `<number>` (Not supported yet).
- **Returns**: `<number>` a file descriptor.

Open a file and returns an integer file descriptor.

Following flags are available.

- `'r'` : Open file for reading.
- `'r+'` : Open file for reading and writing. The file is created if not exists.
- `'w'` : Open file for writing. The file is created if not exists.
- `'w+'` : Open file for reading and writing. The file is created if not exists.
- `'wx'` : Open file for writing, but fails if exists.
- `'wx+'` : Open file for reading and writing, but fails if exists.
- `'a'` : Open file for appending.
- `'a+'` : Open file for reading and appending.
- `'ax'` : Open file for appending, but fails if exists.
- `'ax+'` : Open file for reading and appending, but fails if exists.

### fs.read(fd, buffer[, offset[, length[, position]]])

- **`fd`** `<number>`
- **`buffer`** `<TypedArray>` The buffer will be written.
- **`offset`** `<number>` The position in buffer to write. Default: `0`.
- **`length`** `<number>` The number of bytes to read. Default: the length of `buffer`.
- **`position`** `<number>` The position of the file to read from. Default is current position if not specified.
- **Returns**: `<number>` The number of bytes read.

Read data from the file specified by `fd`.

```javascript
const fs = require('fs');
let fd = fs.open('file.txt');
let buffer = new Uint8Array(10);
// read 10 bytes from the start
fs.read(fd, buffer, 0, buffer.length, 0);
fs.close(fd);
```

### fs.write(fd, buffer[, offset[, length[, position]]])

- **`fd`** `<number>`
- **`buffer`** `<TypedArray>` The buffer will be read.
- **`offset`** `<number>` The position in buffer to read. Default: `0`.
- **`length`** `<number>` The number of bytes to write. Default: the length of `buffer`.
- **`position`** `<number>` The position of the file to write. Default is current position if not specified.
- **Returns**: `<number>` The number of bytes written.

Write data to the file specified by `fd`.

```javascript
const fs = require('fs');
let fd = fs.open('file.txt');
let buffer = new Uint8Array(10);
// buffer[0] = ...
// buffer[1] = ...
// ...

// write 10 bytes
fs.write(fd, buffer, 0, buffer.length, 0);
fs.close(fd);
```

### fs.close(fd)

- **`fd`** `<number>`

Close the file specified by `fd`.

### fs.readFile(path)

- **`path`** `<string>`
- **Returns**: `<Uint8Array>` .

Read data from the file and return the data.

```javascript
const fs = require('fs');
let data = fs.readFile('file.txt');
```

### fs.writeFile(path, data)

- **`path`** `<string>`
- **`data`** `<Uint8Array>`

Write data to the file specified by `path`.

```javascript
const fs = require('fs');
let buffer = new Uint8Array(10);
// buffer[0] = ...
// buffer[1] = ...
// ..
fs.writeFile('file.txt', buffer);
```

### fs.unlink(path)

- **`path`** `<string>`

Remove the file specified by `path`.

### fs.stat(path)

- **`path`** `<string>`
- **Returns**: `<fs.Stats>`

Return status of the file specified by `path`.

### fs.rename(oldPath, newPath)

- **`oldPath`** `<string>`
- **`newPath`** `<string>`

Rename the file or directory.

### fs.exists(path)

- **`path`** `<string>`

Test whether or not the given path exists.

### fs.mkdir(path)

- **`path`** `<string>`

Create a directory.

### fs.rmdir(path)

- **`path`** `<string>`

Remove a directory.

### fs.rm(path)

- **`path`** `<string>`

Remove a file or directory specified by the `path`.

### fs.readdir(path)

- **`path`** `<string>`
- **Returns:** <`string[]`>

Read the contents of a directory.

### Class: fs.Stats

A class representing file status. The objects returned by `fs.stat()` are of this type.

#### stats.isDirectory()

- **Returns**: `<boolean>`

Test whether the file is directory or not.

#### stats.isFile()

- **Returns**: `<boolean>`

Test whether the file is typical file or not.

#### stats.size

- `<number>`

The size of the file.

## Virtual File System

The architecture of file system is resembles Unix's virtual file system. So you can implements your own virtual file system and register. By default, Kaluma provides two file systems: [LittleFS](https://github.com/littlefs-project/littlefs) and [FAT](http://elm-chan.org/fsw/ff/00index_e.html).

```javascript
const fs = require('fs');

// littlefs
const { VFSLittleFS } = require('vfs_lfs');
fs.register('lfs', VFSLittleFS);

// fat
const { VFSFatFS } = require('vfs_fat');
fs.register('lfs', VFSFatFS);
```

### Implementing VFS

You can also implements your own VFS and register it. The VFS class should implements below interface.

```typescript
interface VFS {
  constructor(blkdev: BlockDevice)
  mkfs(): void;
  mount(): void; // throw exception if blkdev is not formatted
  unmount(): void;
  open(path: string, flags: number, mode: number) => number; // id
  write(id: number, buffer: Uint8Array, offset: number, length: number, position: number): number // bytes written
  read(id: number, buffer: Uint8Array, offset: number, length: number, position: number): number // bytes read
  close(id: number): void;
  stat(path: string): {type:number /*1=file,2=dir*/, size:number}
  mkdir(path: string): void;
  rmdir(path: string): void;
  readdir(path: string): string[];
  rename(oldPath: string, newPath: string): void;
  unlink(path: string): void;
}
```

### Block Device

Once you have registered file systems, you can mount block devices.

```javascript
const fs = require('fs');

// block device for flash memory
const blkdev = FlashBD(0, 128);

// format and mount littlefs
fs.mkfs(blkdev, 'lfs');
fs.mount('/', blkdev, 'lfs');

// shortly, mount (format first if not formatted)
fs.mount('/', blkdev, 'lfs', true);
```

You can implement your own block devices based on RAM, SDCard, PSRAM, and more by implementing below interface.

```javascript
interface BlockDevice {
  read(block: number, buf: Uint8Array, offset: number): void;
  write(block: number, buf: Uint8Array, offset: number): void;
  ioctl(op: number, arg: number): number;
  // op = 1: initialize the device
  // op = 2: shutdown the device
  // op = 3: synchronize the device
  // op = 4: returns the total number of blocks
  // op = 5: returns the number of bytes in a block
  // op = 6: erase a block (arg = block num)
  // op = 7: returns the buffer size
}
```

Here is an RAMBlockDev example.

```javascript
class RAMBlockDev {
  constructor() {
    this.blocksize = 4096;
    this.blockcount = 16;
    this.buffersize = 256;
    this.buf = new Uint8Array(this.blocksize * this.blockcount);
    this.buf.fill(0);
  }
  read(block, buffer, offset = 0) {
    for (let i = 0; i < buffer.length; i++) {
      buffer[i] = this.buf[block * this.blocksize + offset + i];
    }
  }
  write(block, buffer, offset = 0) {
    for (let i = 0; i < buffer.length; i++) {
      this.buf[block * this.blocksize + offset + i] = buffer[i];
    }
  }
  ioctl(op, arg) {
    switch (op) {
      case 1: // init
        return 0;
      case 2: // shutdown
        return 0;
      case 3: // sync
        return 0;
      case 4: // block count
        return this.blockcount;
      case 5: // block size
        return this.blocksize;
      case 6: // erase block
        let p = arg * this.blocksize;
        for (let i = p; i < p + this.blocksize; i++) {
          this.buf[i] = 0;
        }
        return 0;
      case 7: // buffer size
        return this.buffersize;
    }
    return -1; // unknown op
  }
}

const fs = require('fs');
const blkdev = RAMBlockDev();
fs.mount('/ram', blkdev, 'lfs', true);
```

# Flash

The `flash` module provides block device for flash memory. Use `require('flash')` to access this module.

> Do not use this class without knowing about flash allocation map. Writing and erasing blocks in flash memory may broke your code, files, storage items in flash.

## Class: Flash

An instance of `Flash` represents a block device.

### new Flash(base, count)

- **`base`** `<number>` Starting block number.
- **`count`** `<number>` Number of blocks.

Returns a block device instance.

### flash.read(block, buffer[, offset])

- **`block`** `<number>` Block number to read.
- **`buffer`** `<Uint8Array>` Buffer where the read data to be stored.
- **`offset`** `<number>` Offset of `buffer`.

Read the block data and store in `buffer`.

### flash.write(block, buffer[, offset])

- **`block`** `<number>` Block number to write.
- **`buffer`** `<Uint8Array>` Data to write.
- **`offset`** `<number>` Offset of `buffer`.

Write the `buffer` data at the block.

### flash.ioctl(op[, arg])

- **`op`** `<number>` I/O operation.
- **`arg`** `<number>` Argument of the `op`.

Perform an I/O operation.

I/O operations are below:

- `1` : Initialize the device (not supported)
- `2` : Shutdown the device (not supported)
- `3` : Synchronize the device (not supported)
- `4` : Returns the total number of blocks
- `5` : Returns the number of bytes in a block
- `6` : Erase a block (`arg` is the block number to erase)
- `7` : Returns the flash page size

# Global

The objects are available in a global scope. The objects in the Global are not need to have a require function call before using it.

## Object: global

### require(module_name)

- **`module_name`** `<string>`Module Name which would be used in the script.
- **Returns:** `<any>` Return `exports` object of the loaded module.

```javascript
var graphics = require('graphics'); // Load a builtin module
```

### print([...data])

- **`...data`** `<any>`Data which is shown in the console.

Print out data to the console. The main difference from [console.log()](https://kalumajs.org/docs/api/console) is that it do not print a carriage return and a new line character (`\r\n`) in the end.

```javascript
// print out "100 string value true 1,2,3"
var number = 100;
var result = true;
var arr = [1, 2, 3];
print(number, 'string value', result, arr);
```

### seed(seed)

- **`seed`** `<number>` A seed number to initialize.

Initializes the pseudo-random number generator. When you use `Math.random()`, it generate always the same random sequence. If you want to generate different random number sequence, you need to provide different seed number.

```javascript
// Seeding by millisecond time
seed(millis());

// Seeding by analog input
seed(analogRead(26) * 10000); // GPIO26 = ADC0
```

### btoa(data)

- **`data`** `<Uint8Array|string>` Binary data to encode.
- **Returns**: `<string>` Base64 encoded string.

Returns a base64 encoded string from binary data.

### atob(encodedData)

- **`encodedData`** `<string>` Base64 encoded string data.
- **Returns**: `<Uint8Array>` Decoded binary data.

Returns a decoded binary data from a base64 encoded string.

### encodeURIComponent

- **`data`** `<string>` Data to encode.
- **Returns:** `<string>` Encoded data.

Encodes URI (Uniform Resource Identifier) component using [percent-encoding](https://en.wikipedia.org/wiki/Percent-encoding). This function escapes all characters excepts `A-Z a-z 0-9 - _ . ! ~ * ' ( )`.

### decodeURIComponent

- **`data`** `<string>` Data to decode.
- **Returns:** `<string>` Decoded data.

Decodes URI (Uniform Resource Identifier) component using [percent-encoding](https://en.wikipedia.org/wiki/Percent-encoding).

### LOW

- `<number>` = `0`

### HIGH

- `<number>` = `1`

### INPUT

- `<number>` = `0`

### OUTPUT

- `<number>` = `1`

### INPUT_PULLUP

- `<number>` = `2`

### INPUT_PULLDOWN

- `<number>` = `3`

### LOW_LEVEL

- `<number>` = `1`

### HIGH_LEVEL

- `<number>` = `2`

### FALLING

- `<number>` = `4`

### RISING

- `<number>` = `8`

### CHANGE

- `<number>` = `12`

### Digital I/O Functions

All the functions in the [Digital I/O](https://kalumajs.org/docs/api/digital-io) are in the global scope.

### Analog I/O Functions

All the functions in the [Analog I/O](https://kalumajs.org/docs/api/analog-io) are in the global scope.

### Timers Functions

All the functions in the [Timers](https://kalumajs.org/docs/api/timers) are in the global scope.

### Interrupt Functions

All the functions in the [Interrupts](https://kalumajs.org/docs/api/interrupts) are in the global scope.

### console

- `<Object>`

[Console](https://kalumajs.org/docs/api/console) object is a global object.

### process

- `<Object>`

[process ](https://kalumajs.org/docs/api/process)object is a global object.

### board object

- `<Object>`

[board ](https://kalumajs.org/docs/api/board)object is a global object.

### storage object

- `<Object>`

[storage](https://kalumajs.org/docs/api/storage) object is a global.

### Class: TextEncoder

#### new TextEncoder([label])

- **`label`** `<string>` Encoding type. Default: `'utf-8'`. Currently supported encoding types are `'ascii'` and `'utf-8'`.

#### textEncoder.encode(input)

- **`input`** `<string>`
- Returns: `<Uint8Array>`

Encodes the input string and returns the encoded buffer.

```javascript
var encoder = new TextEncoder();
var buf = encoder.encode('abc'); // [97, 98, 99]
```

### textEncoder.encoding

- `<string>`

Encoding type.

### Class: TextDecoder

#### new TextDecoder([label])

- **`label`** `<string>` Encoding type. Default: `'utf-8'`. Currently supported encoding types are `'ascii'` and `'utf-8'`.

#### textDecoder.decode(input)

- **`input`** `<Uint8Array>`
- Returns: `<string>`

Decodes the input buffer and returns the decoded string.

### textDecoder.encoding

- `<string>`

Encoding type.

### Class: [SystemError](https://kalumajs.org/docs/api/errors/#class-systemerror)

- Extends: `{Error}`

Represents internal system error.

# GPIO

The `gpio` module supports GPIO pin abstraction. Use `require('gpio')` to access this module.

## Class: GPIO

An instances of `GPIO` represents a GPIO pin.

### new GPIO(pin[, mode])

- **`pin`** `<number>` The pin number which can support GPIO function.
- **`mode`** `<number>` The pin mode `INPUT (0)` , `OUTPUT (1)` , `INPUT_PULLUP (2)` or `INPUT_PULLDOWN (3)` . **Default:** `INPUT`

Instances of the GPIO class can be created using the new keyword.

```javascript
// Create the GPIO instance
const { GPIO } = require('gpio');
const pin = new GPIO(0, OUTPUT);
```

### gpio.read()

- **Returns:** `<number>` The return value is `HIGH (1)` or `LOW (0)`

Read the value from the GPIO pin.

```javascript
// Read value from the pin 0.
const { GPIO } = require('gpio');
const pin = new GPIO(0, INPUT);
const value = pin.read();
```

### gpio.write(value)

- **`value`** `<number>` The value could be `HIGH (1)` or `LOW (0)`.

Writes a value to the GPIO pin.

```javascript
// Write HIGH to the pin 0.
const { GPIO } = require('gpio');
const pin = new GPIO(0, OUTPUT);
pin.write(HIGH);
```

### gpio.toggle()

Toggles the output value of the GPIO pin.

```javascript
// Write LOW to the pin 0 and toggling it.
const { GPIO } = require('gpio');
const pin = new GPIO(0, OUTPUT);
pin.write(LOW); // Set to LOW
pin.toggle(); // HIGH
pin.toggle(); // LOW
```

### gpio.low()

Set the GPIO pin to `LOW`.

```javascript
const { GPIO } = require('gpio');
const pin = new GPIO(0, OUTPUT);
pin.low();
```

### gpio.high()

Set the GPIO pin to `HIGH`.

```javascript
const { GPIO } = require('gpio');
const pin = new GPIO(0, OUTPUT);
pin.high();
```

### gpio.irq(callback[, event])

- **`callback`** `<function>(pin, event)` The function is called when the **`event`** is triggered on the **`pin`**. the pin and event are the arguments of the callback function.
- **`event`** `<number>` set the event of the `pin`. There are three events,`FALLING (4)`, `RISING (8)`, and `CHANGE (12)`. **Default:** `CHANGE`.

Add interrupt on the GPIO pin. Run the **`callback`** function when the **`event`** is triggered on the **`pin`**. There are three events for the interrupts. The `FALLING (4)` event is triggered when the **`pin`** state is changed from `HIGH` to `LOW`. The `RISING (8)` event is triggered when the **`pin`** state is changed from `LOW` to `HIGH`. The `CHANGE (12)` event is triggered when the **`pin`** state is changed to any states, which means the `CHANGE` event is the same as the `FALLING` + `RISING` events.

The interrupt does not support level triggering, you can use setWatch() function when you want to use level triggering events (`LOW_LEVEL`, `HIGH_LEVEL`).

```javascript
// Set interrupt on the GPIO 0
const { GPIO } = require('gpio');
// GPIO 0 and GPIO 1 are connected together
const pin0 = new GPIO(0, INPUT); // GPIO 0 is input port which check the state change of GPIO 1
const pin1 = new GPIO(1, OUTPUT); // GPIO 1 is output
pin1.low(); // Set LOW on the GPIO 1

// Callback function for the GPIO 0
function callback0(pin, mode) {
  console.log('Event ' + mode + ' is triggered on the GPIO' + pin);
}

// Set interrupt callback function on GPIO01
pin0.irq(callback0, CHANGE);

pin1.high(); // Callback is called bcause GPIO 0 is changed from LOW to HIGH
delay(1);
pin1.low(); // Callback is called bcause GPIO 0 is changed from HIGH to LOW
```

### gpio.pin

- `<number>`

Pin number of the GPIO object.

```javascript
// Write LOW to the pin 0 and print the pin number.
const { GPIO } = require('gpio');
const gpio0 = new GPIO(0, OUTPUT);
console.log(gpio0.pin); // Print pin number 0.
```

### gpio.mode

- `<number>`

Current mode of the GPIO pin. The value is `INPUT (0)` , `OUTPUT (1)` , `INPUT_PULLUP (2)` , or `INPUT_PULLDOWN (3)` .

```javascript
// Write LOW to the pin 0 and print the mode.
const { GPIO } = require('gpio');
const gpio0 = new GPIO(0, OUTPUT);
console.log(gpio0.mode); // Print pin number 0.
```

# Graphics

The `graphics` module supports graphic APIs. Use `require('graphics')` to access this module.

## Class: GraphicsContext

An instance of `GraphicsContext` provides basic graphic functions including drawing shapes, fonts, images, and etc. All graphic functions depend on the provided primitive drawing functions which draws directly to a graphic device.

### new GraphicsContext(width, height[, options])

- **`width`** `<number>` Screen width of the graphic device in pixels.

- **`height`** `<number>` Screen height of the graphic device in pixels.

- **`options`** `<object>`  Graphic context options.

  - **`rotation`** `<number>` Rotation of screen. One of `0` (0 degree), `1` (90 degree in clockwise), `2` (180 degree in clockwise), and `3` (270 degree in clockwise).
  - **`setPixel`** `<function(x: number, y:number, color:number)>` A callback function to draw a pixel at `x`, `y` with color `color`.
  - **`getPixel`** `<function(x:number, y:number): number>` A callback function to get color value at a pixel `x`, `y`.
  - **`fillRect`** `<function(x:number, y:number, w:number, h:number, color:number)>` A callback function to draw filled rectangle `x`, `y` with width `w` and height `h` and color `color`. Optional. If not provided, `setPixel` is used to draw filled rectangle.

`GraphicsContext` is instantiated by a *graphic driver* rather than a user. A graphic driver should provide a method (e.g. `driver.getContext()`) to get an appropriate instance of `GraphicsContext` so that user can draw something with APIs.

Here is an example to use `GraphicsContext` of a graphic driver SSD1306.

```javascript
const { SSD1306_I2C } = require('ssd1306');
const { I2C } = require('i2c');
const i2c0 = new I2C(0);
const ssd1306 = new SSD1306_I2C(128, 32, i2c0, { rstPin: 19 });
const gc = ssd1306.getContext();
// use APIs of gc
```

### gc.getWidth()

- **Returns:** `<number>` Screen width of graphic device in pixels.

Returns screen width in pixels.

### gc.getHeight()

- **Returns:** `<number>` Screen height of graphic device in pixels.

Return screen height in pixels.

### gc.clearScreen()

Clear the screen buffer.

### gc.color16(red, green, blue)

- **`red`** `<number>` Red brightness between 0~255.
- **`green`** `<number>` Green brightness between 0~255.
- **`blue`** `<number>` Blue brightness between 0~255.
- **Returns**: 16-bit color value.

Return 16-bit color (5-bit for red, 6-bits for green, 5-bits for blue) value from RGB values.

### gc.fillScreen(color)

- **`color`** `<number>` Color value.

Fill the screen buffer with the color.

### gc.setRotation(rotation)

- **`rotation`** `<number>` Rotation of screen.

Set the rotation of screen.

### gc.getRotation()

- **Returns:** `<number>` Rotation of screen.

Returns rotation of screen.

### gc.setColor(color)

- **`color`** `<number>` Pen color.

Set pen color.

### gc.getColor()

- **Returns:** `<number>` Pen color.

Returns pen color.

### gc.setFillColor(color)

- **`color`** `<number>` Fill color.

Set fill color.

### gc.getFillColor()

- **Returns:** `<number>` Fill color.

Returns fill color.

### gc.setFontColor(color)

- **`color`** `<number>` Font color.

Set font color.

### gc.getFontColor()

- **Returns:** `<number>` Font color.

Returns font color.

### gc.setFont(font)

- `font`
`<object|null>`  Custom font object. If `null` passed, default font is used.

  - **`bitmap`** `<Uint8Array>` Font bitmap data.
  - **`glyphs`** `<Uint8Array>` Font glyph data.
  - **`width`** `<number>` Font character width.
  - **`height`** `<number>` Font character height.
  - **`first`** `<number>` ASCII code for first character in this font.
  - **`last`** `<number>` ASCII code for last character in this font.
  - **`advanceX`** `<number>` Horizontal advance to next character in pixels.
  - **`advanceY`** `<number>` Vertical advance to next line in pixels.

Set a custom font. If you are interested in developing custom font, please check the [font conversion tool](https://github.com/kaluma-project/kameleon-fontconv).

### gc.setFontScale(scaleX, scaleY)

- **`scaleX`** `<number>` Horizontal scale of font.
- **`scaleY`** `<number>` Vertical scale of font.

Set font scale.

### gc.setPixel(x, y, color)

- **`x`** `<number>` coordinate X.
- **`y`** `<number>` coordinate Y.
- **`color`** `<number>` Pixel color.

Draw a pixel at (x, y) coordinate.

### gc.getPixel(x, y)

- **`x`** `<number>` coordinate X.
- **`y`** `<number>` coordinate Y.
- **Returns:** `<number>` Pixel color at (x, y) coordinate.

Get pixel color at (x, y) coordinate.

### gc.drawLine(x0, y0, x1, y1)

- **`x0`** `<number>` coordinate X0.
- **`y0`** `<number>` coordinate Y0.
- **`x1`** `<number>` coordinate X1.
- **`y1`** `<number>` coordinate Y1.

Draw a line from (x0, y0) coordinate to (x1, y1) coordinate.

### gc.drawRect(x, y, w, h)

- **`x`** `<number>` coordinate X.
- **`y`** `<number>` coordinate Y.
- **`w`** `<number>` Width of rectangle.
- **`h`** `<number>` Height of rectangle.

Draw a rectangle at (x, y) coordinate of `w` width and `h` height.

### gc.fillRect(x, y, w, h)

- **`x`** `<number>` coordinate X.
- **`y`** `<number>` coordinate Y.
- **`w`** `<number>` Width of rectangle.
- **`h`** `<number>` Height of rectangle.

Draw a filled rectangle at (x, y) coordinate of `w` width and `h` height.

### gc.drawCircle(x, y, r)

- **`x`** `<number>` coordinate X.
- **`y`** `<number>` coordinate Y.
- **`r`** `<number>` Radius of circle.

Draw a circle at (x, y) coordinate of `r` radius.

### gc.fillCircle(x, y, r)

- **`x`** `<number>` coordinate X.
- **`y`** `<number>` coordinate Y.
- **`r`** `<number>` Radius of circle.

Draw a filled circle at (x, y) coordinate of `r` radius.

### gc.drawRoundRect(x, y, w, h, r)

- **`x`** `<number>` coordinate X.
- **`y`** `<number>` coordinate Y.
- **`w`** `<number>` Width of rectangle.
- **`h`** `<number>` Height of rectangle.
- **`r`** `<number>` Radius of corner.

Draw a rounded rectangle at (x, y) coordinate of `w` width, `h` height and `r` radius corner.

### gc.fillRoundRect(x, y, w, h, r)

- **`x`** `<number>` coordinate X.
- **`y`** `<number>` coordinate Y.
- **`w`** `<number>` Width of rectangle.
- **`h`** `<number>` Height of rectangle.
- **`r`** `<number>` Radius of corner.

Draw a filled and rounded rectangle at (x, y) coordinate of `w` width, `h` height and `r` radius corner.

### gc.drawText(x, y, text)

- **`x`** `<number>` coordinate X.
- **`y`** `<number>` coordinate Y.
- **`text`** `<string>` Text string to draw.

Draw a text at (x, y) coordinate.

### gc.measureText(text)

- **`text`** `<string>` Text to get text bound.

- Returns: `<object>` Text bound.

  - **`width`** `<number>` Width of text.
  - **`height`** `<number>` Height of text.

Get text bound (width, height) of a given text string.

### gc.drawBitmap(x, y, bitmap[, options])

- **`x`** `<number>` coordinate X.

- **`y`** `<number>` coordinate Y.

- **`bitmap`** `<object>` Bitmap image.

  - **`width`** `<number>` Bitmap width.
  - **`height`** `<number>` Bitmap height.
  - **`bpp`** `<number>` Bits per pixel. Default: `1`.
  - **`data`** `<Uint8Array|string>` Bitmap data. If string type is given, it should be a base64-encoded string. *Note that `Uint8Array` type is much faster than base64-encoded string.*


- **`bitmap`** `<object>` Bitmap image.

  - **`color`** `<number>` Color for 1-bit mono bitmap. Default: `1` for 1-bit bitmap and `0xffff` for 16-bit bitmap.
  - **`transparent`** `<number>` Color value to be treated as transparent if defined.
  - **`scaleX`** `<number>` Horizontal scale of bitmap. Default: `1`.
  - **`scaleY`** `<number>` Vertical scale of bitmap. Default: `1`.
  - **`flipX`** `<boolean>` Horizontal flip of bitmap. Default: `false`.
  - **`flipY`** `<boolean>` Vertical flip of bitmap. Default: `false`.

Draw a bitmap image at (x, y) coordinate. The bitmap is drawn with current color in case of 1-bit bitmap.

## Class: BufferedGraphicsContext

This class inherits from `GraphicsContext`.

An instance of `BufferedGraphicsContext` provides graphic functions that effects to the an internal buffer, so you need to call `display()` finally to take effect on actual graphic device.

### new BufferedGraphicsContext(width, height[, options])

- **`width`** `<number>` Screen width of the graphic device in pixels.

- **`height`** `<number>` Screen height of the graphic device in pixels.

- **`options`** `<object>`  Graphic context options.

  - **`rotation`** `<number>` Rotation of screen. One of `0` (0 degree), `1` (90 degree in clockwise), `2` (180 degree in clockwise), and `3` (270 degree in clockwise).
  - **`bpp`** `<number>` Bits per pixel. Currently `1` or `16` supported.
  - **`display`** `<function(buffer: Uint8Array)>` A callback function to display the buffer on the graphic device.

### bgc.buffer

- `<Uint8Array>`

Internal graphic buffer.

### bgc.display()

Display the graphics by pushing the graphic buffer to the driver.

# HTTP

> **THIS IS EXPERIMENTAL AND SUBJECT OF CHANGE**

The `http` module supports HTTP server and client.

> This module may not handle HTTP pages larger than available RAM. If you are using a network driver depends on AT commands with UART, you may handle more larger data by extending read buffer size of UART.

## http.request(options[, callback])

- **`options`** `{Object}`

  - **`host`** `{string}` A domain name or IP address of remote server.
  - **`port`** `{number}` Port of remote server.
  - **`method`** `{string}` HTTP request method. Default: `'GET'`.
  - **`path`** `{string}` Request path. Default: `'/'`.
  - **`headers`** `{Object}` HTTP headers.

- **`callback`** `{Function}` Added as a one-time listener to the `response` event.

  - **`response`** `{http.IncomingMessage}`

- Returns: `{ClientRequest}`

Creates a HTTP request and returns an instance of `ClientRequest`.

```javascript
const http = require('http');

const options = {
  host: 'www.somewhere.com',
  port: 80,
  path: '/',
  method: 'GET',
};

const req = http.request(options, (res) => {
  console.log(`STATUS: ${res.statusCode}`);
  console.log(`HEADERS: ${JSON.stringify(res.headers)}`);
  res.on('data', (chunk) => {
    console.log(`BODY: ${chunk}`);
  });
  res.on('end', () => {
    console.log('No more data in response.');
  });
});

req.on('error', (e) => {
  console.error(`problem with request: ${e.message}`);
});

// Finish the request
req.end();
```

## http.get(options[, callback])

-  **`options`** `<object>` 
  - **`host`** `{string}` A domain name or IP address of remote server.
  - **`port`** `{number}` Port of remote server.
  - **`path`** `{string}` Request path. Default: `'/'`.
  - **`headers`** `{Object}` HTTP headers.
- **`callback`** `{Function}` Added as a one-time listener to the `response` event.
  - **`response`** `{http.IncomingMessage}`
- Returns: `{ClientRequest}`

This function is same with `http.request()` except for the method is fixed to `'GET'` and automatically call `request.end()` with empty body content.

```javascript
const http = require('http');

http.get({ host: 'somewhere.com', path: '/' }, function (response) {
  response.on('data', function (chunk) {
    console.log('received: ', chunk);
  });
});
```

## http.createServer([requestListener])

- **`requestListener`** `{Function}` Added as a listener for `'request'` event.

Creates an instance of `http.Server` and returns.

```javascript
const http = require('http');

var message = '<h1>Hello</h1>';
var port = 80;

var server = http.createServer((req, res) => {
  console.log('Request path: ' + req.url);
  res.writeHead(200, 'OK', {
    'Content-Type': 'text/html',
    'Content-Length': message.length,
  });
  res.write(message);
  res.end();
});

server.listen(port, function () {
  console.log('HTTP server listening on port: ' + port);
});
```

## Class: http.OutgoingMessage

- Extends: [`{stream.Writable}`](https://kalumajs.org/docs/api/stream/#class-writable)

This is an abstract class for [`http.ClientRequest`](https://kalumajs.org/docs/api/http#class-http-clientrequest) and [`http.ServerResponse`](https://kalumajs.org/docs/api/http#class-http-serverresponse).

### message.setHeader(name, value)

- **`name`** `{string}`
- **`value`** `{string}`

Sets a single header value.

### message.getHeader(name)

- **`name`** `{string}`
- Returns: `{string}`

Returns the value of a single header.

### message.removeHeader(name)

- **`name`** `{string}`

Returns a single header value.

### message.headers

- `{Object}`

Key-value pairs of header names and values.

### message.headersSent

- `{boolean}`

Indicates whether the headers were sent or not.

## Class: http.ClientRequest

- Extends: [`{http.OutgoingMessage}`](https://kalumajs.org/docs/api/http#class-http-outgoingmessage)

This object is created internally and returned by `http.request()`. Before you calling `request.end()`, you can set HTTP request headers and body.

### request.write(chunk[, callback])

- **`chunk`** `{Uint8Array|string}` A chunk of request body data to write.
- **`callback`** `{Function}` Called when the chunk of data is handled.
- Returns: `{boolean}`

Writes a chunk of HTTP body data. The chunk of data will be appended into the internal buffer, and actually transmitted to the server when you call `request.end()` function.

### request.end([chunk][, callback])

- **`chunk`** `{Uint8Array|string}` A chunk of data to write before finish.
- **`callback`** `{Function}` Called when the request is finished.
- Returns: `{this}`

Finishes to write HTTP headers and body. Actually the request will be sent to the server and you will be received the response via `'response'` event.

### Event: 'response'

- **`response`** `{http.IncomingMessage}`

Emitted when the response is arrived and the headers are parsed.

## Class: http.Server

- Extends: [`{net.Server}`](https://kalumajs.org/docs/api/net)

This object is instantiated by `http.createServer()` and returned.

### server.close([callback])

- **`callback`** `{Function}` Called when the server is closed.

Closes the HTTP server.

### server.listen(port[, callback])

- **`port`** `{number}` Port to listen.
- **`callback`** `{Function}` Automatically added as a listener for the `'listening'` event.

Starts to listening for HTTP requests.

### Event: 'request'

- **`request`** `{http.IncomingMessage}`
- **`response`** `{http.ServerResponse}`

Emitted whenever a HTTP request is accepted. You need to send appropriate response.

### Event: 'close'

Emitted when the server closed.

### Event: 'error'

- **`err`** `{Error}`

Emitted when an error occurs.

### Event: 'listening'

Emitted when the server has started to listen for HTTP requests.

## Class: http.ServerResponse

- Extends: [`{http.OutgoingMessage}`](https://kalumajs.org/docs/api/http#class-http-outgoingmessage)

This object is created internally and passed to the event `'request'`.

### response.writeHead(statusCode[, statusMessage][, headers])

- **`statusCode`** `{number}`
- **`statusMessage`** `{string}`
- **`headers`** `{Object}`
- Returns: `{this}`

Sends the response headers to the HTTTP client. If there is no `'Content-Length'` in headers, the header `'Transfer-Encoding'` is automatically set to `'chunked'`.

### response.write(chunk[, callback])

- **`chunk`** `{Uint8Array|string}` A chunk of response body data to write.
- **`callback`** `{Function}` Called when the chunk of data is handled.
- Returns: `{boolean}`

Writes a chunk of HTTP response body. The chunk of data is automatically encoded based on the header `'Transfer-Encoding'`.

### response.end([chunk][, callback])

- **`chunk`** `{Uint8Array|string}` A chunk of data to write before finish.
- **`callback`** `{Function}` Called when the response is finished.
- Returns: `{this}`

Finishes to write HTTP response.

### Event: 'close'

Emitted when underlying connection is closed.

### Event: 'finish'

Emitted when the response has been sent.

## Class: http.IncomingMessage

- Extends: [`{stream.Readable}`](https://kalumajs.org/docs/api/stream/#class-readable)

This object is created by `http.ClientRequest` or `http.Server` and passed via the event `'response'` and `'request'` respectively.

When you get this object via `'response'` event, you can get HTTP response headers and body. When you get this object via `'request'` event, you can get HTTP request headers and body.

### message.httpVersion

- `{string}`

Represents HTTP version. Probably `'1.1'` or `'1.0'`.

### message.method

- `{string}`

Represents HTTP method. It's only valid when this message is HTTP request.

### message.statusCode

- `{number}`

The 3-digit HTTP response status code. It's only valid when this message is HTTP response.

### message.statusMessage

- `{string}`

The HTTP response status message. It's only valid when this message is HTTP response.

### message.headers

- `{Object}`

The header object of HTTP request/response message. Header names are lower-cased.

### message.url

- `{string}`

Request URL string. It's only valid when this message is HTTP request.

### message.complete

- `{boolean}`

Represents whether the HTTP request/response message is completely received.

### Event: 'data'

- **`chunk`** `{Uint8Array}`

Emitted when a chunk of body data of HTTP request or HTTP response is received.

### Event: 'end'

Emitted when the server finished to send request or response data.

### Event: 'close'

Emitted when the request or response is closed.

# I2C

The `i2c` module supports communication with I2C (Inter-Integrated Circuit) / TWI (Two Wire Interface) devices. Use `require('i2c')` to access this module.

> Slave Mode is NOT supported yet.

## Class: I2C

An instances of `I2C` represents a I2C bus.

### I2C.MASTER

- `<number>` = `0`

### I2C.SLAVE

- `<number>` = `1`

### new I2C(bus[, options])

- **`bus`** `<number>` I2C bus number.
- **`options`** `<object>` The object of I2C options. Default values are depends on board (Check in a board page).
  - **`mode`** `<number>` I2C mode, `I2C.MASTER` or `I2C.SLAVE` mode. **Default:** `I2C.MASTER`
  - **`baudrate`** `<number>` Clock speed (bit/s) for Master mode.
  - **`scl`** `<number>` SCL pin number. `-1` Not to use this pin. **Default:** board dependent. Check the `Boards` document.
  - **`sda`** `<number>` SDA pin number. `-1` Not to use this pin. **Default:** board dependent. Check the `Boards` document.
- **Returns:** `<object>` The return value is `I2C` object.

Instances of the `I2C` class can be created using the `new` keyword or by calling `i2c.I2C()` as a function. A `RangeError` will be thrown if **`bus`** exceeds max bus number.

```javascript
// Create the I2C instance with master mode
const {I2C} = require('i2c');

// open bus 0 in master mode
const i2c0 = new I2C(0);
// read or write ...
i2c0.close();

// open bus 1 in master mode, full speed
const i2c1 = new I2C(1, { mode: I2C.MASTER, baudrate: 400000 });
// read or write ...
i2c1.close();
```

### i2c.write(data, address[, timeout[, count]])

- **`data`** `<Uint8Array|string>` Data to write.
- **`address`** `<number>` I2C slave address. (7bit)
- **`timeout`** `<number>` Timeout in milliseconds. **Default:** `5000`.
- **`count`** `<number>` Indicates how many times to write data. **Default:** `1`
- **Returns:** `<number>` The number of bytes written, `-1` if it failed to write or timeout.

This method writes data to the specified address (slave device) and returns the number of bytes written. This method can be called only in master mode.

```javascript
const { I2C } = require('i2c');
const i2c0 = new I2C(0, { baudrate: 50000 }); // master mode 50 kbits/s

// Writes 2 bytes as a Uint8Array
var array = new Uint8Array([0x6b, 0x00]);
i2c0.write(array, 0x68);

// Writes 5 bytes as a string
i2c0.write('Hello', 0x68);

i2c0.close();
```

### i2c.read(length, address[, timeout])

- **`length`** `<number>` Data length to read.
- **`address`** `<number>` I2C slave address. (7bit)
- **`timeout`** `<number>` Timeout in milliseconds. **Default:** `5000`.
- **Returns:** `<Uint8Array>` An array buffer having data read, `null` if failed to read.

This method read data from the specified address (slave device) and returns an array buffer object. This method can be called only in master mode.

```javascript
const { I2C } = require('i2c');
const i2c0 = new I2C(0);

// Read 14 bytes from the address 0x68.
var buf = i2c0.read(14, 0x68);
if (buf) {
  console.log(data.length); // 14
  console.log(data[0]); // first byte
}
i2c0.close();
```

### i2c.memWrite(data, address, memAddress[, memAddressSize[, timeout[, count]]])

- **`data`** `<Uint8Array|string>` Data to write.
- **`address`** `<number>` I2C slave address. (7bit)
- **`memAddress`** `<number>` Memory address to write.
- **`memAddressSize`** `<number>` Size of `memAddress`. Set `16` when `memAddress` is 16-bit address, or set `8` if `memAddress` is 8-bit address. **Default:** `8`.
- **`timeout`** `<number>` Timeout in milliseconds. **Default:** `5000`.
- **`count`** `<number>` Indicates how many times to write data. **Default:** `1`
- **Returns:**`<number>` The number of bytes written, `-1` if failed to write or timeout.

This method writes data to the memory address in the specified slave device and returns the number of bytes written. This method can be called only in master mode.

```javascript
const { I2C } = require('i2c');
const i2c0 = new I2C(0);

// Writes 2 bytes at memory address 0x10 of slave 0x68
var array = new Uint8Array([0x6b, 0x00]);
i2c0.memWrite(array, 0x68, 0x10);

i2c0.close();
```

### i2c.memRead(length, address, memAddress[, memAddressSize[, timeout]])

- **`length`** `<number>` Data length to read.
- **`address`** `<number>` I2C slave address. (7bit)
- **`memAddress`** `<number>` Memory address to read.
- **`memAddressSize`** `<number>` Size of `memAddress`. Set `16` when `memAddress` is 16-bit address, or set `8` when `memAddress` is 8-bit address. **Default:** `8`.
- **`timeout`** `<number>` Timeout in milliseconds. **Default:** `5000`
- **Returns:** `<Uint8Array>` A buffer having data read, `null` if failed to read.

This method read data at memory address from the specified slave device and returns an array buffer object. This method can be called only in master mode.

```javascript
const { I2C } = require('i2c');
const i2c0 = new I2C(0);

// Read 14 bytes at memory address 0x0100 from slave 0x68
var buf = i2c0.memRead(0x0100, 14, 0x68, 1);
if (buf) {
  console.log(data.length); // 14
  console.log(data[0]); // first byte
}
i2c0.close();
```

### i2c.close()

This method closes the I2C bus.

```javascript
const { I2C } = require('i2c');
const i2c0 = new I2C(0);

// Write or read from I2C ...

i2c0.close(); // Close i2c device
```

# Interrupts

### attachInterrupt(pin, callback[,events])

- **`pin`** `<number>` The pin number which can support GPIO function.
- **`callback`** `<function>(pin, event)` The function is called when the **`event`** is triggered on the **`pin`**. the pin and event are the arguments of the callback function.
- **`events`** `<number>` Set the events of the `pin`. There are three events,`FALLING (4)`, `RISING (8)`, and `CHANGE (12)`. **Default:** `CHANGE`.

Run the **`callback`** function when the **`events`** is triggered on the **`pin`**. There are three events for the interrupts. The `FALLING (4)` event is triggered when the **`pin`** state is changed from `HIGH` to `LOW`. The `RISING (8)` event is triggered when the **`pin`** state is changed from `LOW` to `HIGH`. The `CHANGE (12)` event is triggered when the **`pin`** state is changed to any states, which means the `CHANGE` event is the same as the `FALLING` + `RISING` events.

The interrupt does not support level triggering, you can use setWatch() function when you want to use level triggering events (`LOW_LEVEL`, `HIGH_LEVEL`).

```javascript
// Set interrupt on the GPIO0
const { GPIO } = require('gpio');
// GPIO0 and GPIO1 are connected together
pinMode(0, INPUT); // GPIO0 is input port which check the state change of GPIO1
pinMode(1, OUTPUT); // GPIO1 is output
digitalWrite(1, LOW); // Set LOW on the GPIO1

// Callback function for the GPIO0
function callback0(pin, mode) {
  console.log('Event ' + mode + ' is triggered on the GPIO' + pin);
}

// Set interrupt callback function on GPIO0
attachInterrupt(0, callback0, CHANGE);

digitalWrite(1, HIGH); // Callback is called bcause GPIO0 is changed from LOW to HIGH
delay(1);
digitalWrite(1, LOW); // Callback is called bcause GPIO0 is changed from HIGH to LOW

detachInterrupt(0); // Remove the interrupt on the GOIO0

digitalWrite(1, HIGH); // No callback function call for this event
delay(1);
digitalWrite(1, LOW); // No callback function call for this event
```

### detachInterrupt(pin)

- **`pin`** `<number>` The pin number which can support GPIO function.

Remove the interrupts on the pin which is added by `attachInterrupt()` function.

### enableInterrupts()

Enable all the interrupts which is attached by `attachInterrupt()` function. The interrupts are automatically enabled when `attachedInterrupts()` is called so it is not needed to use this function if you didn't call `disableInterrupts()` function.

### disableInterrupts()

Disable all the interrupts which is attached by `attachInterrupt()` function. The interrupts will be enabled when `enableInterrupts()` is called.

```javascript
// Set interrupt on the GPIO0
const { GPIO } = require('gpio');
// GPIO0 and GPIO1 are connected together
pinMode(0, INPUT); // GPIO0 is input port which check the state change of GPIO1
pinMode(1, OUTPUT); // GPIO1 is output
digitalWrite(1, LOW); // Set LOW on the GPIO1

// Callback function for the GPIO0
function callback0(pin, mode) {
  console.log('Event ' + mode + ' is triggered on the GPIO' + pin);
}

// Set interrupt callback function on GPIO0
attachInterrupt(0, callback0, CHANGE);

digitalWrite(1, HIGH); // Callback is called bcause GPIO0 is changed from LOW to HIGH
delay(1);
digitalWrite(1, LOW); // Callback is called bcause GPIO0 is changed from HIGH to LOW

disableInterrupts(); // Disable all the interrupts

digitalWrite(1, HIGH); // No callback function call for this event
delay(1);
digitalWrite(1, LOW); // No callback function call for this event

enableInterrupts(); // Enable all the interrupts

digitalWrite(1, HIGH); // Callback is called bcause GPIO0 is changed from LOW to HIGH
delay(1);
digitalWrite(1, LOW); // Callback is called bcause GPIO0 is changed from HIGH 
```

# LED

The `led` module supports LED which connect to GPIO pins. Use `require('led')` to access this module.

## Class: LED

An instances of `LED` represents a LED.

### new LED(pin)

- **`pin`** `<number>` Pin number where LED connected.

```javascript
// Create the LED instance
const { LED } = require('led');
const led = new LED(0); // LED connected to pin 0.
```

### on()

Turns on the LED.

```javascript
// Turn on the LED.
const { LED } = require('led');
const led = new LED(0); // LED connected to pin 0.
led.on();
```

### off()

Turns off the LED.

```javascript
// Turn off the LED.
const { LED } = require('led');
const led = new LED(0); // LED connected to pin 0.
led.off();
```

### toggle()

This method toggles the LED.

```javascript
// Toggle the LED.
const { LED } = require('led');
const led = new LED(0); // LED connected to pin 0.
led.on();
delay(1000); // wait for 1sec
led.off();
delay(1000); // wait for 1sec
led.toggle(); // on
delay(1000); // wait for 1sec
led.toggle(); // off
```

### read()

- Returns: `<number>` State of the LED. 1 means LED ON and 0 means LED OFF state.

Read the state of the LED.

```javascript
// Read the state of LED.
const { LED } = require('led');
const led = new LED(0); // LED connected to pin 0.
led.on();
console.log(led.read()); // Returns 1.
delay(1000);
led.off();
console.log(led.read()); // Returns 0.
```

### pin

- `<number>`

Pin number of the LED.

```javascript
// Javascript example: Print out the LED pin number.
const { LED } = require('led');
const led = new LED(0); // LED connected to pin 0.
console.log(led.pin); // print out the LED pin number, 0;
```

# Net

> **THIS IS EXPERIMENTAL AND SUBJECT OF CHANGE**

The `net` module supports an asynchronous stream-based API for TCP clients and servers. Use `require('net')` to access this module.

> This module is depends on the [network device driver](https://kalumajs.org/docs/api/device-driver/#netdev) so the board should register a network device driver manually if your board is not builtin network capabilities.

## net.createConnection(options[, connectListener])

- **`options`** `{object}`

  - **`host`** `{string}`
  - **`port`** `{number}`

- **`connectListener`** `{function}`

- Returns: `{Socket}`

Creates a socket and initiates a connection.

```javascript
const net = require('net');
var conn = { host: '0.0.0.0', port: 8124 }; // enter valid host ip
var client = net.createConnection(conn, () => {
  // 'connect' listener.
  console.log('connected to server!');
  client.write('hello, world!\r\n');
});
client.on('data', (data) => {
  console.log(data);
  client.end();
});
client.on('end', () => {
  console.log('disconnected from server');
});
```

## net.connect(options[, connectListener])

Alias to [net.createConnection(options[, connectListener\])](https://kalumajs.org/docs/api/net#net-createconnection-options-connectlistener).

## net.createServer([connectionListener])

- **`connectionListener`** `{Function}`
- Returns: `{Server}`

Creates a new TCP server.

```javascript
const net = require('net');
var server = net.createServer((c) => {
  // 'connection' listener.
  console.log('client connected');
  c.on('end', () => {
    console.log('client disconnected');
  });
  c.write('hello, world\r\n');
  c.end();
});
server.on('error', (err) => {
  throw err;
});
server.listen(8124, () => {
  console.log('server bound');
});
```

## Class: Server

- Extends: [`EventEmitter`](https://kalumajs.org/docs/api/events/#class-eventemitter)

This class is used to create a TCP server. This is a condensed version of Server class in [Node.js](https://nodejs.org/).

### new Server([connectionListener])

- **`connectionListener`** `{Function}` Automatically set as a listener for the `'connection'` event.

Creates a TCP server.

### server.close([callback])

- **`callback`** `{Function}` Called when the server is closed.

Closes the server.

### server.listen(port[, callback])

- **`port`** `{number}` Port to listen.
- **`callback`** `{Function}` Automatically added as a listener for the `'listening'` event.

Starts to listening for connections.

### Event: 'close'

Emitted when the server closed.

### Event: 'connection'

- **`connection`** `{Socket}` A new connection.

Emitted when a new connection is made.

### Event: 'error'

- **`err`** `{Error}`

Emitted when an error occurs.

### Event: 'listening'

Emitted when the server has started to listen for connections.

## Class: Socket

- Extends: [`stream.Duplex`](https://kalumajs.org/docs/api/stream/#class-stream-duplex)

This class is an abstraction of a TCP socket. This is a condensed version of Socket class in [Node.js](https://nodejs.org/).

### new Socket()

Create a new socket object.

- Returns: `{Socket}`

### socket.localAddress

- `{string}`

The string representation of the local IP address. (e.g. `'168.120.88.10'`, `'127.0.0.1'`)

### socket.localPort

- `{number}`

The numeric representation of the local port. (e.g. `80`, `8124`)

### socket.remoteAddress

- `{string}`

The string representation of the remote IP address.

### socket.remotePort

- `{number}`

The numeric representation of the remote port.

### socket.connect(options[, connectListener])

- **`options`** `<object>` 

  - **`host`** `{string}` Host to connect.
  - **`port`** `{number}` Port of the host to connect.

- **`connectListener`** `{function}` A listener will be added to the `'connect'` event once.

- Returns: `{this}`

Initiate a TCP connection on the given socket.

### socket.destroy()

- Returns: `{this}`

Closes the socket.

### socket.end([data][, callback])

- **`data`** `{Uint8Array|string}`
- **`callback`** `{function}`
- Returns: `{this}`

Half-closes the socket. Finishes to write data but the counter-side still can send some data.

### socket.write(data[, callback])

- **`data`** `{Uint8Array|string}`
- **`callback`** `{function}`
- Returns: `{boolean}`

Send data on the socket. Returns `true` if all of data successfully sent or `false` if part of data are not sent and added in buffer to send later. Event `'drain'` will be emitted when all buffered data are sent.

### Event: 'connect'

Emitted when a socket connection is established.

### Event: 'close'

Emitted when a socket connection is closed.

### Event: 'end'

Emitted when the counter-side half-closes the socket.

### Event: 'drain'

Emitted when the socket is ready to write.

### Event: 'finish'

Emitted when the socket is finished to write.

### Event: 'data'

- **`data`** `{Uint8Array}`

Emitted when data received.

### Event: 'error'

- **`err`** `{Error}`

Emitted when an error occurs.

# Path

## Path

The `path` module provides functions for file and directory paths. Use `require('path')` to access this module.

### isAbsolute(path)

- **`path`** `<string>`
- **Returns:** `<boolean>`

Determines if the given path is absolute or not.

### format(pathObj)

- **`pathObj`** `<object>`

  - **`root`** `<string>`
  - **`dir`** `<string>`
  - **`base`** `<string>`
  - **`name`** `<string>`
  - **`ext`** `<string>`

- **Returns:** `<string>`

Returns a formatted path string from a given path object.

### parse(path)

- **`path`** `<string>`

- Returns: `<object>`

  - **`root`** `<string>`
  - **`dir`** `<string>`
  - **`base`** `<string>`
  - **`name`** `<string>`
  - **`ext`** `<string>`

Returns a path object parsed from a given path string.

### normalize(path)

- **`path`** `<string>`
- **Returns:** `<string>`

Normalizes the given path, resolving `'.'` and `'..'` segments.

### join(...paths)

- **`...paths`** `<string[]>`
- **Returns:** `<string>`

Joins all path segments together.

### resolve(...paths)

- **`...paths`** `<string[]>`
- **Returns:** `<string>`

Resolves a sequence of path segments into an absolute path.

### sep

- `<string>`

Path segment separator.

# Process

- The `process` object provide the information about the process.

## Object: process

### process.arch

- `<string>`ex) 'arm', 'x64', ...

The system architecture which the Kaluma firmware is compiled.

### process.platform

- `<string>`ex) 'linux', 'darwin', 'unknown', ...

The OS system which the Kaluma firmware is running.

### process.version

- `<string>`version number, semver format. ex) '0.1.0', ...

### process.builtin_modules

- `<string[]>`array of built-in module names.

There's no need to use `require` before using the built-in module.

### process.getBuiltinModule(builtin_module_name)

- **`builtin_module_name`** `<string>`
- **Returns:** `<function>`

load the builtin module and return the module as a function

### process.binding(native_module_name)

- **`native_module_name`** `<string>`
- **Returns:** `<function>`

load a native module

Has properties : binding has native module names as properties

### process.memoryUsage()

- Returns: `<object>`

  - **`heapTotal`** `<number>`
  - **`heapUsed`** `<number>`
  - **`heapPeak`** `<number>`

Returns an object describing memory usage.

### process.stdin

- `<`[`Stream`](https://kalumajs.org/docs/api/stream/#class-stream)`>`

Returns a stream object connected to standard input.

```javascript
// reads 100 bytes from standard in
let size = 0;
while (size < 100) {
  let chunk = process.stdin.read();
  if (chunk) {
    let s = String.fromCharCode.apply(null, chunk);
    console.log('data = ', s);
    size += chunk.length;
  }
}
```

### process.stdout

- `<`[`Stream`](https://kalumajs.org/docs/api/stream/#class-stream)`>`

Returns a stream object connected to standard output.

```javascript
let data = new Uint8Array([65, 66, 67, 68, 69]); // "ABCDE"
process.stdout.write(data);
```

# PWM

The `pwm` module supports PWM(Pulse Width Modulation). Use `require('pwm')` to access this module.

## Class: PWM

An instances of `PWM` represents a PWM object.

### new PWM(pin[, frequency[, duty]])

- **`pin`** `<number>` The pin number which can support PWM function.
- **`frequency`** `<number>` The PWM frequency in Hz. **Default:** `490`Hz
- **`duty`** `<number>` The PWM duty cycle between `0.0` and `1.0`. **Default:** `1.0`

```javascript
// Generate 1000 Hz 50% duth PWM signal on the pin 1.
const { PWM } = require('pwm');
const pwm = new PWM(1, 1000, 0.5); // Create the PWM instance with pin 1
pwm.start(); // Generate PWM signal
// ...
pwm.stop(); // Stop PWM signal
pwm.close(); // Close PWM port.
```

### pwm.start()

Start to generate PWM signal.

```javascript
// Generate 1000 Hz 30% duth PWM signal on the pin 1.
const { PWM } = require('pwm');
const pwm = new PWM(1, 1000, 0.3); // Create the PWM instance with pin 1
pwm.start(); // Generate PWM signal
```

### pwm.stop()

Stop to generate PWM signal.

```javascript
// Generate 1000 Hz 50% duth PWM signal on the pin 1.
const { PWM } = require('pwm');
const pwm = new PWM(1, 1000, 0.5); // Create the PWM instance with pin 1
pwm.start(); // Generate PWM signal
delay(100); // Wait 100ms.
pwm.stop(); // Stop PWM signal
```

### pwm.getFrequency()

- **Returns:** `<number>` PWM frequency of the PWM instance.

Get the PWM frequency.

```javascript
// Generate 1000 Hz 50% duth PWM signal on the pin 1 and print the frequency
const { PWM } = require('pwm');
const pwm = new PWM(1, 1000, 0.5); // Create the PWM instance with pin 1
console.log(pwm.getFrequency()); // Print current PWM frequency.
```

### pwm.setFrequency(frequency)

- **`frequency`** `<number>` PWM frequency of the PWM instance.

Set the new PWM frequency.

```javascript
// Generate 1000 Hz 50% duth PWM signal on the pin 1 and print the frequency
const { PWM } = require('pwm');
const pwm = new PWM(1, 1000, 0.5); // Create the 1000 Hz PWM instance with pin 1
pwm.setFrequency(500); // Change the PWM frequency to 500 Hz.
```

### pwm.getDuty()

- **Returns:** `<number>` PWM duty of the PWM instance.

Get the PWM duty.

```javascript
// Generate 1000 Hz 50% duth PWM signal on the pin 1 and print the duty
const { PWM } = require('pwm');
const pwm = new PWM(1, 1000, 0.5); // Create the PWM instance with pin 1
console.log(pwm.getDuty()); // Print current PWM duty.
```

### pwm.setDuty(duty)

- **`duty`** `<number>` PWM duty of the PWM instance.

Set the new PWM duty.

```javascript
// Generate 1000 Hz 50% duth PWM signal on the pin 1 and print the duty
const { PWM } = require('pwm');
const pwm = new PWM(1, 1000, 0.5); // Create the PWM instance with pin 1
pwm.setDuty(0.7); // Change the PWM duty to 70%.
```

### pwm.close()

Close the PWM port.

```javascript
// Generate 1000 Hz 50% duth PWM signal on the pin 1.
const { PWM } = require('pwm');
const pwm = new PWM(1, 1000, 0.5); // Create the PWM instance with pin 3
pwm.start(); // Generate PWM signal
// ...
pwm.stop(); // Stop PWM signal
pwm.close(); // Close PWM port.
```

# RP2

> **THIS IS EXPERIMENTAL AND SUBJECT OF CHANGE**

The `rp2` module includes features only for RP2 target. Use `require('rp2')` to access this module.

## dormant(pins, events)

- **`pins`** `<number[]>` An array of GPIO pin numbers for wakeup.
- **`events`** `<numbers[]>` An array of wakeup events for the `pins` parameter. The length of pins and events should be the same.

Enter dormant mode for low power consumption.

> Note that once it goes to dormant mode, the USB will be disconnected and the connection will not be recovered even when it wakes up. You need to reset the board.

```javascript
const rp2 = require('rp2');

pinMode(25, OUTPUT); // On-board LED
pinMode(0, INPUT_PULLUP); // Button for dormant
pinMode(1, INPUT_PULLUP); // Button for wakeup

// Blinking LED
setInterval(() => {
  digitalToggle(25);
}, 200);

// Enter dormant when you press the button on GPIO 0.
setWatch(
  () => {
    // Wakeup when falling event detected on GPIO 1.
    rp2.dormant([1], [FALLING]);
  },
  0,
  FALLING,
  10
);
```

## Object: PIO

Constants for state machine of PIO.

### PIO.FIFO_JOIN_NONE

- `<number>`

### PIO.FIFO_JOIN_TX

- `<number>`

### PIO.FIFO_JOIN_RX

- `<number>`

### PIO.SHIFT_LEFT

- `<number>`

### PIO.SHIFT_RIGHT

- `<number>`

### PIO.TX_LESSTHAN

- `<number>`

### PIO.RX_LESSTHAN

- `<number>`

## Class: ASM

An instances of `ASM` represents an assembly program for PIO (Programmable I/O). This object emulates the assembly language for RP2's PIO using [method chaining](https://en.wikipedia.org/wiki/Method_chaining).

```javascript
// hello.pio
// ---------
// .program hello
// loop:
//   pull
//   out pins, 1
//   jmp loop

// ASM object corresponds to hello.pio
const asm = new ASM();
asm.label('loop').pull().out('pins', 1).jmp('loop');
```

### new ASM(options)

- **`options`** `<object>`   An option object.

  - **`sideset`** `<number>` The side-set count. Default: `0`.
  - **`sidesetOpt`** `<boolean>` The side-set opt. Default: `false`.
  - **`sidesetPindirs`** `<boolean>` The side-set pindirs. Default: `false`.

Instances of the ASM class can be created.

### asm.jmp([cond, ]target)

- **`cond`** `<string>` One of the strings: `'!x'`, `'x--'`, `'!y'`, `'y--'`, `'x!=y'`, `'pin'`, `'!osre'`. This parameter is optional.
- **`target`** `<string>` The target label name where to jump.

### asm.wait(pol, src, idx[, rel])

- **`pol`** `<number>`
- **`src`** `<string>` One of the strings: `'gpio'`, `'pin'`, `'irq'`.
- **`idx`** `<number>`
- **`rel`** `<string>` Use relative IRQ number if `'rel'` passed. this option works when `'src'` parameter is 'irq'.

### asm.in(src, bits)

- **`src`** `<string>` One of the strings: `'pins'`, `'x'`, `'y'`, `'null'`, `'isr'`, `'osr'`.
- **`bits`** `<number>`

### asm.out(dst, bits)

- **`dst`** `<string>` One of the strings: `'pins'`, `'x'`, `'y'`, `'null'`, `'pindirs'`, `'pc'`, `'isr'`, `'exec'`.
- **`bits`** `<number>`

### asm.push([iffull[, block]])

- **`iffull`** `<number|string>` `1` or `'iffull'` to set iffull flag to `1`, otherwise set to `0`.
- **`block`** `<number|string>` `1` or `'block'` means block, otherwise non-block (recommend to use `'noblock'`). Default: `1`.

### asm.pull([ifempty[, block]])

- **`ifempty`** `<number|string>` `1` or `'ifempty'` to set ifempty flag to `1`, otherwise set to `0`.
- **`block`** `<number|string>` `1` or `'block'` means block, otherwise non-block (recommend to use `'noblock'`). Default: `1`.

### asm.mov(dst, src)

- **`dst`** `<string>` One of the strings: `'pins'`, `'x'`, `'y'`, `'exec'`, `'pc'`, `'isr'`, `'osr'`.
- **`src`** `<string>` One of the strings: `'pins'`, `'x'`, `'y'`, `'null'`, `'status'`, `'isr'`, `'osr'`. Additionally you can use unary operators (`'~'` and `'!'` for invert, `'::'` for bit-reverse) in this `src` parameter like `'!x'`, `'~y'` or `'::pins'`.

### asm.irq([cmd ,]irqnum[, rel])

- **`cmd`** `<string>` One of the strings: `'set'`, `'nowait'`, `'wait'`, `'clear'`.
- **`irqnum`** `<number>` IRQ number to wait on.
- **`rel`** `<string>` Use relative IRQ number if `'rel'` passed.

### asm.set(dst, val)

- **`dst`** `<string>` One of the strings: `'pins'`, `'x'`, `'y'`, `'pindirs'`.
- **`val`** `<number>`

### asm.nop()

It means no operation, so it has no side effect.

### asm.label(name)

- **`name`** `<string>`

Creates a label at the position. It can be used as target in `jmp()`.

### asm.side(val)

- **`val`** `<number>`

Set side-set value to the latest instruction.

```javascript
// .program spi_tx_fast
// .side_set 1
//
// loop:
//   out pins, 1 side 0
//   jmp loop side 1

// ASM object corresponds to the above
const asm = new ASM({ sideset: 1 });
asm.label('loop').out('pins', 1).side(0).jmp('loop').side(1);
```

### asm.delay(val)

- **`val`** `<number>`

Set delay value to the latest instruction.

### asm.wrap_target()

Indicates the `.wrap_target` position of PIO assembly.

```javascript
// squarewave_fast.pio
// -------------------
// .program squarewave_fast
//   set pindirs, 1 ; Set pin to output
// .wrap_target
//   set pins, 1 ; Drive pin high
//   set pins, 0 ; Drive pin low
// .wrap

const squareware_fast_asm = new ASM();
squareware_fast_asm
  .set('pindirs', 1)
  .wrap_target()
  .set('pins', 1)
  .set('pins', 0)
  .wrap();

// You can access wrap_target and wrap offset
console.log('wrap_target:', squareware_fast_asm.labels['wrap_target']);
console.log('wrap:', squareware_fast_asm.labels['wrap']);
```

### asm.wrap()

Indicates the `.wrap` position of PIO assembly.

### asm.toBinary()

- **Returns**: `<Uint16Array>`

Returns the encoded binary code from the assembly program.

### asm.toInst([idx])

- `idx` `<number>` Index of instruction to export. Default: `0`.
- **Returns:** `<number>`

Returns an instruction at the specified index from the assembly program.

### asm.labels

- `<Object<string, number>>`

A map from label name to code offset. You can find `'wrap_target'` and `'wrap'` labels if you used.

## Class: StateMachine

An instance of StateMachine represents a state machine of RP2's PIO (Programmable I/O). You can use total 8 state machines (4 in PIO0 and 4 in PIO1) in RP2.

### StateMachine.getAvailableId()

- **Returns** `<number>` An available id for state machine.

Returns an available id (0~3 for PIO0 and 4~7 for PIO1) for state machine. It returns an id in sequence of 0, 4, 1, 5, 2, 6, 3, 7. If a `StateMachine` is instantiated with an id, then the id will be removed from the sequence.

It is useful when you writing a library based on PIO so that user don't concerns about assigning an id for state machine.

### new StateMachine(id, asm[, options])

- **`id`** `<number>` Id of the state machine. You can use total 8 state machines (0~7).
- **`asm` ** `<ASM>` An instance of ASM object to be executed by state machine.
- **`options`** `<object>`  An option object.

  - **`freq`** `<number>` Default: `125000000`.
  - **`inBase`** `<number>` Default: `-1`.
  - **`inCount`** `<number>` Default: `1`.
  - **`outBase`** `<number>` Default: `-1`.
  - **`outCount`** `<number>` Default: `1`.
  - **`setBase`** `<number>` Default: `-1`.
  - **`setCount`** `<number>` Default: `1`.
  - **`sidesetBase`** `<number>` Default: `-1`.
  - **`jmpPin`** `<number>` Default: `-1`.
  - **`inShiftDir`** `<number>` Default: `PIO.SHIFT_RIGHT`.
  - **`autopush`** `<boolean>` Default: `false`.
  - **`pushThreshold`** `<number>` Default: `32`.
  - **`outShiftDir`** `<number>` Default: `PIO.SHIFT_RIGHT`.
  - **`autopull`** `<boolean>` Default: `false`.
  - **`pullThreshold`** `<number>` Default: `32`.
  - **`fifoJoin`** `<number>` Default: `PIO.FIFO_JOIN_NONE`.
  - **`outSticky`** `<boolean>` Default: `false`.
  - **`outEnablePin`** `<number>` Default: `-1`.
  - **`movStatusSel`** `<number>` Default: `PIO.TX_LESSTHAN`.
  - **`movStatusN`** `<number>` Default: `0`.

### sm.active(value)

- **`value`** `<number>`

### sm.restart()

Restart the state machine.

### sm.exec(inst)

- **`inst`** `<number>` An instruction to execute.

Execute an PIO instruction.

```javascript
sm.exec(new ASM().out('pins', 1).toInst());
```

### sm.get()

- **Returns**: `<number>`

Pull a data from the state machine's RX FIFO.

### sm.put(value)

- **`value`** `<number|Uint32Array>`

Push a number (32bit unsigned integer) or unsigned 32bit integer array to the state machine's TX FIFO.

### sm.setPins(value[, mask])

- **`value`** `<number>` A value to set on all (or masked) pins.
- **`mask`** `<number>` A mask to filter pins to set the value.

Set a (32-bit) value on all (or masked) pins using the state machine.

### sm.rxfifo()

- **Returns**: `<number>`

Returns the number of elements in the state machine's RX FIFO. The size of RXFIFO is 0 in the `PIO.FIFO_JOIN_TX` mode, 4 in the `PIO.FIFO_JOIN_NONE` mode, 8 in the `PIO.FIFO_JOIN_RX` mode. This buffer size can be used to check RXFIFO full condition.

### sm.txfifo()

- **Returns**: `<number>`

Returns the number of elements in the state machine's TX FIFO. The size of RXFIFO is 0 in the `PIO.FIFO_JOIN_RX` mode, 4 in the `PIO.FIFO_JOIN_NONE` mode, 8 in the `PIO.FIFO_JOIN_TX` mode. This buffer size can be used to check TXFIFO full condition.

### sm.clearFIFOs()

Clear the state machine's TX FIFO and RX FIFO.

### sm.drainTXFIFO()

Empty out the state machine's TX FIFO.

### sm.irq(handler)

- **`handler`** `<Function>`

  - **`interrupt`** `<value>` interrupt value, bit0 is an interrupt to sm0, bit1 is an interrupt to sm1, bit2 is an interrupt to sm2, bit3 is an interrupt to sm3.

Binds a IRQ handler for the given PIO. PIO0 if current id is sm0~sm3, PIO1 if current id is sm4~sm7.

# RTC

The `rtc` module supports RTC (Real Time Clock). Use `require('rtc')` to access this module.

## setTime(time)

- **`time`** `<number>`

Set RTC time to the number of milliseconds since Unix Epoch (UTC 1970/1/1-00:00).

## getTime()

- **Returns** `<number>`

Read RTC time and returns the time as the number of milliseconds since Unix Epoch (UTC 1970/1/1-00:00).

This is equivalent to `Date.now()`.

# SDCard

The `sdcard` module provides block device for sdcard memory. Use `require('sdcard')` to access this module.

> We haven't tested all types of SDCard. The SD Standard Capacity(SDSC, up to 2GB) does not work well in this version. SD High Capacity(SDHC, more than 2GB up to 32GB) are tested (But there may be some interoperability issues). Please try differnt SD Card if you have issue when you use this module. Do not use this class without knowing about sdcard allocation map. Writing and erasing blocks in sdcard memory may broke your code, files, storage items in sdcard.

Here is an example code to mount sdcard (connected to `SPI0`) with FAT filesystem.

```js
const fs = require("fs");
const { SDCard } = require("sdcard");
const { VFSFatFS } = require("vfs_fat");

const sdcard = new SDCard(0, { cs: 5 }); // SPI0
fs.register("fat", VFSFatFS);
fs.mount("/sd", sdcard, "fat");
```

## Class: SDCard

An instance of `SDCard` represents a block device.

### new SDCard(bus[, options])

- **`bus`** `<number>` SPI bus number.

- **`options`** `<object>`

  The object of SPI options. _Default values for sck, mosi, miso are depends on board (Check in a board page).

  - **`sck`** `<number>` SPI SCK pin number.
  - **`mosi`** `<number>` SPI MOSI (TX) pin number.
  - **`miso`** `<number>` SPI MISO (RX) pin number.
  - **`cs`** `<number>` SPI CS pin number. Default is 0.

Returns a block device instance.

### sdcard.read(block, buffer[, offset])

- **`block`** `<number>` Block number to read.
- **`buffer`** `<Uint8Array>` Buffer where the read data to be stored.
- **`offset`** `<number>` Offset of `buffer`. Offset is ignored (always 0) for sdcard module.

Read the block data and store in `buffer`.

### sdcard.write(block, buffer[, offset])

- **`block`** `<number>` Block number to write.
- **`buffer`** `<Uint8Array>` Data to write.
- **`offset`** `<number>` Offset of `buffer`. Offset is ignored (always 0) for sdcard module.

Write the `buffer` data at the block.

### sdcard.ioctl(op[, arg])

- **`op`** `<number>` I/O operation.
- **`arg`** `<number>` Argument of the `op`.

Perform an I/O operation.

I/O operations are below:

- `1` : Initialize the device
- `2` : Shutdown the device (not supported)
- `3` : Synchronize the device (not supported)
- `4` : Returns the total number of blocks
- `5` : Returns the number of bytes in a block
- `6` : Erase a block (`arg` is the block number to erase)
- `7` : Returns the sdcard page size. The same as number of bytes in a block for sdcard.

# SPI

The `spi` module supports communication with devices using SPI (Serial Peripheral Interface) bus as the master device. Use `require('spi')` to access this module. A chip select pin of slave device need to be controlled by GPIO output pin.

## Class: SPI

An instances of `SPI` represents a SPI bus.

### SPI.MODE_0

- `<number>` = `0`

'SPI MODE0: CPOL = 0, CPHA = 0

### SPI.MODE_1

- `<number>` = `1`

'SPI MODE0: CPOL = 0, CPHA = 1

### SPI.MODE_2

- `<number>` = `2`

'SPI MODE0: CPOL = 1, CPHA = 0

### SPI.MODE_3

- `<number>` = `3`

'SPI MODE0: CPOL = 1, CPHA = 1

### SPI.MSB

- `<number>` = `0`

SPI bitorder, MSB is the first bit.

### SPI.LSB

- `<number>` = `1`

SPI bitorder, LSB is the first bit.

### new SPI(bus[, options])

- **`bus`** `<number>` SPI bus number.

- **`options`** `<object>`   The object of SPI options.

  Default values are depends on board (Check in a board page).

  - **`mode`** `<number>` `SPI.MODE_0` (CPOL=0/CPHA=0), `SPI.MODE_1` (CPOL=0/CPHA=1), `SPI.MODE_2` (CPOL=1/CPHA=0), or `SPI.MODE_3` (CPOL=1/CPHA=1). **Default:** `SPI.MODE_0`.
  - **`baudrate`** `<number>` Baud rate. **Default:** `3000000`, 3 Mbit/s
  - **`bitorder`** `<number>` `SPI.MSB (0)` or `SPI.LSB (1)` **Default:** `SPI.MSB (0)`.
  - **`sck`** `<number>` SPI SCK pin number. `-1` Not to use this pin. **Default:** board dependent. Check the `Boards` document.
  - **`mosi`** `<number>` SPI MOSI (TX) pin number. `-1` Not to use this pin. **Default:** board dependent. Check the `Boards` document.
  - **`miso`** `<number>` SPI MISO (RX) pin number. `-1` Not to use this pin. **Default:** board dependent. Check the `Boards` document.

- **Returns:** `<object>` An initialized SPI instance corresponds to the bus number. Once initialized, the same object will be returned.

Instances of the `SPI` class can be created using the new keyword or by calling spi.SPI() as a function. A `RangeError` will be thrown if **`bus`** is not less than max bus number. Please see [here](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface#Clock_polarity_and_phase) for more about SPI modes.

```javascript
// Create the SPI instance with master mode
const { GPIO } = require('gpio');
const { SPI } = require('spi');

var spi0cs = new GPIO(9, OUTPUT); // Set chip select to pin 9.
var spiOptions = {
  // SPI options
  mode: SPI.MODE_0,
  baudrate: 800000,
  bitorder: SPI.MSB,
};
var spi0 = new SPI(0, spiOptions);
// ChipSelect LOW
spi0cs.write(LOW);
// transfer data...
spi0.close(); // Close SPI bus 0
```

### spi.transfer(data[, timeout])

- **`data`** `<Uint8Array|string>` Data to write.
- **`timeout`** `<number>` Timeout in milliseconds. **Default:** `5000`.
- **Returns:** `<Uint8Array>` Received data or `null` if failed to transfer or timeout.

Send and receive data simultaneously via SPI bus.

```javascript
const { GPIO } = require('gpio');
const { SPI } = require('spi');

var spi0cs = new GPIO(9, OUTPUT); // Set chip select to pin 9.
var spiOptions = {
  // SPI options
  mode: SPI.MODE_0,
  baudrate: 800000,
  bitorder: SPI.MSB,
};
var spi0 = new SPI(0, spiOptions);
// ChipSelect LOW
spi0cs.write(LOW);
// Send two bytes and receive two bytes
var buf = spi0.transfer(new Uint8Array([0x88, 0x24])); // send and receive two byte data.
if (buf) {
  console.log(data.length); // == 2
  console.log(data[0]); // first byte
  console.log(data[1]); // second byte
}
```

### spi.send(data[, timeout[, count]])

- **`data`** `<Uint8Array|string>` Data to write.
- **`timeout`** `<number>` Timeout in milliseconds. **Default:** `5000`.
- **`count`** `<number>` Indicates how many times to send data. Default: `1`.
- **Returns:** `<number>` The number of bytes written, `-1` if it failed to write or timeout.

Send data via SPI bus

```javascript
const { GPIO } = require('gpio');
const { SPI } = require('spi');

var spi0cs = new GPIO(9, OUTPUT); // Set chip select to pin 9.
var spiOptions = {
  // SPI options
  mode: SPI.MODE_0,
  baudrate: 800000,
  bitorder: SPI.MSB,
};
var spi0 = new SPI(0, spiOptions);
// ChipSelect LOW
spi0cs.write(LOW);

// Send 2 bytes with an array of numbers
var array = new Uint8Array([0x6b, 0x00]);
spi0.send(array);
```

### spi.recv(length[, timeout])

- **`length`** `<number>` Data length to read.
- **`timeout`** `<number>` Timeout in milliseconds. **Default:** `5000`.
- **Returns:** `<Uint8Array>` Received data or `null` if failed to transfer or timeout.

Receive data via SPI bus.

```javascript
const { GPIO } = require('gpio');
const { SPI } = require('spi');

var spi0cs = new GPIO(9, OUTPUT); // Set chip select to pin 9.
var spiOptions = {
  // SPI options
  mode: SPI.MODE_0,
  baudrate: 800000,
  bitorder: SPI.MSB,
};
var spi0 = new SPI(0, spiOptions);
// ChipSelect LOW
spi0cs.write(LOW);

// Receive 10 bytes
var buf = spi0.recv(10);
if (buf) {
  console.log(data.length); // == 10
  console.log(data[0]); // first byte
  console.log(data[1]); // second byte
  // ...
}

spi0.close();
```

### spi.close()

Closes the SPI bus.

```javascript
const { GPIO } = require('gpio');
const { SPI } = require('spi');

var spi0cs = new GPIO(9, OUTPUT); // Set chip select to pin 9.
var spiOptions = {
  // SPI options
  mode: SPI.MODE_0,
  baudrate: 800000,
  bitorder: SPI.MSB,
};
var spi0 = new SPI(0, spiOptions);
// transfer data...
spi0.close(); // Close SPI bus 0
```

# Storage

The `storage` object provide the functions for storing key-value items in local persistent storage.

> Please note that typically internal flash memory is used to store data items. As you know flash memory has finite number of write/erase cycles (less than tens of thousands), so please do not use this storage API for the jobs need frequently write/erase cycles.

## Object: Storage

### storage.length

- `<number>`

Returns the number of data items in the storage.

```javascript
storage.setItem('key1', 'value1');
storage.setItem('key2', 'value2');
console.log(storage.length); // 2
```

### storage.getItem(key)

- **`key`** `<string>`

Return the value associated with the given key.

```javascript
storage.setItem('key1', 'value1');
var value1 = storage.getItem('key1');
var value2 = storage.getItem('key2'); // null
console.log(value); // "value1"
```

### storage.setItem(key, value)

- `key` `<string>`
- `value` `<string>`

Add the key with the value to the storage.

```javascript
storage.setItem('key1', 'value1');
var value1 = storage.getItem('key1'); // "value1"
storage.setItem('key1', 'new value');
value1 = storage.getItem('key1'); // "new value"
```

### storage.removeItem(key)

- **`key`** `<string>`

Remove the data item associated with the given key.

```javascript
storage.setItem('key1', 'value1');
storage.removeItem('key1');
var value1 = storage.getItem('key1'); // null
```

### storage.clear()

Remove all the data items in the storage.

```javascript
storage.setItem('key1', 'value1');
storage.setItem('key2', 'value2');
storage.clear();
var value1 = storage.getItem('key1'); // null
var value2 = storage.getItem('ket2'); // null
```

### storage.key(index)

- **`index`** `<number>`
- **Returns:** `<string>`

Returns the key string of the given index.

```javascript
storage.setItem('key1', 'value1');
storage.setItem('key2', 'value2');
console.log(storage.length); // 2
console.log(storage.key(0)); // "key1"
console.log(storage.key(1)); // "key2"
```

# Stream

> **THIS IS EXPERIMENTAL AND SUBJECT OF CHANGE**

This `steam` module provides abstract interfaces for working with streaming data. Use `require('stream')` to access this module.

## Class: Stream

- Extends: [`EventEmitter`](https://kalumajs.org/docs/api/events/#class-eventemitter)

This is a class for stream.

### stream.readable

- `{boolean}`

Indicates whether the stream is readable or not.

### stream.writable

- `{boolean}`

Indicates whether the stream is writable.

### stream.read([size])

- **`size`** `{number}`
- **Returns:** `{Uint8Array|null}`

Read data as the specified `size` from the stream. If `size` not specified, reads all data in the stream buffer.

If the size of data in the stream buffer is less then `size`, returns `null`.

### stream.write(chunk)

- **`chunk`** `{Uint8Array}`
- **Returns:** `{number}`

Writes `chuck` to the stream and returns the number of bytes written.

### stream.end()

Finishes to write.

### stream.destroy()

- Returns: `{this}`

Closes the stream.

## Class: Readable

> THIS CLASS WILL BE DEPRECATED

Readable streams are an abstraction for a *source* from which data is consumed. This is a condensed version of Readable stream in [Node.js](https://nodejs.org/).

### readable.readableEnded

- `{boolean}`

Becomes `true` when [`'end'`](https://kalumajs.org/docs/api/stream#event-end) event is emitted.

### readable.push(chunk)

- **`chunk`** `{Uint8Array}`

*This function should not be called in application code. It is only for stream implementors.* This should be called when a chunk of data is arrived from the stream.

### readable.destroy()

- Returns: `{this}`

Closes the stream.

### Event: 'data'

- **`chunk`** `{Uint8Array}`

Emitted whenever a chunk of data is available to consume.

### Event: 'end'

Emitted when all data completely consumed and no more data to read.

### Event: 'error'

- **`err`** `{Error}`

Emitted when error occurred.

### Event: 'close'

Emitted when the stream is closed.

## Class: Writable

> THIS CLASS WILL BE DEPRECATED

Writable streams are an abstraction for a *destination* to which data is written. This is a condensed version of Writable stream in [Node.js](https://nodejs.org/).

### writable.writableEnded

- `{boolean}`

Becomes `true` when [`writable.end()`](https://kalumajs.org/docs/api/stream#writable-end-chunk-callback) is called.

### writable.writableFinished

- `{boolean}`

Becomes `true` when [`'finish'`](https://kalumajs.org/docs/api/stream#event-finish) event is emitted.

### writable.write(chunk[, callback])

- **`chunk`** `{Uint8Array|string}` A chunk of data to write.
- **`callback`** `{Function}` Called when the chunk of data is handled.
- Returns: `{boolean}`

Writes a chunk of data on the stream. It returns `true` if the stream is ready to handle the next chunk of data. It returns `false`, then the stream will emit [`'drain'`](https://kalumajs.org/docs/api/stream#event-drain) event when the all data written are handled and the internal buffer is empty. If you write data before [`'drain'`](https://kalumajs.org/docs/api/stream#event-drain) event emitted, the chunk of data will be appended to the internal buffer.

### writable.end([chunk][, callback])

- **`chunk`** `{Uint8Array|string}` A chunk of data to write before finish.
- **`callback`** `{Function}` Called when the stream is finished.
- Returns: `{this}`

Finishes to write data on the stream. The event [`'finish'`](https://kalumajs.org/docs/api/stream#event-finish) will be emitted after successfully finished.

### writable._write(chunk[, callback])

- **`chunk`** `{Uint8Array|string}`
- **`callback`** `{Function}`

*This function should not be called in application code. It is only for stream implementors.* This function must be implemented in the inherited stream classes.

### writable._final([callback])

- **`callback`** `{Function}`

*This function should not be called in application code. It is only for stream implementors.* This function must be implemented in the inherited stream classes.

### writable.destroy()

- Returns: `{this}`

Closes the stream.

### Event: 'drain'

Emitted when the stream is ready to write and the internal buffer is empty.

### Event: 'finish'

Emitted when the stream is finished.

### Event: 'error'

- **`err`** `{Error}`

Emitted when error occurred.

### Event: 'close'

Emitted when the stream is closed.

## Class: stream.Duplex

Extends: [`stream.Readable`](https://kalumajs.org/docs/api/stream#class-readable), [`stream.Writable`](https://kalumajs.org/docs/api/stream#class-writable)

> THIS CLASS WILL BE DEPRECATED

Duplex streams are streams that implement both the [Readable](https://kalumajs.org/docs/api/stream#class-readable) and [Writable](https://kalumajs.org/docs/api/stream#class-writable) interfaces.

# Timers

## delay(msec)

- **`msec`** `<number>` The delay value in milliseconds.

Waits for **`msec`** milliseconds.

> This function would block all the process for **`msec`** milliseconds, so please use it carefully. It will block the entire console. `setTimeout`, `setInterval` is strongly recommended instead of this function.

```javascript
delay(3000); // delay 3 seconds.
```

## millis()

- **Returns:** `<number>` The current timestamp in milliseconds.

Returns the number of milliseconds elapsed since January 1, 1970 00:00:00 UTC.

```javascript
// Show the processing time of the function.
var startTime = millis(); // store start time.
for (var i = 1; i <= 10; i++) console.log('Processing...', i * 10, '%');
var endTime = millis(); //store end time.
console.log('Processing time is ', endTime - startTime, 'ms.');
```

## delayMicroseconds(usec)

- **`usec`** `<number>` The delay value in microseconds.

Waits for **`usec`** microseconds. It provides finer grained time delays than `delay()`.

> This function is not so time correct, so don't use it for time critical job.

## micros()

- **Returns:** `<number>` The current timestamp in microseconds.

Returns the number of microseconds elapsed since the system boot. Typically it will overflow after approximately 70 minutes.

> This function is not so time correct, so don't use it for time critical job.

## setTimeout(callback, timeout)

- **`callback`** `<function>` The function which is called after **`timeout`** milliseconds
- **`timeout`** `<number>` The timeout value in milliseconds.
- **Returns**: `<number>` Timer id.

Set the timeout event which call the **`callback`** function after **`delay`** milliseconds.

```javascript
// Show the message after 1 sec.
var timerId = setTimeout(function () {
  print('done.');
}, 1000); // 1sec timeout
```

## setInterval(callback, interval)

- **`callback`** `<function>` The function which is called at every **`interval`** milliseconds.
- **`interval`** `<number>` The time interval value in milliseconds.
- **Returns**: `<number>` Timer id.

Set the interval event which call the **`callback`** function every**`interval`** milliseconds.

```javascript
// Print "tick" for every seconds
var timerId = setInterval(function () {
  print('tick');
}, 1000); // 1sec interval
// ...
clearInterval(timerId); // To stop printing "tick"
```

## clearTimeout(id)

- **`id`** `<number>` Timer id.

Clear timeout event which is set using `setTimeout()`.

```javascript
// Show the message after 1 sec.
var timerId = setTimeout(function () {
  print('done.');
}, 1000); // 1sec timeout
// ...
clearTimeout(timerId); // clear it after the timeout event.
```

## clearInterval(id)

- **`id`** `<number>` Timer id.

Clear interval event which is set using `setInterval()`.

```javascript
// Print "tick" for every seconds and clear it.
var timerId = setInterval(function () {
  print('tick');
}, 1000); // 1sec interval
// ...
clearInterval(timerId); // clear it to stop printing "tick"
```

# UART

The `uart` module supports communication with computers or devices using serial ports (UART). Use `require('uart')` to access this module.

## Class: UART

An instances of `UART` represents a UART port.

### UART.PARITY_NONE

- `<number>` = `0`.

Do not use a parity bit.

### UART.PARITY_ODD

- `<number>` = `1`.

Use odd parity bit

### UART.PARITY_EVEN

- `<number>` = `2`.

Use even parity bit

### UART.FLOW_NONE

- `<number>` = `0`.

Do not use flow control.

### UART.FLOW_RTS

- `<number>` = `1`.

Use RTS flow control.

### UART.FLOW_CTS

- `<number>` = `2`.

Use CTS flow control.

### UART.FLOW_RTS_CTS

- `<number>` = `3`.

Use both RTS and CTS flow control.

### new UART(port[, options])

- **`port`** `<number>` UART port number. This value should be less than max port number.

- **`options`** `<object> ` The object of UART options.

  Default values are depends on board (Check in a board page).

  - **`baudrate`** `<number>` Baud rate. One of the `[0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400]`.
  - **`bits`** `<number>` Number of bits per a character. One of the `[8, 9]`. **Default:**`8`
  - **`parity`** `<number>`. The parity is one of the `UART.PARITY_NONE (0)`, `UART.PARITY_ODD (1)`, or `UART.PARITY_EVEN (2)`. **Default:** `UART.PARITY_NONE`.
  - **`stop`** `<number>` Number of stop bits. One of the `[1, 2]`. **Default:** `1`.
  - **`flow`** `<number>` Flow control. One of the `UART.FLOW_NONE (0)`, `UART.FLOW_RTS (1)`, `UART.FLOW_CTS (2)`, or `UART.FLOW_RTS_CTS (3)`. **Default:** `UART.FLOW_NONE`
  - **`bufferSize`** `<number>` The size of internal read buffer.
  - **`tx`** `<number>` UART TX pin number. `-1` Not to use this pin. **Default:** board dependent. Check the `Boards` document.
  - **`rx`** `<number>` UART RX pin number. `-1` Not to use this pin. **Default:** board dependent. Check the `Boards` document.
  - **`cts`** `<number>` UART CTS pin number. `-1` Not to use this pin. **Default:**`-1`
  - **`rts`** `<number>` UART RTS pin number. `-1` Not to use this pin. **Default:**`-1`

```javascript
// Create the UART instance and close it
const { UART } = require('uart');
const options = {
  baudrate: 9600,
  bits: 8,
  partity: UART.PARTIY_NONE,
  stop: 1,
  flow: UART.FLOW_NONE,
  bufferSize: 2048,
};
const serial0 = new UART(0, options);
// read or write data...
serial0.close();
```

### write(data[, options])

- **`data`** `<Uint8Array|string>` Data to write.

- **`options`** `<object>`

  - **`count`** `<number>` Indicates how many times to write data. **Default:** `1`.

Writes data to the UART port.

```javascript
const { UART } = require('uart');
const serial0 = new UART(0, { baudrate: 38400 });

// Write a string
serial0.write('Hello, world\n');

// Write Uint8Array
serial0.write(new Uint8Array([0x00, 0x7f, 0x80, 0xff]));

serial0.close();
```

### close()

Close the UART port.

### Event: 'data'

- **`data`** `<Uint8Array>` Received data.

The `data` event is emitted whenever data is arrived (buffer size may varies).

```javascript
const { UART } = require('uart');
const serial0 = new UART(0);
serial0.on('data', (data) => {
  var s = String.fromCharCode.apply(null, data);
  print(s);
});
```

# URL

The `url` module supports URL parsing and formatting. Use `require('url')` to access this module.

## Class: URL

A URL string is a structured string containing multiple meaningful components. For example `https://user:pass@sub.example.com:8080/p/a/t/h?query=string#hash` is consists of below components:

- protocol: `https:`
- username: `user`
- password: `pass`
- hostname: `sub.example.com`
- port: `8080`
- pathname: `/p/a/t/h`
- search: `?query=string`
- hash: `hash`

### new URL(input)

- **`input`** `<string>`

Creates a URL object by parsing the input string.

### url.hash

- `<string>`

Gets and sets the fragment portion of the URL.

### url.host

- `<string>`

Returns readonly host portion of the URL.

### url.hostname

- `<string>`

Gets and sets the hostname portion of the URL.

### url.href

- `<string>`

Returns serialized URL string.

### url.origin

- `<string>`

Returns readonly origin portion of the URL.

### url.password

- `<string>`

Gets and sets the password portion of the URL.

### url.pathname

- `<string>`

Gets and sets the pathname portion of the URL.

### url.port

- `<string>`

Gets and sets the port portion of the URL.

### url.protocol

- `<string>`

Gets and sets the protocol portion of the URL.

### url.search

- `<string>`

Gets and sets the search string portion of the URL.

### url.searchParams

- `<URLSearchParams>`

Returns [URLSearchParams](https://kalumajs.org/docs/api/url.md#class-urlsearchparams) object constructed from `search` property.

### url.username

- `<string>`

Gets and sets the username portion of the URL.

### url.toString()

- **Returns:** `<string>`

Returns serialized URL string.

### url.toJSON()

- **Returns:** `<string>`

Returns serialized URL string.

## Class: URLSearchParams

This class allows to read and write access to the query string of a URL.

### new URLSearchParams(input)

- **`input`** `<string>` A query string

Parses the input query string. A leading `'?'` characters is ignored.

### urlSearchParams.append(name, value)

- **`name`** `<string>`
- **`value`** `<string>`

Append a name-value pair.

### urlSearchParams.delete(name)

- **`name`** `<string>`

Delete all name-value pairs whose name is `name`.

### urlSearchParams.entries()

- **Returns:** `<Array>`

Returns all name-value pairs as an array.

### urlSearchParams.get(name)

- **`name`** `<string>`
- **Returns:** `<string>`

Returns the value of first name-value pairs whose name is `name`.

### urlSearchParams.getAll(name)

- **`name`** `<string>`
- **Returns:** `<string[]>`

Returns the values of all name-value pairs whose name is `name`.

### urlSearchParams.has(name)

- **`name`** `<string>`
- **Returns:** `<boolean>`

Returns `true` if there is at least one of name-value pair whose name is `name`.

### urlSearchParams.keys()

- **Returns:** `<string[]>`

Returns all names of all name-value pairs.

### urlSearchParams.set(name, value)

- **`name`** `<string>`
- **`value`** `<string>`

Set the value of first name-value pair and deletes all other name-value pairs whose name is `name`. If not exists, append a name-value pair.

### urlSearchParams.values()

- **Returns:** `<string[]>`

Returns all values of all name-value pairs.

### urlSearchParams.toString()

- **Returns:** `<string>`

Serializes and returns a query string from all name-value pairs with percent encoding.

# WDT

The `wdt` module provides the WDT(Watchdog timer) class which supports Watchdog timer. Use `require('wdt')` to access this module.

## Class: WDT

An instances of `WDT` represents a WDT object.

### new WDT(timeout)

- **`timeout`** `<number>` milliseconds of the watchdog timeout.
- **Returns:** `<object>` The return value is `WDT` object.

Start watchdog timer. the system is reset if watchdog timer is expired.

```javascript
const { WDT } = require('wdt');
var wdt = new WDT(100); // 100 ms watchdog timer settings
```

### wdt.feed()

This method reset watdog reset timer. To avoid watchdog reset, this function should be called before watchdog timer expiration.

```javascript
const { WDT } = require('wdt');
const wdt = new WDT(1000); // 1 sec watchdog timeout

var timer = 0;

var timer_id = setInterval(()=> {
    wdt.feed(); // Reset watchdog timer
    timer += 500;
    console.log("Reset watchdog timer " + timer + " ms");
    if (timer > 10000) { // Do not reset for 10 sec.
        clearInterval(timer_id);
        console.log("Watchdog reset after 1 sec.");
    }
}, 500);
```

# Wi-Fi

> **THIS IS EXPERIMENTAL AND SUBJECT OF CHANGE**

The `wifi` module supports to scan, connect and disconnect Wi-Fi networks. Use `require('wifi')` to access this module.

> This module requires a [IEEE 802.11 device driver](https://kalumajs.org/docs/api/device-driver/#ieee-80211-dev). If the board you are using is not capable for Wi-Fi, you may need to inject external Wi-Fi device driver.

## Class: WiFi

An instance of `WiFi` represents a Wi-Fi device.

### new WiFi()

Create an instance of `Wi-Fi` class.

```javascript
var WiFi = require('wifi').WiFi;
var wifi = new WiFi();
```

### wifi.reset([callback])

- **`callback`** `{Function}` Called when reset complete.

Reset the Wi-Fi device.

### wifi.scan([callback])

- **`callback`** `{Function}`

- **`err`** `{Error}`
  
- **`scanResults`** `{Array<Object>}`

    - **`security`** `{string}` Security. `OPEN`, or multiple of `WEP`, `WPA`, `PSK`, `WPA2`, `WPA2-EAP`.
    - **`ssid`** `{string}` SSID.
    - **`rssi`** `{number}` Received signal strength indication.
    - **`bssid`** `{string}` _**_BSSID. (Typically MAC address)
    - **`channel`** `{number}` Channel.

Scan Wi-Fi networks.

```javascript
const { WiFi } = require('wifi');
const wifi = new WiFi();

wifi.scan((err, scanResults) => {
  if (err) {
    console.error(err);
  } else {
    console.log(scanResults);
  }
});
```

### wifi.connect([connectInfo][, callback])

- **`connectInfo`** `{object}`   Information to connect a Wi-Fi network.

  - **`ssid`** `{string}` SSID.
  - **`password`** `{string}` Password. Default: `undefined`.
  - **`bssid`** `{string}` O_**_BSSID. (Typically MAC address). Default: `undefined`.
  - **`security`** `{string}` Security. `OPEN`, `WPA2_WPA_PSK`, `WPA2_PSK`, `WPA_PSK`, `WEP_PSK`. Defaule: `OPEN` if password is not set or length of the password is less than 8 characters. `WPA2_WPA_PSK` if length of the password is greater or equal to 8 characters.
  - **`enforce`** `{boolean}` When set to `true`, enforce to connect even if there is already a Wi-Fi connection. Otherwise, do not try to connect if there is Wi-Fi connection. Default: `false`.

- **`callback`** `{Function}`  A callback function called when a Wi-Fi connection is established. This is also called when there is already a Wi-Fi connection.
  - **`err`** `{Error}`
  - **`connectInfo`** `{Object}`

Establish a connection to Wi-Fi network.

```javascript
const { WiFi } = require('wifi');
const wifi = new WiFi();

wifi.connect({ ssid: 'MyHome', password: '12345678' }, (err) => {
  if (err) {
    console.error(err);
  } else {
    // add your code using Wi-fi connection
  }
});
```

If you do not want to expose your Wi-Fi connection info, you can set them in the storage. (Do not add item in the storage in the source code, add manually in Terminal)

```javascript
storage.setItem('WIFI_SSID', 'MyHome');
storage.setItem('WIFI_PASSWORD', '12345678');
storage.setItem('WIFI_SECURITY', 'WPA2_WPA_PSK');
```

And then, call `connect` method without `connectInfo` argument as below.

```javascript
const { WiFi } = require('wifi');
const wifi = new WiFi();

wifi.connect((err) => {
  if (err) {
    console.error(err);
  } else {
    // add your code using Wi-fi connection
  }
});
```

### wifi.disconnect([callback])

- **`callback`** `{Function}`
  - **`error`** `{Error}`

Disconnect from currently connected Wi-Fi network.

### wifi.getConnection([callback])

- **`callback`** `{Function}`
  - **`error`** `{Error}`

  - **`connectionInfo`** `{object}`

    - **`ssid`** `{string}`
    - **`bssid`** `{string}`

Get connection information of currently connected Wi-Fi network.

### Event: 'associated'

This event is emitted when Wi-Fi network is connected. IP may not be assigned.

### Event: 'connected'

This event is emitted when Wi-Fi network is connected with IP assignment.

### Event: 'disconnected'

This event is emitted when Wi-Fi network is disconnected.