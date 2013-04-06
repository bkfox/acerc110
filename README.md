This project is a daemon to run an acer c110 pico-projector on gnu/Linux (should run on other UNIX-like systems). Once compiled, you just need to run the c110d program. To know all options:

	c110d --help

You can give a window id, pan the resulting size, select a frame rate (to avoid CPU overload), show benchmark...


Compilation and installation
============================
You need xcb, xcb-image, xcb-shm, libjpeg (except if jpeg support is disabled), libusb-1.0 and boost in order to compile:

	autoreconf
	./configure
	make

Because the projector by default is a block device, you must run the script modeswitch.

	./modeswitch

This script requires usb\_modeswitch

Also take a look there too
==========================

https://github.com/rettichschnidi/acerc11xdrv
http://git.ao2.it/libam7xxx.git


