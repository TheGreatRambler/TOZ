#!/bin/bash

# Get current folder
programFolder=$PWD
deviceName=proCon

# Create device

mkdir /sys/kernel/config/usb_gadget/$deviceName
cd /sys/kernel/config/usb_gadget/$deviceName

# Write data about the usb gadget

echo 0x0200 > bcdDevice # Version 2.0.0
echo 0x0200 > bcdUSB # USB 2.0
echo 0x00 > bDeviceClass # Device class (?)
echo 0x00 > bDeviceProtocol # Device protocol (?)
echo 0x00 > bDeviceSubClass # Device subclass (?)
echo 0x0040 > bMaxPacketSize0 # 64 bytes
echo 0x2009 > idProduct # Not quite sure
echo 0x057E > idVendor # Nintendo Co., Ltd

# Localization

englishT=0x409
mkdir strings/$englishT
echo "Nintendo Co., Ltd." > strings/$englishT/manufacturer # Manufacturer
echo "Pro Controller" > strings/$englishT/product # Product (Must be Pro Controller)
echo "000000000001" > strings/$englishT/serialnumber # Serial number

# Define functions (hope these are right)

mkdir functions/hid.usb0
echo 1 > functions/hid.usb0/protocol
echo 22 > functions/hid.usb0/report_length # 22-byte reports (Structure of the inputPacket)
echo 1 > functions/hid.usb0/subclass

# Add report descriptor
# Pipes from prgramFolder and converts to binary
cat $programFolder/reportDescriptor.hex | xxd -r -p - > functions/hid.usb0/report_desc

# Might not need it because were sending data to /dev/hidg0
# https://wiki.tizen.org/USB/Linux_USB_Layers/Configfs_Composite_Gadget/Usage_eq._to_g_hid.ko
# Configs

mkdir configs/c.1
mkdir configs/c.1/strings/$englishT
echo 0xA0 > configs/c.1/bmAttributes # Bus Powered, Remote Wakeup
echo 0xFA > configs/c.1/MaxPower # 500 mA
echo "Example configuration" > configs/c.1/strings/$englishT/configuration

# Link function to config

ln -s functions/hid.usb0 configs/c.1

# Enable gadget

ls /sys/class/udc > UDC