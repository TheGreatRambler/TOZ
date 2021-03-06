#pragma once

#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadgetfs.h>

#include <stdio.h>

#include "../thirdParty/usbgadget.h"

// HID Descriptor type
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

enum {
	STRING_MANUFACTURER = 1,
	STRING_PRODUCT = 2,
	STRING_SERIAL = 3,
	STRING_CONFIG = 4,
	STRING_INTERFACE = 4,
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
	{
		STRING_CONFIG,
		"The Configuration",
	},
	{
		STRING_INTERFACE,
		"Source/Sink",
	},
};

static struct usb_gadget_strings deviceStrings = {
	.language = 0x0409, /* en-us */
	.strings = procontrollerStrings,
};

static struct usb_device_descriptor procontroller_device_descriptor = {
	//.bLength = sizeof(procontroller_device_descriptor), // 18 bytes
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE, // Defines that this is a device

	.bcdUSB = usb_gadget_cpu_to_le16(0x0200), // Defines that this is USB 2.0
	.bDeviceClass = USB_CLASS_HID, // HID class
	.bDeviceSubClass = 0x00, // Also the default
	.bDeviceProtocol = 0x00, // Suprise, it's the default

	.bMaxPacketSize0 = 0x40, // 64 bytes

	.idVendor = usb_gadget_cpu_to_le16(0x057E), // Nintendo
	.idProduct = usb_gadget_cpu_to_le16(0x2009), // Pro Controller
	.bcdDevice = usb_gadget_cpu_to_le16(0x0200), // BCD Device 4.00

	.iManufacturer = usb_gadget_cpu_to_le16(STRING_MANUFACTURER), // 1 byte
	.iProduct = usb_gadget_cpu_to_le16(STRING_PRODUCT), // 1 byte
	.iSerialNumber = usb_gadget_cpu_to_le16(STRING_SERIAL), // 1 byte
	.bNumConfigurations = 0x01,
};

// printf("%d\n", sizeof(procontroller_device_descriptor));
/*
static struct usb_config_descriptor procontroller_config_descriptor = {
	.bLength = sizeof(procontroller_config_descriptor), // 9 bytes
	.bDescriptorType = USB_DT_CONFIG, // This is a configuration

	.wTotalLength = usb_gadget_cpu_to_le16(0x0029), // 41 bytess
	.bNumInterfaces = 0x01, // One interface
	.bConfigurationValue = 0x01, // One??
	.iConfiguration = 0x00, // I dunno what this does
	.bmAttributes = 0xA0, // Remote Wakeup
	.bMaxPower = 0xFA, // Max power is 500 mA
};

// printf("%d\n", sizeof(procontroller_config_descriptor));

// Need high speed
static struct usb_config_descriptor procontroller_hs_config_descriptor = {
	.bLength = sizeof(procontroller_hs_config_descriptor), // 9 bytes
	.bDescriptorType = USB_DT_CONFIG, // This is a configuration

	.wTotalLength = usb_gadget_cpu_to_le16(0x0029), // 41 bytes
	.bNumInterfaces = 0x01, // One interface
	.bConfigurationValue = 0x01, // One??
	.iConfiguration = 0x00, // I dunno what this does
	.bmAttributes = 0xA0, // Remote Wakeup
	.bMaxPower = 0xFA, // Max power is 500 mA
};
*/
// printf("%d\n", sizeof(procontroller_hs_config_descriptor));

static const struct usb_interface_descriptor procontroller_interface_descriptor = {
	.bLength = sizeof(procontroller_interface_descriptor), // 9 bytes
	.bDescriptorType = USB_DT_INTERFACE, // This is an interface

	//.bInterfaceNumber = 0x00, // Interface number 1
	//.bAlternateSetting = 0x00, // I dunno
	.bNumEndpoints = 0x02, // Two endpoints
	.bInterfaceClass = USB_CLASS_HID, // HID interface
	.bInterfaceSubClass = 0x01,//0x00,
	.bInterfaceProtocol = 0x01,//0x00,
	.iInterface = STRING_INTERFACE,//0x00, // No interface string
};

// printf("%d\n", sizeof(procontroller_interface_descriptor));

static const struct hid_descriptor procontroller_hid_descriptor = {
	.bLength = sizeof(procontroller_hid_descriptor), // 9 bytes
	.bDescriptorType = USB_DT_WIRE_ADAPTER, // Don't really know why

	.bcdHID = usb_gadget_cpu_to_le16(0x0111), // bcdHID 1.11
	.bCountryCode = 0x00, // Don't know
	.bNumDescriptors = 0x01, // I dunno
	.desc = { {
		.bDescriptorType = 0x22, // HID, I guess
		.wDescriptorLength = usb_gadget_cpu_to_le16(0x00CB) // Length of HID report descriptor 203
	} },
};

// printf("%d\n", sizeof(procontroller_hid_descriptor));

static struct usb_endpoint_descriptor procontroller_ep_in_descriptor = {
	.bLength = sizeof(procontroller_ep_in_descriptor), // 7 bytes
	.bDescriptorType = USB_DT_ENDPOINT, // This is an endpoint

	.bEndpointAddress = 0x81, // bEndpointAddress (IN/D2H)
	.bmAttributes = USB_ENDPOINT_XFER_INT, // Interrupt mode
	.wMaxPacketSize = usb_gadget_cpu_to_le16(0x0040), // Max packet size is 64 bytes
	//.bInterval = 0x08, // I think it means 8 bytes per packet, I dunno, depends on device speed
};

// printf("%d\n", sizeof(procontroller_ep_in_descriptor));

static struct usb_endpoint_descriptor procontroller_ep_out_descriptor = {
	.bLength = sizeof(procontroller_ep_out_descriptor), // 7 bytes
	.bDescriptorType = USB_DT_ENDPOINT, // This is an endpoint

	.bEndpointAddress = 0x01, // bEndpointAddress (OUT/H2D)
	.bmAttributes = USB_ENDPOINT_XFER_INT, // Interrupt mode
	.wMaxPacketSize = usb_gadget_cpu_to_le16(0x0040), // Max packet size is 64 bytes
	//.bInterval = 0x08, // I think it means 8 bytes per packet, I dunno, depends on device speed
};

// printf("%d\n", sizeof(procontroller_ep_out_descriptor));