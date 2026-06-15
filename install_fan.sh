

# Backup config.txt
echo "Backup config.txt"
sudo cp /boot/firmware/config.txt /boot/firmware/config.txt.bak

# Add lines to '/boot/firmware/config.txt'
echo "Add lines to '/boot/firmware/config.txt"
sudo sh -c 'echo "# Enable case fan" >> /boot/firmware/config.txt'
sudo sh -c 'echo "dtoverlay=pwm-gpio-fan,fan_gpio=27" >> /boot/firmware/config.txt'
sudo sh -c 'echo "" >> /boot/firmware/config.txt'
sudo sh -c 'echo "# Fan temperature settings (CPU and case fan)" >> /boot/firmware/config.txt'
sudo sh -c 'echo "dtparam=fan_temp0=45000" >> /boot/firmware/config.txt'
sudo sh -c 'echo "dtparam=fan_temp1=50000" >> /boot/firmware/config.txt'
sudo sh -c 'echo "dtparam=fan_temp2=55000" >> /boot/firmware/config.txt'
sudo sh -c 'echo "dtparam=fan_temp3=60000" >> /boot/firmware/config.txt'
