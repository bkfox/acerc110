To be able to use the projector, you need to run usb\_modeswitch (you can use
modeswitch script that does that)

	usb_modeswitch --default-vendor 0x1de1 --default-product 0x1101 \
	--target-vendor 0x1de1 --target-product c101 --message-endpoint 0x01 \
	-M 555342430890d8f90000000000000cff020000000000000000000000000000

There is also the missing FindLIBUSB module for cmake that is not especially
included in most distribution. You have to copy it to /usr/share/cmake-2.8/Modules


Also take a look there
======================

https://github.com/rettichschnidi/acerc11xdrv
http://git.ao2.it/libam7xxx.git


