# Make every shell file executable
chmod +x update.sh
# Update RasPi Kernel
sudo BRANCH=next rpi-update
# Get some updates
sudo apt-get update -y
sudo apt-get upgrade -y
# Enable USB gadget mode
echo "dtoverlay=dwc2" | sudo tee -a /boot/config.txt
echo "dwc2" | sudo tee -a /etc/modules
# Enable libcomposite driver
sudo echo "libcomposite" | sudo tee -a /etc/modules
# Enable GadgetFS
systemctl enable gadgetfs.service
# TODO: Add addGadget.sh to startup
echo "Installed"
