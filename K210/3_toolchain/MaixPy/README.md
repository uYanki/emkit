01Studio：www.01Studio.cc

Maix-I docs：https://wiki.sipeed.com/hardware/zh/maix/index.html

MaixPy docs：https://wiki.sipeed.com/soft/maixpy/zh/index.html

kendryte：https://github.com/kendryte/K210-Micropython-OpenMV

openmv docs：

- https://book.openmv.cc

- https://docs.singtown.com/micropython/zh/latest/openmvcam/index.html

cuda：https://docs.singtown.com/micropython/zh/latest/openmvcam/index.html

Mx-yolov3不能用GPU训练解决方法：https://blog.csdn.net/fanre/article/details/123979389?utm_source=app&app_version=5.3.1&code=app_1562916241&uLinkId=usr1mkqgl919blen

## 固件烧录

##### kflash_gui（烧录工具）：https://github.com/sipeed/kflash_gui/releases

##### Firmware（固件）：https://dl.sipeed.com/shareURL/MAIX/MaixPy/release/master

- 后缀名为`.bin`或`.kfpkg`。（`.kfpkg`其实是多个`.bin`的打包版本）

##### Model（模型）：https://dl.sipeed.com/shareURL/MAIX/MaixPy/model

* `face_model_at_0x300000.kfpkg`：人脸模型放在地址为`0x300000`的位置。
* 在线模型训练：https://maix.sipeed.com/home


## 开发环境

##### XCOM

![1.use xcom to debug](5.Images\1.use xcom to debug.png)

* 可一次发送多条指令，每条指令需以换行符结尾。

##### Thonny：https://thonny.org/

##### MaixPy IDE：https://dl.sipeed.com/shareURL/MAIX/MaixPy/ide

![1.use maixpy ide to debug](5.Images\1.use maixpy ide to debug.png)

## 代码上传

![2.how to upload file](5.Images\2.how to upload file.png)



