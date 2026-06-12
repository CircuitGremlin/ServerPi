# OLED Advanced Stats Display Script For Raspberry Pi

- [Information](#information)
- [Setup your display](#setup-your-display)
- [Installation guide](#installation-guide)
- [Configuration](#configuration)
- [Testing](#testing)
- [Add service OR crontab](#add-service-or-crontab)
- [Contribute](#contribute)
- [Acknowledgment](#acknowledgment)

# Information

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
