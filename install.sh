# Update RasPi Kernel
sudo BRANCH=next rpi-update
# Enable USB gadget mode
echo "dtoverlay=dwc2" | sudo tee -a /boot/config.txt
echo "dwc2" | sudo tee -a /etc/modules
# Enable libcomposite driver
sudo echo "libcomposite" | sudo tee -a /etc/modules
# TODO: Add addGadget.sh to startup
