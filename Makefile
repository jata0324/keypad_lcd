INCLUDEDIR :=/root/PXA255-pro/Kernel/linux-2.4.19-pro3_usb20/include
CROSS_COMPILE:=arm-linux-
CFLAGS:= -D__KERNEL__ -DMODULE -Wall -O2 -I$(INCLUDEDIR)
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
all: keypad_device.o fb_test3
keypad_device.o:
	$(CC) $(CFLAGS) -c keypad_device.c -o keypad_device.o
fb_test3: fb_test3.c
	$(CC) -o fb_test3 fb_test3.c

clean :
	rm -f *.o
	rm -f fb_test3
