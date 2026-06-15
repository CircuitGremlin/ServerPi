#!/bin/sh

DIRNAME=$(dirname "$(realpath $0)")
TEMP_DIR=$(mktemp -d)

DESCRIPTION="Sends ready status to ATtiny over I2C"
AFTER="network.target multi-user.target"
WANTED_BY="multi-user.target"
FILENAME="serverpi-send-ready-status"
VIRTUAL_ENV="$DIRNAME/../pyvenv/bin/python"

# Create $TEMP_DIR/$FILENAME.service
echo "Create $TEMP_DIR/$FILENAME.service"
echo "[Unit]" > $TEMP_DIR/$FILENAME.service
echo "Description=$DESCRIPTION" >> $TEMP_DIR/$FILENAME.service
echo "After=$AFTER" >> $TEMP_DIR/$FILENAME.service
echo "" >> $TEMP_DIR/$FILENAME.service
echo "[Service]" >> $TEMP_DIR/$FILENAME.service
echo "Type=oneshot" >> $TEMP_DIR/$FILENAME.service
echo "User=$USER" >> $TEMP_DIR/$FILENAME.service
echo "Group=$USER" >> $TEMP_DIR/$FILENAME.service
echo "ExecStart=$VIRTUAL_ENV $DIRNAME/$FILENAME.py" >> $TEMP_DIR/$FILENAME.service
echo "WorkingDirectory=$DIRNAME" >> $TEMP_DIR/$FILENAME.service
echo "Restart=on-failure" >> $TEMP_DIR/$FILENAME.service
echo "RestartSec=5" >> $TEMP_DIR/$FILENAME.service
echo "" >> $TEMP_DIR/$FILENAME.service
echo "[Install]" >> $TEMP_DIR/$FILENAME.service
echo "WantedBy=$WANTED_BY" >> $TEMP_DIR/$FILENAME.service

# Copy service file
echo "Copy $TEMP_DIR/$FILENAME.service to /lib/systemd/system/$FILENAME.service"
sudo cp $TEMP_DIR/$FILENAME.service /lib/systemd/system/$FILENAME.service

# Enable service
echo "Enable $FILENAME.service"
sudo systemctl enable $FILENAME.service

# Start service
echo "Start $FILENAME.service"
sudo systemctl restart $FILENAME.service

# Reload deamon
echo "Reload deamon"
sudo systemctl daemon-reload

# Done
echo "Done!"
