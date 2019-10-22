#pragma once

#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadgetfs.h>

#include <stdio.h>

#include "thirdParty/usbgadget.h"

static struct usb_endpoint_descriptor ep_descriptor_in = {
  .bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_IN | 1,
	.bmAttributes = USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize = 512, // HS size
};

static struct usb_endpoint_descriptor ep_descriptor_out = {
  .bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_OUT | 2,
	.bmAttributes = USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize = 512, // HS size
};

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
	/*.iManufacturer = "MyOwnGadget",
	.iProduct = "Custom Gadget",
	.iSerialNumber = "0001",*/
	
	.idVendor = usb_gadget_cpu_to_le16(0x057E), // Nintendo
	.idProduct = usb_gadget_cpu_to_le16(0x2009), // Pro Controller
	.bcdDevice = usb_gadget_cpu_to_le16(0x0200), // BCD Device 4.00
	
	.bNumConfigurations = 1, // Only one configuration
};

static struct usb_interface_descriptor if_descriptor = {
  .bLength = sizeof(if_descriptor),
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 2,
	.bInterfaceClass = USB_CLASS_COMM,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	//.iInterface = STRINGID_INTERFACE,
  .iInterface = 0,
};

static struct usb_config_descriptor config_hs = {
  .bLength = sizeof(config_hs),
	.bDescriptorType = USB_DT_CONFIG,
	.wTotalLength = config_hs.bLength + if_descriptor.bLength + ep_descriptor_in.bLength + ep_descriptor_out.bLength,
	.bNumInterfaces = 1,
	.bConfigurationValue = 2, //CONFIG VALUE
	.iConfiguration = 0,//STRINGID_CONFIG_HS,
	.bmAttributes = USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER,
	.bMaxPower = 1,
};

static struct usb_config_descriptor config = {
  .bLength = sizeof(config),
	.bDescriptorType = USB_DT_CONFIG,
	.wTotalLength = config.bLength + if_descriptor.bLength + ep_descriptor_in.bLength + ep_descriptor_out.bLength,
	.bNumInterfaces = 1,
	.bConfigurationValue = 2,//CONFIG_VALUE,
	.iConfiguration = 0,//STRINGID_CONFIG_LS,
	.bmAttributes = USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER,
	.bMaxPower = 1,
};