# Enable GadgetFS
modprobe dwc2
modprobe gadgetfs

mkdir /dev/gadget
mount -t gadgetfs gadgetfs /dev/gadget
