- [Information](#information)
- [Case](#case)
- [PCB](#pcb)
- [Bill of materials](#bill-of-materials)
- [Installation guide](#installation-guide)
- [Configuration](#configuration)


# Information


## Case
[3D print files](https://github.com/CircuitGremlin/ServerPi/blob/main/3d_print_files)
[Plywood panel plans](https://github.com/CircuitGremlin/ServerPi/blob/main/plywood_panel_plans)

![picture alt](https://github.com/CircuitGremlin/ServerPi/blob/main/RPi5_X1009_Case.png "RPi5 X1009 Case")

## PCB
[Schematic](https://github.com/CircuitGremlin/ServerPi/blob/main/pcb_files/Schematic_RPi5_ServerPi_v0.03.pdf)
[Preview](https://github.com/CircuitGremlin/ServerPi/blob/main/pcb_files/PCB_RPi5_ServerPi_v0.03.pdf)
[Files](https://github.com/CircuitGremlin/ServerPi/blob/main/pcb_files)


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

# Installation guide
## 1. Update system
```
sudo apt update && sudo apt -y full-upgrade
```
## 2. Install needed packages
Install apt packages
```
sudo apt install -y python3 python3-pip i2c-tools git
```
Install pip packages
```
pip3 install pyyaml psutil luma.oled
```
## 3. Enable I2C interface
Enabling I2C on the Raspberry Pi using one simple command
```
sudo raspi-config nonint do_i2c 0
```
Checking that I2C is enabled
```
sudo raspi-config nonint get_i2c
```
This command will return:
- **1** if the port is **disabled**
- **0** if the port is **enabled**

Check the I2C status
```
sudo i2cdetect -y 1
```
You should see output something like this
```
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:                         -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- 3c -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- --
```
Reboot if necessary
```
sudo reboot
```
## 4. Enable Docker memory stats
**(You can skip this step if you are not using Docker)**

Edit `/boot/cmdline.txt`
```
sudo nano /boot/cmdline.txt
```
Add the following options at the beginning of the line
```
systemd.unified_cgroup_hierarchy=0 cgroup_enable=memory cgroup_memory=1 
```
Save and close. Confirm that c-groups are enabled
```
cat /proc/cgroups
```
You should see output like this
```
#subsys_name    hierarchy       num_cgroups     enabled
cpuset  9       15      1
cpu     7       69      1
cpuacct 7       69      1
blkio   8       69      1
memory  11      158     1
devices 3       69      1
freezer 5       16      1
net_cls 2       15      1
perf_event      6       15      1
net_prio        2       15      1
pids    4       76      1
rdma    10      1       1
```
Reboot
```
sudo reboot
```
## 5. Clone git repository
```
git clone https://github.com/blenherr/OledAdvStatsLuma.git
```
