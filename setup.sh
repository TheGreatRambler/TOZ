modprobe dwc2
modprobe gadgetfs
mkdir /dev/gadget
mount -t gadgetfs none /dev/gadget
