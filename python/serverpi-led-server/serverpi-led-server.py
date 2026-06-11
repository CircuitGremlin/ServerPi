"""
!/usr/bin/env python
This Python file uses the following encoding: utf-8

GPL-3.0 license

Server to configure a neopixel on TvPi IO-Board
"""

# Standard librarys
import json
import os
import sys
import time
from http.server import BaseHTTPRequestHandler, HTTPServer, SimpleHTTPRequestHandler
from urllib.parse import urlparse, parse_qs
import socket

# Insatlled libraries via pip
from smbus2 import SMBus

# Webserver
PORT = 3001

# I2c
I2C_BUS = 1  # 1
I2C_ADDRESS = 0x08  # 8
I2C_WRITE_REPETITION = 1  # 1

# Default brightness register
I2C_DEFAULT_PWR_LED_MIN_BRT_REG = 0xB0  # 176
I2C_DEFAULT_PWR_LED_MAX_BRT_REG = 0xB1  # 177
I2C_DEFAULT_SATA_LED_BRT_REG = 0xB2  # 178
I2C_DEFAULT_BRT_STRUCT_REG = 0xBF  # 191
# Stored brightness register
I2C_STORED_PWR_LED_MIN_BRT_REG = 0xC0  # 192
I2C_STORED_PWR_LED_MAX_BRT_REG = 0xC1  # 193
I2C_STORED_SATA_LED_BRT_REG = 0xC2  # 194
I2C_STORED_BRT_STRUCT_REG = 0xCF  # 207
# Test brightness register
I2C_TEST_PWR_LED_MIN_BRT_REG = 0xD0  # 192 208
I2C_TEST_PWR_LED_MAX_BRT_REG = 0xD1  # 209
I2C_TEST_SATA_LED_BRT_REG = 0xD2  # 210
I2C_TEST_BRT_STRUCT_REG = 0xDF  # 223


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


def read_data(address, register, length):
    """Write i2c data"""
    for _ in range(10):
        try:
            with SMBus(I2C_BUS) as bus:
                data = bus.read_i2c_block_data(address, register, length)
                # print("RAW data: " + str(data))
                if len(data) == 1:
                    return data
                if len(data) == 3:
                    return [data[2], data[1], data[0]]
        except IOError:
            pass
            return None


class WebRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):

        # Serve '/img/*'
        if self.path.startswith("/img/"):
            with open(os.path.join(sys.path[0], "html" + self.path), "rb") as fh:
                self.send_response(200)
                self.send_header("Content-type", "image/png")
                self.end_headers()
                self.wfile.write(fh.read())

        # Serve '/' or '/index.html'
        if self.path == "/" or self.path == "/index.html":
            with open(os.path.join(sys.path[0], "html/index.html"), "r") as fh:
                self.send_response(200)
                self.send_header("Content-type", "text/html")
                self.end_headers()
                self.wfile.write(fh.read().encode())

        # Serve '/html/style.css'
        if self.path == "/css/style.css":
            with open(os.path.join(sys.path[0], "html/css/style.css"), "r") as fh:
                self.send_response(200)
                self.send_header("Content-type", "text/css")
                self.end_headers()
                self.wfile.write(fh.read().encode())

        # Serve '/json/defaultBrightness.json'
        if self.path.split("?")[0] == "/json/defaultBrightness.json":
            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            defaultBrightness = read_data(
                I2C_ADDRESS, I2C_DEFAULT_BRT_STRUCT_REG, 3)
            # print(defaultBrightness)
            jsonData = json.dumps(defaultBrightness[::-1]).encode()
            # print(jsonData)
            self.wfile.write(jsonData)

        # Serve '/json/storedBrightness.json'
        if self.path.split("?")[0] == "/json/storedBrightness.json":
            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            storedBrightness = read_data(
                I2C_ADDRESS, I2C_STORED_BRT_STRUCT_REG, 3)
            # print(storedBrightness)
            jsonData = json.dumps(storedBrightness[::-1]).encode()
            # print(jsonData)
            self.wfile.write(jsonData)

        # Serve '/json/testBrightness.json'
        if self.path.split("?")[0] == "/json/testBrightness.json":
            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            testBrightness = read_data(
                I2C_ADDRESS, I2C_TEST_BRT_STRUCT_REG, 3)
            # print(testBrightness)
            jsonData = json.dumps(testBrightness[::-1]).encode()
            # print(jsonData)
            self.wfile.write(jsonData)

        # Route to save brightness
        if self.path.split("?")[0] == "/saveBrightness":
            query_components = parse_qs(urlparse(self.path).query)
            # print(query_components)
            brightness = eval(query_components["brightness"][0])
            # print("brightness: " + str(brightness))
            i2c_write_data(
                I2C_ADDRESS,
                I2C_STORED_BRT_STRUCT_REG,
                brightness,
                I2C_WRITE_REPETITION
            )
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.send_header('Custom-Info', 'Using standard send_response')
            self.end_headers()
            self.wfile.write("<h1>Hello, Standard HTTP!</h1>".encode())

        # Route to set brightness
        if self.path.split("?")[0] == "/setTestBrightness":
            query_components = parse_qs(urlparse(self.path).query)
            # print(query_components)
            index = eval(query_components["index"][0])
            # print("index: " + str(index))
            register = None
            if index == 0:
                register = I2C_TEST_PWR_LED_MIN_BRT_REG
            elif index == 1:
                register = I2C_TEST_PWR_LED_MAX_BRT_REG
            elif index == 2:
                register = I2C_TEST_SATA_LED_BRT_REG
            # print("register: " + str(register))
            brightness = eval(query_components["brightness"][0])
            # print("brightness: " + str(brightness))
            i2c_write_data(
                I2C_ADDRESS,
                register,
                brightness,
                I2C_WRITE_REPETITION
            )
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.send_header('Custom-Info', 'Using standard send_response')
            self.end_headers()
            self.wfile.write("<h1>Hello, Standard HTTP!</h1>".encode())


if __name__ == "__main__":
    webServer = HTTPServer(("", PORT), WebRequestHandler)
    try:
        print("Webserver started")
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass
    webServer.server_close()
    print("Server stopped.")
