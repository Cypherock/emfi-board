import os
import serial
#from getch import _Getch
import threading
import time

#!/usr/bin/python3
import struct
from typing import Optional, Union
import serial
import serial.tools.list_ports
import os
import numpy
from scipy.interpolate import LinearNDInterpolator


class Cypherock:
    def __init__(self, dev=None, sn=None):
        if dev is None:
            print("Press 'i' for sending data")
            print("Press 'q' to quit")
            dev = input("port: ")
        baud = 9600

            #getch = _Getch()
        self.ser = serial.Serial(dev, baud)
        self._software_limit = None

    def read_voltage_raw(self) -> int:
        cw = [0xAA,0x03]
        self.ser.write(serial.to_bytes(cw))
        self.ser.write(b"\x02")
        return int.from_bytes(self.ser.read(2), "little", signed=False)

    def read_voltage(self) -> float:
        raw = self.read_voltage_raw()
        v = self.to_volt(raw)
        return v

    def on_off(self, enable: bool):
        cw = [0xAA,0x04]
        self.ser.write(serial.to_bytes(cw))
        
        command = b"\x01" + (b"\1" if enable else b"\0")
        self.ser.write(command)
        self.ser.read(1)
        #assert self.ser.read(1) == b"\x01"

    def set_pwm_settings(self, period: int, width: int):
        if period < 1:
            raise ValueError("Invalid PWM period")
        if (width < 0) or (width > period):
            raise ValueError("Invalid PWM width")
        cw = [0xAA,0x06]
        self.ser.write(serial.to_bytes(cw))            
        command = bytearray(b"\x03")
        command += period.to_bytes(2, "little", signed=False)
        command += width.to_bytes(2, "little", signed=False)
        self.ser.write(command)
        self.ser.read(1)

    def software_shoot(self, duration: int):
        assert duration in range(0x10000)
        cw = [0xAA,0x04]
        self.ser.write(serial.to_bytes(cw)) 
        command = bytearray(b"\x04")
        command += duration.to_bytes(2, "little", signed=False)
        self.ser.write(command)
        assert self.ser.read(1) == b"\x04"

    def get_ticks(self) -> int:
        cw = [0xAA,0x03]
        self.ser.write(serial.to_bytes(cw)) 

        self.ser.write(b"\x05")
        return struct.unpack(">Q", self.ser.read(8))[0]

    def get_voltage_setpoint(self) -> float:
        cw = [0xAA,0x03]
        self.ser.write(serial.to_bytes(cw)) 

        self.ser.write(b"\x06")
        destination = struct.unpack(">H", self.ser.read(2))[0]
        return self.to_volt(destination)

    def set_voltage_setpoint(self, destination: float):
        cw = [0xAA,0x06]
        self.ser.write(serial.to_bytes(cw)) 

        command = b"\x07"
        command += struct.pack(">H", self.to_raw(destination))
        self.ser.write(command)

    def get_pwm_settings(self) -> tuple[int, int]:
        cw = [0xAA,0x03]
        self.ser.write(serial.to_bytes(cw)) 
        
        self.ser.write(b"\x08")
        period = int.from_bytes(self.ser.read(2), "little", signed=False)
        width = int.from_bytes(self.ser.read(2), "little", signed=False)
        return period, width

    def set_adc_control_pid(
        self,
        kp: float,
        ki: float,
        kd: float,
        control_ticks: int,
        to_flash=False,
    ):
        cw = [0xAA,24]
        self.ser.write(serial.to_bytes(cw)) 

        command = b"\x0A"
        command += struct.pack(">?3fQ", to_flash, kp, ki, kd, control_ticks)
        self.ser.write(command)

    def set_Adc_setpoint(self, destination: int):
            cw = [0xAA,0x05]
            self.ser.write(serial.to_bytes(cw)) 

            command = b"\x0D"
            command += destination.to_bytes(2, "little", signed=False)
            self.ser.write(command)


