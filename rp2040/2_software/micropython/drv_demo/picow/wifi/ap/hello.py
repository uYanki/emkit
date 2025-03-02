import network

ssid = 'PICOW'
password = '12345678'
ap = network.WLAN(network.AP_IF)
ap.config(essid=ssid, password=password)
ap.active(True)
