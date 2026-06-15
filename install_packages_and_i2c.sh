#!/bin/bash

# Update system
echo "Update system"
sudo apt update && sudo apt -y full-upgrade

# Install apt packages
echo "Install apt packages"
sudo apt install -y i2c-tools git

# Enable I2C interface
echo "Enable I2C interface"
sudo raspi-config nonint do_i2c 0

# Done
echo "Done!"
