deviceName=proCon
# Create device
mkdir /sys/kernel/config/usb_gadget/$deviceName
cd /sys/kernel/config/usb_gadget/$deviceName
# Write data about the usb gadget
echo 0x0100 > bcdDevice # Version 1.0.0
echo 0x0200 > bcdUSB # USB 2.0
echo 0x00 > bDeviceClass
echo 0x00 > bDeviceProtocol
echo 0x00 > bDeviceSubClass
echo 0x08 > bMaxPacketSize0
echo 0x0104 > idProduct # Multifunction Composite Gadget
echo 0x1d6b > idVendor # Linux Foundation
# Localization
englishT=0x409
echo "My manufacturer" > strings/$englishT/manufacturer
echo "My virtual keyboard" > strings/$englishT/product
echo "0123456789" > strings/$englishT/serialnumber
