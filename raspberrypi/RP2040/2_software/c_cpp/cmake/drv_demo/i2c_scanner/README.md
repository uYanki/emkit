`bi_decl ` 

作用：查看 bin、elf、uf2 文件中的 引脚复用、内存分配、编译日期 等信息。

```shell
$ picotool info -a i2c_scanner.uf2

File i2c_scanner.uf2:      

Program Information        
 name:          i2c_scanner
 binary start:  0x10000000 
 binary end:    0x100049cc 

Fixed Pin Information
 16:  I2C0 SDA
 17:  I2C0 SCL

Build Information
 sdk version:       1.5.1
 pico_board:        pico
 boot2_name:        boot2_w25q080
 build date:        Aug 21 2023
 build attributes:  Release
```

