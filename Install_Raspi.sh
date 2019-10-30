# Enable EWC2
if ! grep -Fq "dtoverlay=dwc2" /boot/config.txt
then
	# Dtoverlay has not been set
	echo "dtoverlay=dwc2" | sudo tee -a /boot/config.txt
fi

# Add rpi-serial-console
if ! [ -x "$(command -v rpi-serial-console)" ]
then
	# Script to turn the Raspi into a serial device and prevent it from being turned into a terminal
	sudo wget https://raw.githubusercontent.com/lurch/rpi-serial-console/master/rpi-serial-console -O /usr/bin/rpi-serial-console && sudo chmod +x /usr/bin/rpi-serial-console
	# Start now
	sudo rpi-serial-console enable
fi

# Finally, build the program
make

# Done
echo "Installed"
