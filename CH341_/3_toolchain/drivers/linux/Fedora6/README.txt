ch34x linux驱动内核2.6.18使用说明
驱动安装
（1）手工安装驱动
	a.获得驱动：你可以通过E-mail方式请求驱动，发送邮件到tech@wch.cn,请务必注明驱动内核版本。
	b.加载usbserial.ko
		insmod usbserial.ko
	c.加载ch34x.ko
		insmod ch34x.ko
	d.如果安装成功会在/dev目录下多出个ttyUSB0设备，理论上最多可支持255个设备。依次为ttyUSB1,ttyUSB2...
（2）脚本安装驱动
	#!/bin/sh
	insmod /root/my_driver/ch34x/usbserial.ko
	insmod /root/my_driver/ch34x/ch34x.ko
