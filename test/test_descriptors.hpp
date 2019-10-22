#pragma once

#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadgetfs.h>

#include <stdio.h>

#include "thirdParty/usbgadget.h"

struct usb_string {
	__u8 id;
	const char* s;
};

struct usb_gadget_strings {
	__u16 language; /* 0x0409 for en-us */
	struct usb_string* strings;
};

enum {
	STRING_MANUFACTURER = 1,
	STRING_PRODUCT = 2,
	STRING_SERIAL = 3,
};

static struct usb_string procontrollerStrings[] = {
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

static struct usb_gadget_strings deviceStrings = {
	.language = 0x0409, /* en-us */
	.strings = procontrollerStrings,
};

static struct usb_device_descriptor device_descriptor = {
  .bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	
	.bcdUSB = usb_gadget_cpu_to_le16(0x200),
	.bDeviceClass = USB_CLASS_HID,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	// device_descriptor.bMaxPacketSize0 = 255; Set by driver
	/*.idVendor = 0xAA, // My own id
	.idProduct = 0xBB, // My own id
	.bcdDevice = 0x0200, // Version*/
	// Strings
	/*.iManufacturer = "MyOwnGadget",
	.iProduct = "Custom Gadget",
	.iSerialNumber = "0001",*/
	
	.bMaxPacketSize0 = 0x40,
	
	.idVendor = usb_gadget_cpu_to_le16(0x057E), // Nintendo
	.idProduct = usb_gadget_cpu_to_le16(0x2009), // Pro Controller
	.bcdDevice = usb_gadget_cpu_to_le16(0x0200), // BCD Device 4.00
	
	.iManufacturer = usb_gadget_cpu_to_le16(STRING_MANUFACTURER), // 1 byte
	.iProduct = usb_gadget_cpu_to_le16(STRING_PRODUCT), // 1 byte
	.iSerialNumber = usb_gadget_cpu_to_le16(STRING_SERIAL), // 1 byte
	
	.bNumConfigurations = 1, // Only one configuration
};

static struct usb_interface_descriptor if_descriptor = {
  .bLength = sizeof(if_descriptor),
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 2,
	.bInterfaceClass = USB_CLASS_HID,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	//.iInterface = STRINGID_INTERFACE,
  .iInterface = 0,
};

static struct usb_endpoint_descriptor ep_descriptor_in = {
  .bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x81,//USB_DIR_IN | 1,
	.bmAttributes = USB_ENDPOINT_XFER_INT,//USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize = usb_gadget_cpu_to_le16(0x0040),//512, // HS size
};

static struct usb_endpoint_descriptor ep_descriptor_out = {
  .bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x01,//USB_DIR_OUT | 2,
	.bmAttributes = USB_ENDPOINT_XFER_INT,//USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize = usb_gadget_cpu_to_le16(0x0040),//512, // HS size
};