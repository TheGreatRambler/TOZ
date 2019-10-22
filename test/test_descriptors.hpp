#pragma once

#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadgetfs.h>

#include <stdio.h>

#include "thirdParty/usbgadget.h"

static struct usb_device_descriptor device_descriptor = {
  .bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bDeviceClass = USB_CLASS_COMM,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	// device_descriptor.bMaxPacketSize0 = 255; Set by driver
	.idVendor = 0xAA, // My own id
	.idProduct = 0xBB, // My own id
	.bcdDevice = 0x0200, // Version
	// Strings
	.iManufacturer = STRINGID_MANUFACTURER,
	.iProduct = STRINGID_PRODUCT,
	.iSerialNumber = STRINGID_SERIAL,
	.bNumConfigurations = 1, // Only one configuration
};