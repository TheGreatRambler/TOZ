# Enable EWC2
if ! grep -Fq "dtoverlay=dwc2" /boot/config.txt
then
	# Dtoverlay has not been set
	echo "dtoverlay=dwc2" | sudo tee -a /boot/config.txt
fi

# Disable serial console (alternative to UI setup)
raspi-config nonint do_serial 1

# Finally, build the program
make

# Done
echo "Installed"
