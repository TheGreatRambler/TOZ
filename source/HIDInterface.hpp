#pragma once

#include <fcntl.h>
#include <linux/usb/ch9.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* /dev/gadget/ep* doesn't support poll, we have to use an alternative
   approach. */
#include <pthread.h>

extern "C" {
// Needed to include C header files
#include "thirdParty/GadgetFSApi/usb-gadget.h"
}

#include "helpers.hpp"

#define STRING_MANUFACTURER 25
#define STRING_PRODUCT 45
#define STRING_SERIAL 101
#define STRING_LOOPBACK 250

// This is a really importand constant, but it isn't included here for some reason
#define USB_REQ_GET_REPORT 0x01

struct hid_class_descriptor {
	__u8 bDescriptorType;
	__le16 wDescriptorLength;
} __attribute__((packed));

// Class used to define the HID information (descriptor)
// It isn't built into GadgetFS
struct hid_descriptor {
	__u8 bLength;
	__u8 bDescriptorType;
	__le16 bcdHID;
	__u8 bCountryCode;
	__u8 bNumDescriptors;

	struct hid_class_descriptor desc[1];
} __attribute__((packed));

// Don't know how to include this yet
char procontrollerHIDReportDescriptor[] = {
	0x05, 0x01, // Usage Page (Generic Desktop Ctrls)
	0x15, 0x00, // Logical Minimum (0)
	0x09, 0x04, // Usage (Joystick)
	0xA1, 0x01, // Collection (Application)
	0x85, 0x30, //   Report ID (48)
	0x05, 0x01, //   Usage Page (Generic Desktop Ctrls)
	0x05, 0x09, //   Usage Page (Button)
	0x19, 0x01, //   Usage Minimum (0x01)
	0x29, 0x0A, //   Usage Maximum (0x0A)
	0x15, 0x00, //   Logical Minimum (0)
	0x25, 0x01, //   Logical Maximum (1)
	0x75, 0x01, //   Report Size (1)
	0x95, 0x0A, //   Report Count (10)
	0x55, 0x00, //   Unit Exponent (0)
	0x65, 0x00, //   Unit (None)
	0x81, 0x02, //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x09, //   Usage Page (Button)
	0x19, 0x0B, //   Usage Minimum (0x0B)
	0x29, 0x0E, //   Usage Maximum (0x0E)
	0x15, 0x00, //   Logical Minimum (0)
	0x25, 0x01, //   Logical Maximum (1)
	0x75, 0x01, //   Report Size (1)
	0x95, 0x04, //   Report Count (4)
	0x81, 0x02, //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x75, 0x01, //   Report Size (1)
	0x95, 0x02, //   Report Count (2)
	0x81, 0x03, //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x0B, 0x01, 0x00, 0x01, 0x00, //   Usage (0x010001)
	0xA1, 0x00, //   Collection (Physical)
	0x0B, 0x30, 0x00, 0x01, 0x00, //     Usage (0x010030)
	0x0B, 0x31, 0x00, 0x01, 0x00, //     Usage (0x010031)
	0x0B, 0x32, 0x00, 0x01, 0x00, //     Usage (0x010032)
	0x0B, 0x35, 0x00, 0x01, 0x00, //     Usage (0x010035)
	0x15, 0x00, //     Logical Minimum (0)
	0x27, 0xFF, 0xFF, 0x00, 0x00, //     Logical Maximum (65534)
	0x75, 0x10, //     Report Size (16)
	0x95, 0x04, //     Report Count (4)
	0x81, 0x02, //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0, //   End Collection
	0x0B, 0x39, 0x00, 0x01, 0x00, //   Usage (0x010039)
	0x15, 0x00, //   Logical Minimum (0)
	0x25, 0x07, //   Logical Maximum (7)
	0x35, 0x00, //   Physical Minimum (0)
	0x46, 0x3B, 0x01, //   Physical Maximum (315)
	0x65, 0x14, //   Unit (System: English Rotation, Length: Centimeter)
	0x75, 0x04, //   Report Size (4)
	0x95, 0x01, //   Report Count (1)
	0x81, 0x02, //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x09, //   Usage Page (Button)
	0x19, 0x0F, //   Usage Minimum (0x0F)
	0x29, 0x12, //   Usage Maximum (0x12)
	0x15, 0x00, //   Logical Minimum (0)
	0x25, 0x01, //   Logical Maximum (1)
	0x75, 0x01, //   Report Size (1)
	0x95, 0x04, //   Report Count (4)
	0x81, 0x02, //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x75, 0x08, //   Report Size (8)
	0x95, 0x34, //   Report Count (52)
	0x81, 0x03, //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x06, 0x00, 0xFF, //   Usage Page (Vendor Defined 0xFF00)
	0x85, 0x21, //   Report ID (33)
	0x09, 0x01, //   Usage (0x01)
	0x75, 0x08, //   Report Size (8)
	0x95, 0x3F, //   Report Count (63)
	0x81, 0x03, //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x81, //   Report ID (-127)
	0x09, 0x02, //   Usage (0x02)
	0x75, 0x08, //   Report Size (8)
	0x95, 0x3F, //   Report Count (63)
	0x81, 0x03, //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x01, //   Report ID (1)
	0x09, 0x03, //   Usage (0x03)
	0x75, 0x08, //   Report Size (8)
	0x95, 0x3F, //   Report Count (63)
	0x91, 0x83, //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Volatile)
	0x85, 0x10, //   Report ID (16)
	0x09, 0x04, //   Usage (0x04)
	0x75, 0x08, //   Report Size (8)
	0x95, 0x3F, //   Report Count (63)
	0x91, 0x83, //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Volatile)
	0x85, 0x80, //   Report ID (-128)
	0x09, 0x05, //   Usage (0x05)
	0x75, 0x08, //   Report Size (8)
	0x95, 0x3F, //   Report Count (63)
	0x91, 0x83, //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Volatile)
	0x85, 0x82, //   Report ID (-126)
	0x09, 0x06, //   Usage (0x06)
	0x75, 0x08, //   Report Size (8)
	0x95, 0x3F, //   Report Count (63)
	0x91, 0x83, //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Volatile)
	0xC0, // End Collection
};

static struct usb_gadget_string strings[] = {
	{
		STRING_MANUFACTURER,
		"Nintendo Co., Ltd",
	},
	{
		STRING_PRODUCT,
		"Pro Controller",
	},
	{
		STRING_SERIAL,
		"000000000001",
	},
};

static struct usb_gadget_strings procontroller_strings = {
	.language = 0x0409, /* en-us */
	.strings = strings,
};

static struct usb_device_descriptor procontroller_device_descriptor = {
	.bLength = sizeof(procontroller_device_descriptor),
	.bDescriptorType = USB_DT_DEVICE, // Defines that this is a device

	.bcdUSB = usb_gadget_cpu_to_le16(0x0200), // Defines that this is USB 2.0
	.bDeviceClass = 0x00, //  The default
	.bDeviceSubClass = 0x00, // Also the default
	.bDeviceProtocol = 0x00, // Suprise, it's the default

	.bMaxPacketSize0 = 0x40, // 64 bytes

	.idVendor = usb_gadget_cpu_to_le16(0x057E), // Nintendo
	.idProduct = usb_gadget_cpu_to_le16(0x2009), // Pro Controller
	.bcdDevice = usb_gadget_cpu_to_le16(0x0200), // BCD Device 4.00

	.iManufacturer = usb_gadget_cpu_to_le16(STRING_MANUFACTURER),
	.iProduct = usb_gadget_cpu_to_le16(STRING_PRODUCT),
	.iSerialNumber = usb_gadget_cpu_to_le16(STRING_SERIAL),
	.bNumConfigurations = 0x01,
};

static struct usb_config_descriptor procontroller_config_descriptor = {
	.bLength = sizeof(procontroller_config_descriptor),
	.bDescriptorType = USB_DT_CONFIG, // This is a configuration

	.bNumInterfaces = 0x01, // One interface
	.bConfigurationValue = 0x01, // One??
	.iConfiguration = 0x00, // I dunno what this does
	.bmAttributes = 0xA0, // Remote Wakeup
	.bMaxPower = 0xFA, // Max power is 500 mA
};

static const struct usb_interface_descriptor procontroller_interface_descriptor = {
	.bLength = sizeof(procontroller_interface_descriptor),
	.bDescriptorType = USB_DT_INTERFACE, // This is an interface

	.bInterfaceNumber = 0x00, // Interface number 1
	.bAlternateSetting = 0x00, // I dunno

	.bNumEndpoints = 0x02, // Two endpoints
	.bInterfaceClass = USB_CLASS_HID, // HID interface
	.iInterface = 0x00, // No interface string
};

static const struct hid_descriptor procontroller_hid_descriptor = {
	.bLength = sizeof(procontroller_hid_descriptor),
	.bDescriptorType = USB_DT_WIRE_ADAPTER, // Don't really know why
	.bcdHID = usb_gadget_cpu_to_le16(0x0111), // bcdHID 1.11
	.bCountryCode = 0x00, // Don't know
	.bNumDescriptors = 0x01, // I dunno
	.desc = { {
		.bDescriptorType = 0x22, // HID, I guess
		.wDescriptorLength = usb_gadget_cpu_to_le16(0x00CB) // Length of HID report descriptor 203
	} },
};

static struct usb_endpoint_descriptor procontroller_ep_in_descriptor = {
	.bLength = sizeof(procontroller_ep_in_descriptor), // Size of endpoint
	.bDescriptorType = USB_DT_ENDPOINT, // This is an endpoint

	.bEndpointAddress = 0x81, // bEndpointAddress (IN/D2H)
	.bmAttributes = USB_ENDPOINT_XFER_INT, // Interrupt mode
	.wMaxPacketSize = usb_gadget_cpu_to_le16(64), // Max packet size is 64 bytes
	.bInterval = 0x08, // I think it means 8 bytes per packet, I dunno, depends on device speed
};

static struct usb_endpoint_descriptor procontroller_ep_out_descriptor = {
	.bLength = sizeof(procontroller_ep_out_descriptor), // Size of endpoint
	.bDescriptorType = USB_DT_ENDPOINT, // This is an endpoint

	.bEndpointAddress = 0x01, // bEndpointAddress (OUT/H2D)
	.bmAttributes = USB_ENDPOINT_XFER_INT, // Interrupt mode
	.wMaxPacketSize = usb_gadget_cpu_to_le16(64), // Max packet size is 64 bytes
	.bInterval = 0x08, // I think it means 8 bytes per packet, I dunno, depends on device speed
};

static struct usb_descriptor_header* procontroller_config[] = {
	(struct usb_descriptor_header*) &procontroller_config_descriptor,
	(struct usb_descriptor_header*) &procontroller_interface_descriptor,
	(struct usb_descriptor_header*) &procontroller_hid_descriptor,
	(struct usb_descriptor_header*) &procontroller_ep_in_descriptor,
	(struct usb_descriptor_header*) &procontroller_ep_out_descriptor,
	NULL,
};

// High speed is not supported

static struct usb_gadget_endpoint *procontroller_ep_in, *procontroller_ep_out;
static pthread_t loopback_thread;

static void procontroller_stop_endpoints(void* data) {
	usb_gadget_endpoint_close(procontroller_ep_in);
	usb_gadget_endpoint_close(procontroller_ep_out);
}

static void* data_read_loop(void* data) {
	// Don't know why it is a void pointer
	// 64 is wMaxPacketSize, the max acceptable packet size
	char buf[64];
	int ret;

	// Stop endpoints later
	pthread_cleanup_push(procontroller_stop_endpoints, NULL);
	while (1) {
		int i;

		// This is where the thread will cancel if need be
		pthread_testcancel();

		ret = usb_gadget_endpoint_read(procontroller_ep_out, buf, 64, 100);
		if (ret < 0) {
			perror("usb_gadget_endpoint_read");
			break;
		}

		// `buf` now represents our data and `ret` is the size of the data
		printCharArray(buf, ret);

		/* No need to write right now
		if (usb_gadget_endpoint_write(procontroller_ep_in, buf, ret, 100) < 0) {
			perror("usb_gadget_endpoint_write");
			break;
		}
		*/
	}
	pthread_cleanup_pop(1);
}


static void procontroller_event_cb(usb_gadget_dev_handle* handle, struct usb_gadget_event* event, void* arg) {
	switch (event->type) {
		case USG_EVENT_ENDPOINT_ENABLE:
			// Endpoints have started
			if (event->u.number == (procontroller_ep_in_descriptor.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK))
				procontroller_ep_in = usb_gadget_endpoint(handle, event->u.number);
			else if (event->u.number == (procontroller_ep_out_descriptor.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK))
				procontroller_ep_out = usb_gadget_endpoint(handle, event->u.number);

			if (!procontroller_ep_in || !procontroller_ep_out)
				return;

			if (pthread_create(&loopback_thread, 0, data_read_loop, NULL) != 0)
				perror("pthread_create");
			break;
		case USG_EVENT_ENDPOINT_DISABLE:
			if (event->u.number == (procontroller_ep_in_descriptor.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK))
				procontroller_ep_in = NULL;
			else if (event->u.number == (procontroller_ep_out_descriptor.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK))
				procontroller_ep_out = NULL;
		case USG_EVENT_DISCONNECT: // FALLTHROUGH
			if (loopback_thread)
				pthread_cancel(loopback_thread);
			break;
		default:
			return;
	}
}

bool alreadyMounted() {
	struct stat buffer;
	const std::string name = "/dev/gadget";   
	return (stat (name.c_str(), &buffer) == 0); 
}

void StartGadget() {

	// Create gadgetfs in memory
	if(!alreadyMounted())) {
		system("sudo modprobe dwc2");
		system("sudo modprobe gadgetfs");
		system("sudo mkdir /dev/gadget");
		system("sudo mount -t gadgetfs gadgetfs /dev/gadget");
	}

	struct usb_gadget_device device = {
		.device = &procontroller_device_descriptor,
		.config = procontroller_config,
		//.hs_config = loopback_hs_config,
		.strings = &procontroller_strings,
		// HID report descriptor
		.HIDreport = procontrollerHIDReportDescriptor,
		.HIDreportSize = 203,
	};

	struct pollfd fds;
	usb_gadget_dev_handle* handle;
	struct usb_gadget_endpoint* ep0;
	int debug_level = 1;

	// Open device
	handle = usb_gadget_open(&device);
	if (!handle) {
		fprintf(stderr, "Couldn't open device.\n");
		return;
	}
	// Debug everything
	usb_gadget_set_debug_level(handle, debug_level);
	// Get first endpoint
	ep0 = usb_gadget_endpoint(handle, 0);

	usb_gadget_set_event_cb(handle, procontroller_event_cb, NULL);
	fds.fd = usb_gadget_control_fd(handle);
	fds.events = POLLIN;
	while (1) {
		if (poll(&fds, 1, -1) < 0) {
			perror("poll");
			break;
		}
		if (fds.revents & POLLIN)
			usb_gadget_handle_control_event(handle);
	}

	// Close device
	usb_gadget_close(handle);
}
