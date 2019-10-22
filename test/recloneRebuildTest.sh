cd ~
rm -r -f TOZ
git clone https://github.com/TheGreatRambler/TOZ.git
cd TOZ
sudo chmod -R 777 *
cd test
gcc test.c -o test -lpthread