# TOZ
Tas On Zero: Run TAS on a Raspi Zero

## How to install
For those that need to install Rasbian
1. Install [Raspbian Lite](https://www.raspberrypi.org/downloads/raspbian/) and flash to a micro SD card (I recommend [Rufus](https://rufus.ie/))
2. Open the Boot partition of the SD card (Simply open it normally on Windows) and create the file `SSH`
3. Create another file named `wpa_supplicant.conf` with the following contents:
```
country=US
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

network={
   ssid="[NAME OF NETWORK HERE]"
   psk="[NAME OF NETWORK PASSWORD HERE]"
}
```
4. Eject the SD card and put it in the PI
5. SSH into the PI using this command to get the IP:
```
ping raspberrypi
```
6. Run some updates with the following commands:
```
sudo apt-get update -y
sudo apt-get upgrade -y
```
7. Get GIT with this command:
```
sudo apt install git
```
8. Get the GIT repository with this command:
```
