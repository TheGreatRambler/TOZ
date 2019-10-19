obj-m += dummy_hcd.o

FILE_NAME ?= http://raw.githubusercontent.com/raspberrypi/linux/rpi-$(shell uname -r | cut -d '.' -f 1,2).y/drivers/usb/gadget/udc/dummy_hcd.c

# Build dummy_hcd from source
all:
	# Remove earlier file
	rm dummy_hcd.c
	# Download new one
	wget $(FILE_NAME)
	# Build it as a kernel module
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
