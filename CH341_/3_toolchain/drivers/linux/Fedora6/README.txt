ch34x linux�����ں�2.6.18ʹ��˵��
������װ
��1���ֹ���װ����
	a.��������������ͨ��E-mail��ʽ���������������ʼ���tech@wch.cn,�����ע�������ں˰汾��
	b.����usbserial.ko
		insmod usbserial.ko
	c.����ch34x.ko
		insmod ch34x.ko
	d.�����װ�ɹ�����/devĿ¼�¶����ttyUSB0�豸������������֧��255���豸������ΪttyUSB1,ttyUSB2...
��2���ű���װ����
	#!/bin/sh
	insmod /root/my_driver/ch34x/usbserial.ko
	insmod /root/my_driver/ch34x/ch34x.ko
