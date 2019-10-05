# Enable GadgetFS
sudo modprobe dwc2
sudo modprobe gadgetfs

sudo mkdir /dev/gadget
sudo mount -t gadgetfs gadgetfs /dev/gadget
