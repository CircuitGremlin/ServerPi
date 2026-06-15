Information\
    [Information](#information)
  - [Key features](#key-features)
  - [Case](#case)
  - [PCB](#pcb)
  - [Bill of materials](#bill-of-materials)


- [Installation](#installation)
- [Led configuration](#led-configuration)
- [Oled stats](#oled-stats)
- [Known issues](#known-issues)
- [To do](#to-do)
- [Contribute](#contribute)


# Information
A few years ago, I built a server with a CM4, a CM4IO board and an asm1064 PCI-e card. With the release of the Raspberry Pi 5, I wanted to build a new server. I prefer the Geekworm X1009 to the Radxa Penta because it has five ports and the power connector is on the back.

This server case isn't the smallest, but I'm happy with it so far. It sits on my media unit and is always visible, so it needs to look good.


# Key features
- Supports 5x2.5" HDD up to 15mm on caddy's
- The SD card access is located on the side behind the ventilation grille.
- Custom PCB and software
- 149x194x88mm (width x depth x height(without feet))


## Case
[3D print files](/3d_print_files)<br />
[Plywood panel plans](https://github.com/CircuitGremlin/ServerPi/blob/main/plywood_panel_plans)

![picture alt](https://github.com/CircuitGremlin/ServerPi/blob/main/RPi5_X1009_Case_1.png "RPi5 X1009 Case")
![picture alt](https://github.com/CircuitGremlin/ServerPi/blob/main/RPi5_X1009_Case_2.png "RPi5 X1009 Case")

## PCB
[Schematic](https://github.com/CircuitGremlin/ServerPi/blob/main/pcb_files/Schematic_RPi5_ServerPi_v0.03.pdf)<br />
[Preview](https://github.com/CircuitGremlin/ServerPi/blob/main/pcb_files/PCB_RPi5_ServerPi_v0.03.pdf)<br />
[Files](https://github.com/CircuitGremlin/ServerPi/blob/main/pcb_files)

![picture alt](https://github.com/CircuitGremlin/ServerPi/blob/main/pcb_files/RPi5_ServerPi_v0.03_1.png "RPi5 ServerPi v0.03")
![picture alt](https://github.com/CircuitGremlin/ServerPi/blob/main/pcb_files/RPi5_ServerPi_v0.03_2.png "RPi5 ServerPi v0.03")



## Bill of materials
### Raspberry Pi hardware
Quantity  | Part
:---------|:-----
1 | Raspberry Pi 5
1 | Geekworm X1009 PCIe to 5-Port SATA Shield for Raspberry Pi 5
1 | Geekworm H505 Active Cooler for Raspberry Pi 5
5 | Sata male to female adapter
5 | 10cm Sata cable
1 | Raspberry Pi 5 RTC Batterie
1 | 60x15mm or 60x10mm fan (12V PWM recomended)

### Case hardware
Quantity  | Part | Dimension | Purpose
:---------|:-----|:----------|:-------
45  | Stainless screw | M3x6mm - M3x7mm | Case, PCB, fan
45  | Heatinsert | M3xL3xOD4.5mm | Case, PCB, fan
4   | Standoff | M2.5x6mm | Raspberry Pi mount
4   | Screw | M2.5x6mm - M2.5x8mm | Raspberry Pi mount

### Plywood panels
Quantity  | Part | Dimension
:---------|:-----|:----------
2 | Top/Bottom | 194x149x4mm
2 | Left/Right | 192x80x4mm
1 | Front | 80x48x2mm

### PCB
Quantity  | Part | Value
:---------|:-----|:----------
1 | PCB | JLCPCB
1 | Oled | 1.54"
2 | Button | 12X12X7,3mm 6Pin with led
5 | Led | 5mm
1 | Connector JST XH | 5pin male & female & crimp contacts
1 | Connector JST XH | 9pin male & female & crimp contacts
2 | Pinheader 2.54mm | 4pin
1 | MCU SOIC-14 | ATtiny1614
1 | LDO SOT-23 | MCP1702T-3302ECB
5 | PNP transistor SOT-23 | BC857
1 | NPN transistor SOT-23 | BC847
2 | Resistor 0805 | 30R
5 | Resistor 0805 | 270R
2 | Resistor 0805 | 4K7
1 | Resistor 0805 | 10K
6 | Resistor 0805 | 33K
2 | Capacitor 0805 | 1μF 
1 | Capacitor 0805 | 0.1μF / 100nF
1 | Fuse 0805 | <100mA (optional)
14 | Jumperwire | 15-20cm with female housing
1 | Combo | Male PWM fan connector & cable


# Installation
### Install needed packages and enable I2C interface
```
wget -qO- https://raw.githubusercontent.com/CircuitGremlin/ServerPi/main/install_packages_and_i2c.sh | bash
```
Reboot if necessary
```
sudo reboot
```
### Install repo
```
wget -qO- https://raw.githubusercontent.com/CircuitGremlin/ServerPi/main/install_repo.sh | bash
```
### Install service
```
wget -qO- https://raw.githubusercontent.com/CircuitGremlin/ServerPi/main/install_service.sh | bash
```


# Led configuration
Activate virtual python environment
```
source ~/ServerPi/python/pyvenv/bin/activate
```
Run python led server
```
python ~/ServerPi/python/serverpi-led-server/serverpi-led-server.py
```
Go to http://your-server:3001 and set your led brightness and stop server with ctrl+c.


# Oled stats
I use my own python script: https://github.com/CircuitGremlin/RPiOledStatsLuma


# Known issues
- None yet ;)


# To do
- Setup power button hold function (c++ and python) to reboot Raspberry Pi
- Setup status led (Secondary button) to visualize status, disk health, etc.


# Contribute
Let me know if you found an issue or like to contribute. Thank you!
