# 功能：20种物体检测（需烧录带`kmodel_v4`的固件）

import sensor
import image
import lcd
import time
import KPU as kpu
import gc
import sys


def lcd_show_except(e):
    import uio
    err_str = uio.StringIO()
    sys.print_exception(e, err_str)
    err_str = err_str.getvalue()
    img = image.Image(size=(224, 224))
    img.draw_string(0, 10, err_str, scale=1, color=(0xff, 0x00, 0x00))
    lcd.display(img)


def main(model_addr=0x300000, lcd_rotation=0, sensor_hmirror=False, sensor_vflip=False):
    try:
        sensor.reset()
    except Exception as e:
        raise Exception("sensor reset fail, please check hardware connection, or hardware damaged! err: {}".format(e))
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)
    sensor.set_hmirror(sensor_hmirror)
    sensor.set_vflip(sensor_vflip)
    sensor.run(1)

    lcd.init(color=lcd.WHITE)
    lcd.rotation(lcd_rotation)

    anchors = (1.08, 1.19, 3.42, 4.41, 6.63, 11.38, 9.42, 5.11, 16.62, 10.52)
    classes = ['aeroplane', 'bicycle', 'bird', 'boat', 'bottle', 'bus', 'car', 'cat', 'chair', 'cow',
               'diningtable', 'dog', 'horse', 'motorbike', 'person', 'pottedplant', 'sheep', 'sofa', 'train', 'tvmonitor']
    try:
        task = None
        task = kpu.load(model_addr)
        kpu.init_yolo2(task, 0.5, 0.3, 5, anchors)  # threshold:[0,1], nms_value: [0, 1]
        while (True):
            img = sensor.snapshot()
            t = time.ticks_ms()
            objects = kpu.run_yolo2(task, img)
            t = time.ticks_ms() - t
            if objects:
                for obj in objects:
                    img.draw_rectangle(obj.rect())
                    img.draw_string(obj.x(), obj.y(), classes[obj.classid()], color=lcd.RED, mono_space=False)
                    img.draw_string(obj.x(), obj.y()+12, '%f1.3' % obj.value(), color=lcd.RED, mono_space=False)
            img.draw_string(0, 0, "t:%dms" % (t))
            lcd.display(img)
    except Exception as e:
        raise e
    finally:
        if not task is None:
            kpu.deinit(task)


if __name__ == "__main__":
    try:
        # main( model_addr=0x300000, lcd_rotation=0, sensor_hmirror=False, sensor_vflip=False)
        main(model_addr="/sd/objectdetect.smodel", lcd_rotation=2)
    except Exception as e:
        sys.print_exception(e)
        lcd_show_except(e)
    finally:
        gc.collect()
