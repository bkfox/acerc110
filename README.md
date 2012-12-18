This project is a daemon to run an acer c110 pico-projector on gnu/Linux (should run on other UNIX-like systems). Once compiled, you just need to run the c110d program. To know all options:

	c110d --help

You can give a window id, pan the resulting size, select a frame rate (to avoid CPU overload), show benchmark...


Compilation and installation
============================
You need jam, libusb, and libjpeg-turbo (if you want the JPEG support) to compile the daemon and the library.

To compile, run:

	jam

To compile without jpeg support, run:
	jam disable_jpeg=1

To be able to use the projector, you need to run usb\_modeswitch first:

	usb_modeswitch --default-vendor 0x1de1 --default-product 0x1101 \
	--target-vendor 0x1de1 --target-product c101 --message-endpoint 0x01 \
	-M 555342430890d8f90000000000000cff020000000000000000000000000000

Also take a look there too
==========================

https://github.com/rettichschnidi/acerc11xdrv
http://git.ao2.it/libam7xxx.git


