# algorithm
algorithm study

echo "deb http://cn.archive.ubuntu.com/ubuntu/ vivid main" | sudo tee
/etc/apt/sources.list.d/vivid.list
echo -e "Package: *\nPin: release o=Ubuntu,n=vivid\nPin-Priority: -1" | sudo tee
cat /etc/apt/preferences.d/vivid
sudo apt-get update
sudo apt-get install wpasupplicant=2.1-0ubuntu7
sudo apt-mark hold wpasupplicant
sudo reboot
