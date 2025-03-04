@echo off
reg query "HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\SERIALCOMM"
echo "[erase flash]"
python -m esptool --chip ESP32 erase_flash
echo "[write fireware]"
python -m esptool --chip esp32 --baud 921600 write_flash -z 0x0 maixduino_esp32_firmware_v1.4.1_0x0.bin
echo "[fireware info]"
python -m esptool --chip esp32 flash_id
echo "[press any key to exit]"
pause
