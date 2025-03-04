# 功能：拍照

import camera
from time import sleep


def sanpshot():
    try:
        if camera.deinit():
            sleep(0.5)
        if camera.init(0, format=camera.JPEG, fb_location=camera.PSRAM):
            sleep(0.5)
            with open("snapshot.jpeg", "wb") as f:
                img = camera.capture()
                f.write(img)
                return True
    except Exception as e:
        pass
    return False


print('success' if sanpshot() else 'error')
