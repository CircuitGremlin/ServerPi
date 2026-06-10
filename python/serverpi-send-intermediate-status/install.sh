#!/bin/sh

DIRNAME=$(dirname "$(realpath $0)")
TEMP_DIR=$(mktemp -d)

DESCRIPTION="Sends intermediate status to ATtiny over I2C before reboot, halt or poweroff"
BEFORE="reboot.target halt.target poweroff.target"
WANTED_BY="reboot.target halt.target poweroff.target"
FILENAME="serverpi-send-intermediate-status"
VIRTUAL_ENV="$DIRNAME/../venv/bin/python"

# Create $TEMP_DIR/$FILENAME.service
echo "Create $TEMP_DIR/$FILENAME.service"
echo "[Unit]" > $TEMP_DIR/$FILENAME.service
echo "Description=$DESCRIPTION" >> $TEMP_DIR/$FILENAME.service
echo "Before=$BEFORE" >> $TEMP_DIR/$FILENAME.service
echo "DefaultDependencies=no" >> $TEMP_DIR/$FILENAME.service
echo "" >> $TEMP_DIR/$FILENAME.service
echo "[Service]" >> $TEMP_DIR/$FILENAME.service
echo "Type=oneshot" >> $TEMP_DIR/$FILENAME.service
echo "User=$USER" >> $TEMP_DIR/$FILENAME.service
echo "Group=$USER" >> $TEMP_DIR/$FILENAME.service
echo "ExecStart=$VIRTUAL_ENV $DIRNAME/$FILENAME.py" >> $TEMP_DIR/$FILENAME.service
echo "WorkingDirectory=$DIRNAME" >> $TEMP_DIR/$FILENAME.service
echo "" >> $TEMP_DIR/$FILENAME.service
echo "[Install]" >> $TEMP_DIR/$FILENAME.service
echo "WantedBy=$WANTED_BY" >> $TEMP_DIR/$FILENAME.service

# Copy service file
echo "Copy $TEMP_DIR/$FILENAME.service to /lib/systemd/system/$FILENAME.service"
sudo cp $TEMP_DIR/$FILENAME.service /lib/systemd/system/$FILENAME.service

# Enable service
echo "Enable $FILENAME.service"
sudo systemctl enable $FILENAME.service

# Reload deamon
echo "Reload deamon"
sudo systemctl daemon-reload

# Done
echo "Done!"