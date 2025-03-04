

all:	module  



install: module
	mkdir -p /lib/modules/$(shell uname -r)/kernel/drivers/usb/
	cp -f ./ch34x.ko /lib/modules/$(shell uname -r)/kernel/drivers/usb/
	mkdir -p /lib/modules/$(shell uname -r)/misc/
	cp -f ./ch34x.ko /lib/modules/$(shell uname -r)/misc/
	depmod -a

ifneq ($(KERNELRELEASE),)
obj-m := ch34x.o
else
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

module:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules


	

clean: clean_local
#	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean 

endif

clean_local:
	rm -f *~
	rm -f *.o
	rm -rf ./.tmp_versions
	rm -f ch34x.mod*
	rm -f .ch34x*
	rm -f ch34x.ko


	

