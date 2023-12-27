#!/usr/bin/python3

from cypherock import Cypherock
from time import sleep



toaster = Cypherock()
toaster.on_off(True)
avg_count = 100
input("Waiting to shoot...")
toaster.software_shoot(80)
input("Waiting to stop...")
toaster.on_off(False)