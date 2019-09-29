# Enable GadgetFS
modprobe gadgetfs
modprobe libcomposite
mkdir /dev/gadget
mount -t gadgetfs gadgetfs /dev/gadget
