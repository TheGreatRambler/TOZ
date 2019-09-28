# Enable GadgetFS
modprobe gadgetfs
mkdir /dev/gadget
mount -t gadgetfs gadgetfs /dev/gadget
