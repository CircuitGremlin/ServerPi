#!/usr/bin/python
"""
!/usr/bin/env python
This Python file uses the following encoding: utf-8
GPL-3.0 license

Write ready byte to ATtiny
"""

# Standard librarys
import os
from datetime import datetime
import logging

# Insatlled libraries via pip
from smbus2 import SMBus

# I2c
I2C_BUS = 1  # 1
I2C_ADDRESS = 0x08  # 8
I2C_WRITE_REPETITION = 5  # 5
# Runlevel register
I2C_RUNLEVEL_REG = 0xA0  # 160
# Runlevel's
I2C_RUNLEVEL_OFF = 0x00  # 0
I2C_RUNLEVEL_INTER = 0x01  # 1
I2C_RUNLEVEL_READY = 0x02  # 2

# Sleep time in seconds (Sleep this script for the specified time)
SLEEP_TIME = 5

# Logging
DIR_PATH = os.path.dirname(os.path.realpath(__file__))
FILE_NAME = os.path.basename(__file__)
os.makedirs(DIR_PATH + "/logs", exist_ok=True)
now = datetime.now()
logging.basicConfig(
    format="%(asctime)s %(levelname)-8s %(lineno)-4d %(funcName)s(): %(message)s",
    filename=DIR_PATH + "/logs/%s.log" % (now.strftime("%Y_%m_%d")),
    datefmt="%H:%M:%S",
    level=logging.ERROR,
    # level=logging.DEBUG,
)
LOGGER = logging.getLogger(__name__)
LOGGER.debug("")
LOGGER.debug("Script has started")


def is_array_like(obj):
    """Check for array like value"""
    return hasattr(obj, "__len__")


def i2c_write_data(address, register, data, repetition):
    """Write i2c data"""
    for _ in range(10):
        try:
            with SMBus(I2C_BUS) as bus:
                for _ in range(repetition):
                    if is_array_like(data):
                        bus.write_block_data(address, register, data)
                    else:
                        bus.write_byte_data(address, register, data)
            return True
        except IOError:
            pass
            return None


def main():
    """Main programm"""
    # RPi is in ready state
    i2c_write_data(
        I2C_ADDRESS,
        I2C_RUNLEVEL_REG,
        I2C_RUNLEVEL_READY,
        I2C_WRITE_REPETITION
    )
    LOGGER.debug("RPi is in ready state. Write i2c data.")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        LOGGER.error(e)
