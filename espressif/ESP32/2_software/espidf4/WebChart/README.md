# esp-idf-web-chart
This is a demonstration of __real-time data visualization__ using a web browser.   
The purpose of this project is to demonstrate how to use components that enable real-time data visualization.   
Although it use as data to display ADC converted values, it can also be applied to data from sensors such as thermometers and hygrometers.   

The ESP32 has two ADCs: ADC1 and ADC2.   
You can use these to convert analog values to digital values.   
The analog values change dynamically and are suitable for this demonstration.
This project uses ADC1.   

![web-meter](.assest/README/164379601-68aaf0e3-f50c-4776-8de1-216ce94d63df.jpg)
![web-horizontal](.assest/README/164379617-143ab49b-af77-4cfe-9d65-6f213d724d28.jpg)
![web-vertical](.assest/README/164379627-dac0078c-0a25-45bb-941f-d5588c87413b.jpg)
![Chartjs](.assest/README/164872690-abad13da-563f-44a4-b579-b4dd25598c33.jpg)
![Plotly](.assest/README/164872660-be85b191-c0ed-4f06-b04c-1ba6c020d6d7.jpg)
![Epoch](.assest/README/164948839-9a6997b3-6c40-441c-841e-be1a32b36890.jpg)

I used [this](https://github.com/Molorius/esp32-websocket) component.   
This component can communicate directly with the browser.   

# Software requirements
ESP-IDF V4.4/V5.x.   
ESP-IDF V5.0 is required when using ESP32-C2.
ESP-IDF V5.1 is required when using ESP32-C6.

# Installation for ESP-IDF V4.4
```
git clone -b v4.4 https://github.com/nopnop2002/esp-idf-web-chart
cd esp-idf-web-chart/RadialGauge/
git clone https://github.com/Molorius/esp32-websocket components/websocket
idf.py set-target {esp32/esp32s2/esp32s3/esp32c3}
idf.py menuconfig
idf.py flash monitor
```

# Installation for ESP-IDF V5.x
```
git clone https://github.com/nopnop2002/esp-idf-web-chart
cd esp-idf-web-chart/RadialGauge/
git clone https://github.com/Molorius/esp32-websocket components/websocket
idf.py set-target {esp32/esp32s2/esp32s3/esp32c2/esp32c3/esp32c6}
idf.py menuconfig
idf.py flash monitor
```

# Configuration
![config-top](.assest/README/164379960-58350b2d-17d4-48b5-84d1-615ff037242a.jpg)
![config-app](.assest/README/164379982-149e4044-7889-4755-813e-0185fd082c9b.jpg)


## WiFi Setting
Set the information of your access point.
![config-wifi-1](.assest/README/164383151-ea783d1c-406b-42d5-9767-2e6911be9b2f.jpg)

You can connect using the mDNS hostname instead of the IP address.   
![config-wifi-2](.assest/README/164380164-8be36ca2-a5c4-402e-b83d-d21513e66c55.jpg)

## ADC Setting
Set the information of gpio for analog input.
![config-adc-1](.assest/README/164380386-c6dffeb8-9bdd-46bf-8e55-9c4ecef16090.jpg)

It is possible to monitor 3 channels at the same time.   
![config-adc-2](.assest/README/164380399-fe125c4f-006d-48cb-9e4e-f104c389b8b5.jpg)

Analog input gpio for ESP32 is GPIO32 ~ GPIO39. 12Bits width.   
Analog input gpio for ESP32S2 is GPIO01 ~ GPIO10. 13Bits width.   
Analog input gpio for ESP32S3 is GPIO01 ~ GPIO10. 12Bits width.   
Analog input gpio for ESP32C2 is GPIO00 ~ GPIO04. 12Bits width.   
Analog input gpio for ESP32C3 is GPIO00 ~ GPIO04. 12Bits width.   
Analog input gpio for ESP32C6 is GPIO00 ~ GPIO06. 12Bits width.   

# ADC Attenuation   
This project uses ADC_ATTEN_DB_12(12dB) for attenuation.   
12dB attenuation (ADC_ATTEN_DB_12) gives full-scale voltage 3.9V.   
But the range that can be measured accurately is as follows:   
- Measurable input voltage range for ESP32 is 150 mV ~ 2450 mV.   
- Measurable input voltage range for ESP32S2 is 0 mV ~ 2500 mV.   
- Measurable input voltage range for ESP32S3 is 0 mV ~ 3100 mV.   
- Measurable input voltage range for ESP32C3 is 0 mV ~ 2500 mV.   
- Measurable input voltage range for ESP32C2 is 0 mV ~ 2800 mV.   


# Analog source
Connect ESP32 and Analog source using wire cable.   
I used a variable resistor for testing.
```
                                          +---------------------------+
                                          |      variable resistor    |
ESP32 3.3V   -----------------------------+ Ra of variable resistor   |
                                          |                           |
                                          |                           |
ESP32 GPIO32 -------------------------+---+ Vout of variable resistor |
                                      |   |                           |
                  R1      R2      R3  |   |                           |
ESP32 GND    ----^^^--+--^^^--+--^^^--+   |                           |
                      |       |           |                           |
                      |       |           |                           |
ESP32 GPIO33 ---------+       |           |                           |
                              |           |                           |
                              |           |                           |
ESP32 GPIO34 -----------------+           |                           |
                                          |                           |
                                          |                           |
ESP32 GND    -----------------------------+ Rb of variable resistor   |
                                          |                           |
                                          +---------------------------+
```

# Launch a web browser   
Enter the following in the address bar of your web browser.   

```
http:://{IP of ESP32}/
or
http://esp32-server.local/
```

# Using mDNS hostname
You can connect using the mDNS hostname instead of the IP address.   
- esp-idf V4.4  
 If you set CONFIG_MDNS_STRICT_MODE = y in sdkconfig.defaults, the firmware will be built with MDNS_STRICT_MODE.   
 __If MDNS_STRICT_MODE is not set, mDNS name resolution will not be possible after long-term operation.__   
- esp-idf V4.4.1   
 mDNS component has been updated.   
 If you set CONFIG_MDNS_STRICT_MODE = y in sdkconfig.defaults, the firmware will be built with MDNS_STRICT_MODE.   
 __Even if MDNS_STRICT_MODE is set, mDNS name resolution will not be possible after long-term operation.__   
- esp-idf V5.0 or later   
 mDNS component has been updated.   
 Long-term operation is possible without setting MDNS_STRICT_MODE.   
 The following lines in sdkconfig.defaults should be removed before menuconfig.   
 ```CONFIG_MDNS_STRICT_MODE=y```


# WEB Pages
WEB pages are stored in the html folder.   
You can change it as you like.   