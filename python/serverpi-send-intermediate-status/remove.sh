#!/bin/sh

FILENAME="serverpi-send-intermediate-status.service"

# Disable services
echo "Disable $FILENAME"
sudo sudo systemctl disable $FILENAME

# Remove services
echo "Remove $FILENAME from /lib/systemd/system/"
sudo sudo rm /lib/systemd/system/$FILENAME

# Reload deamon
echo "Reload deamon"
sudo sudo systemctl daemon-reload

# Done
echo "Done!"