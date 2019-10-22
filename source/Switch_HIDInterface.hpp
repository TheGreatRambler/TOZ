#pragma once

#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadgetfs.h>

#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>

#include "thirdParty/usbstring.h"

// Descriptors
#include "descriptors/Switch_Descriptors.hpp"

// clang-format off
#define FETCH(_var_) \
	memcpy(cp, &_var_, _var_.bLength); \
	cp += _var_.bLength;
// clang-format on

#define CONFIG_VALUE 2

// specific to controller
#define USB_DEV "/dev/gadget/20980000.usb"
#define USB_EPIN "/dev/gadget/ep1in"
#define USB_EPOUT "/dev/gadget/ep2out"

struct io_thread_args {
	unsigned stop;
	int fd_in, fd_out;
};

static struct io_thread_args thread_args;

static struct usb_endpoint_descriptor ep_descriptor_in;
static struct usb_endpoint_descriptor ep_descriptor_out;

/*
 * Respond to host requests
 */
static void* io_thread(void* arg) {
	struct io_thread_args* thread_args = (struct io_thread_args*) arg;
	fd_set read_set, write_set;
	struct timeval timeout;
	int ret, max_read_fd, max_write_fd;
	char buffer[512];

	max_read_fd = max_write_fd = 0;

	if (thread_args->fd_in > max_write_fd)
		max_write_fd = thread_args->fd_in;
	if (thread_args->fd_out > max_read_fd)
		max_read_fd = thread_args->fd_out;

	while (!thread_args->stop) {
		FD_ZERO(&read_set);
		FD_SET(thread_args->fd_out, &read_set);
		timeout.tv_sec = 0;
		timeout.tv_usec = 10000; // 10ms

		memset(buffer, 0, sizeof(buffer));
		ret = select(max_read_fd + 1, &read_set, NULL, NULL, &timeout);

		// Timeout
		if (ret == 0)
			continue;

		// Error
		if (ret < 0)
			break;

		ret = read(thread_args->fd_out, buffer, sizeof(buffer));

		if (ret > 0)
			printf("Read %d bytes : %s\n", ret, buffer);
		else
			printf("Read error %d(%m)\n", ret);

		FD_ZERO(&write_set);
		FD_SET(thread_args->fd_in, &write_set);

		memset(buffer, 0, sizeof(buffer));
		ret = select(max_write_fd + 1, NULL, &write_set, NULL, NULL);

		// Error
		if (ret < 0)
			break;

		strcpy(buffer, "My name is USBond !");

		ret = write(thread_args->fd_in, buffer, strlen(buffer) + 1);

		printf("Write status %d (%m)\n", ret);
	}

	close(thread_args->fd_in);
	close(thread_args->fd_out);

	thread_args->fd_in = -1;
	thread_args->fd_out = -1;

	return NULL;
}

static int init_ep(int* fd_in, int* fd_out) {
	uint8_t init_config[2048];
	uint8_t* cp;
	int ret = -1;
	uint32_t send_size;

	// Configure ep1 (low/full speed + high speed)
	*fd_in = open(USB_EPIN, O_RDWR);

	if (*fd_in <= 0) {
		printf("Unable to open %s (%m)\n", USB_EPIN);
		goto end;
	}

	*(uint32_t*) init_config = 1;
	cp = &init_config[4];

	FETCH(ep_descriptor_in);
	FETCH(ep_descriptor_in);

	send_size = (uint32_t) cp - (uint32_t) init_config;
	ret = write(*fd_in, init_config, send_size);

	if (ret != send_size) {
		printf("Write error %d (%m)\n", ret);
		goto end;
	}

	printf("ep1 configured\n");

	// Configure ep2 (low/full speed + high speed)
	*fd_out = open(USB_EPOUT, O_RDWR);

	if (*fd_out <= 0) {
		printf("Unable to open %s (%m)\n", USB_EPOUT);
		goto end;
	}

	*(uint32_t*) init_config = 1;
	cp = &init_config[4];

	FETCH(ep_descriptor_out);
	FETCH(ep_descriptor_out);

	send_size = (uint32_t) cp - (uint32_t) init_config;
	ret = write(*fd_out, init_config, send_size);

	if (ret != send_size) {
		printf("Write error %d (%m)\n", ret);
		goto end;
	}

	printf("ep2 configured\n");

	ret = 0;

end:
	return ret;
}

static void handle_setup_request(int fd, struct usb_ctrlrequest* setup) {
	int status;
	uint8_t buffer[512];
	pthread_t thread;

	printf("Setup request %d\n", setup->bRequest);

	switch (setup->bRequest) {
		case USB_REQ_GET_DESCRIPTOR:
			if (setup->bRequestType != USB_DIR_IN)
				goto stall;
			switch (setup->wValue >> 8) {
				case USB_DT_STRING:
					printf("Get string id #%d (max length %d)\n", setup->wValue & 0xff, setup->wLength);
					// Procontroller specific strings
					status = usb_gadget_get_string(&deviceStrings, setup->wValue & 0xff, buffer);
					// Error
					if (status < 0) {
						printf("String not found !!\n");
						break;
					} else {
						printf("Found %d bytes\n", status);
					}
					write(fd, buffer, status);
					return;
				default:
					printf("Cannot return descriptor %d\n", (setup->wValue >> 8));
			}
			break;
		case USB_REQ_SET_CONFIGURATION:
			if (setup->bRequestType != USB_DIR_OUT) {
				printf("Bad dir\n");
				goto stall;
			}
			switch (setup->wValue) {
				case CONFIG_VALUE:
					printf("Set config value\n");
					if (!thread_args.stop) {
						thread_args.stop = 1;
						usleep(200000); // Wait for termination
					}
					if (thread_args.fd_in <= 0) {
						status = init_ep(&thread_args.fd_in, &thread_args.fd_out);
					} else
						status = 0;
					if (!status) {
						thread_args.stop = 0;
						pthread_create(&thread, NULL, io_thread, &thread_args);
					}
					break;
				case 0:
					printf("Disable threads\n");
					thread_args.stop = 1;
					break;
				default:
					printf("Unhandled configuration value %d\n", setup->wValue);
					break;
			}
			// Just ACK
			status = read(fd, &status, 0);
			return;
		case USB_REQ_GET_INTERFACE:
			printf("GET_INTERFACE\n");
			buffer[0] = 0;
			write(fd, buffer, 1);
			return;
		case USB_REQ_SET_INTERFACE:
			printf("SET_INTERFACE\n");
			ioctl(thread_args.fd_in, GADGETFS_CLEAR_HALT);
			ioctl(thread_args.fd_out, GADGETFS_CLEAR_HALT);
			// ACK
			status = read(fd, &status, 0);
			return;
	}

stall:
	printf("Stalled\n");
	// Error
	if (setup->bRequestType & USB_DIR_IN)
		read(fd, &status, 0);
	else
		write(fd, &status, 0);
}

static void handle_ep0(int fd) {
	int ret, nevents, i;
	fd_set read_set;
	struct usb_gadgetfs_event events[5];

	while (1) {
		FD_ZERO(&read_set);
		FD_SET(fd, &read_set);

		select(fd + 1, &read_set, NULL, NULL, NULL);

		ret = read(fd, &events, sizeof(events));

		if (ret < 0) {
			printf("Read error %d (%m)\n", ret);
			goto end;
		}

		nevents = ret / sizeof(events[0]);

		printf("%d event(s)\n", nevents);

		for (i = 0; i < nevents; i++) {
			switch (events[i].type) {
				case GADGETFS_CONNECT:
					printf("EP0 CONNECT\n");
					break;
				case GADGETFS_DISCONNECT:
					printf("EP0 DISCONNECT\n");
					break;
				case GADGETFS_SETUP:
					printf("EP0 SETUP\n");
					handle_setup_request(fd, &events[i].u.setup);
					break;
				case GADGETFS_NOP:
				case GADGETFS_SUSPEND:
					break;
			}
		}
	}

end:
	return;
}

bool alreadyMounted() {
	struct stat buffer;
	const std::string name = "/dev/gadget";
	// Check if directory exists
	return (stat(name.c_str(), &buffer) == 0);
	// Maybe this is the reason
	// return false;
}

int StartGadget() {
	// Create gadgetfs in memory
	if (!alreadyMounted()) {
		puts("Mount endpoint");
		system("sudo modprobe dwc2");
		//system("sudo modprobe dummy_hcd");
		system("sudo modprobe gadgetfs");
		system("sudo mkdir /dev/gadget");
		system("sudo mount -t gadgetfs none /dev/gadget");
	}
	int fd = -1, ret, err = -1;
	uint32_t send_size;
	uint8_t init_config[2048];
	uint8_t* cp;

	fd = open(USB_DEV, O_RDWR | O_SYNC);

	if (fd <= 0) {
		printf("Unable to open %s (%m)\n", USB_DEV);
		return 1;
	}

	printf("Start init\n");

	*(uint32_t*) init_config = 0;
	cp = &init_config[4];

	struct usb_config_descriptor config;
	struct usb_config_descriptor config_hs;
	
	
	config.bLength = sizeof(config); // 9 bytes
	config.bDescriptorType = USB_DT_CONFIG; // This is a configuration

	config.wTotalLength = config.bLength + procontroller_interface_descriptor.bLength + procontroller_ep_in_descriptor.bLength + procontroller_ep_out_descriptor.bLength;//usb_gadget_cpu_to_le16(0x0029), // 41 bytess
	config.bNumInterfaces = 0x01; // One interface
	config.bConfigurationValue = 0x01; // One??
	config.iConfiguration = 0x00; // I dunno what this does
	config.bmAttributes = USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER;//0xA0 (Remote Wakeup), ONE and WAKEUP should be here
	config.bMaxPower = 0xFA; // Max power is 500 mA
	
	
	
	config_hs.bLength = sizeof(config_hs); // 9 bytes
	config_hs.bDescriptorType = USB_DT_CONFIG; // This is a configuration

	config_hs.wTotalLength = config_hs.bLength + procontroller_interface_descriptor.bLength + procontroller_ep_in_descriptor.bLength + procontroller_ep_out_descriptor.bLength;//usb_gadget_cpu_to_le16(0x0029), // 41 bytes
	config_hs.bNumInterfaces = 0x01; // One interface
	config_hs.bConfigurationValue = 0x01; // One??
	config_hs.iConfiguration = 0x00; // I dunno what this does
	config_hs.bmAttributes = USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER;//0xA0 (Remote Wakeup), ONE and WAKEUP should be here
	config_hs.bMaxPower = 0xFA; // Max power is 500 mA
	
	// Add all included descriptors
	// First, config
	FETCH(config);//FETCH(procontroller_config_descriptor);
	
	// Interface
	FETCH(procontroller_interface_descriptor);
	// HID descriptor
	//FETCH(procontroller_hid_descriptor)
	// Endpoint in
	FETCH(procontroller_ep_in_descriptor);
	// Endpoint out
	FETCH(procontroller_ep_out_descriptor);

	// Add same thing for highspeed (same config)
	FETCH(config_hs);//FETCH(procontroller_hs_config_descriptor);
	FETCH(procontroller_interface_descriptor);
	//FETCH(procontroller_hid_descriptor)
	FETCH(procontroller_ep_in_descriptor);
	FETCH(procontroller_ep_out_descriptor);

	// Finally, add device descriptor
	FETCH(procontroller_device_descriptor);

	// Configure ep0
	send_size = (uint32_t) cp - (uint32_t) init_config;
	
	ret = write(fd, init_config, send_size);

	printf("%d %d\n", send_size, ret);

	if (ret != send_size) {
		printf("Write error %d (%m)\n", ret);
		printf("Errstr: %s\n", strerror(errno));
		goto end;
	}

	printf("ep0 configured\n");

	handle_ep0(fd);

end:
	if (fd != -1)
		close(fd);

	return err;
}
