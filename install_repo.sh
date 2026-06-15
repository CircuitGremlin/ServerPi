#!/bin/bash

# Go into home directory
echo "Go into home directory"
cd ~/

# Get repo
echo "Get 'ServerPi' repo"
git clone https://github.com/CircuitGremlin/ServerPi.git

# Open directory
echo "Open 'ServerPi/python' directory"
cd ServerPi/python

# Create virtual python environment
echo "Create virtual python environment"
python3 -m venv --system-site-packages pyvenv

# Activate virtual python environment
echo "Activate virtual python environment"
source ./pyvenv/bin/activate

# Install pip packages
echo "Install pip packages"
pip3 install smbus2

# Done
echo "Done!"
