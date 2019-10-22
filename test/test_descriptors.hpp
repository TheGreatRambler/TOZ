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
	/*.iManufacturer = "MyOwnGadget",
	.iProduct = "Custom Gadget",
	.iSerialNumber = "0001",*/
	
	.idVendor = usb_gadget_cpu_to_le16(0x057E), // Nintendo
	.idProduct = usb_gadget_cpu_to_le16(0x2009), // Pro Controller
	.bcdDevice = usb_gadget_cpu_to_le16(0x0200), // BCD Device 4.00
	
	.bNumConfigurations = 1, // Only one configuration
};

static struct usb_interface_descriptor if_descriptor = {
  .bLength = sizeof(if_descriptor);
	.bDescriptorType = USB_DT_INTERFACE;
	.bInterfaceNumber = 0;
	.bAlternateSetting = 0;
	.bNumEndpoints = 2;
	.bInterfaceClass = USB_CLASS_COMM;
	.bInterfaceSubClass = 0;
	.bInterfaceProtocol = 0;
	.iInterface = STRINGID_INTERFACE;
};

static struct usb_config_descriptor config_hs = {
  config_hs.bLength = sizeof(config_hs);
	config_hs.bDescriptorType = USB_DT_CONFIG;
	config_hs.wTotalLength = config_hs.bLength + if_descriptor.bLength + ep_descriptor_in.bLength + ep_descriptor_out.bLength;
	config_hs.bNumInterfaces = 1;
	config_hs.bConfigurationValue = CONFIG_VALUE;
	config_hs.iConfiguration = STRINGID_CONFIG_HS;
	config_hs.bmAttributes = USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER;
	config_hs.bMaxPower = 1;
};

static struct usb_config_descirptor config = {
  config.bLength = sizeof(config);
	config.bDescriptorType = USB_DT_CONFIG;
	config.wTotalLength = config.bLength + if_descriptor.bLength + ep_descriptor_in.bLength + ep_descriptor_out.bLength;
	config.bNumInterfaces = 1;
	config.bConfigurationValue = CONFIG_VALUE;
	config.iConfiguration = STRINGID_CONFIG_LS;
	config.bmAttributes = USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER;
	config.bMaxPower = 1;
};