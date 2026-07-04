import bluetooth
import pygame
from numpy import interp

import time
from math import sqrt, floor, pi

import movement


def exit():
    if sock is not None:
        sock.close()
    quit()


def normalize(x: float, y: float) -> tuple[int, int]:
    if x > 0.2:
        x = 1
    elif x < -0.2:
        x = -1
    else:
        x = 0
    if y > 0.2:
        y = 1
    elif y < -0.2:
        y = -1
    else:
        y = 0
    return x, y


def deadzone(stick_val: float) -> float:
    if abs(stick_val) > 0.23:
        return stick_val
    return 0


def constrain(x, start: int, end: int, delta: int):
    x += delta
    if x < start:
        return start
    elif x > end:
        return end
    return x


with open("mac.txt", "r") as f:
    esp_mac = f.readline().strip()
sock = None
try:
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    sock.connect((esp_mac, 1))
    time.sleep(2)
except bluetooth.btcommon.BluetoothError as e:
    print(f"Error: {e}")

pygame.init()
pygame.joystick.init()
controller = None
wait_next_press = False
left_speed = 0
right_speed = 0
fork_pos = 150
while True:
    # hotplug the controller (dualshock 4)
    for event in pygame.event.get():
        if event.type == pygame.JOYDEVICEADDED:
            controller = pygame.joystick.Joystick(event.device_index)
        if event.type == pygame.JOYDEVICEREMOVED:
            controller = None
    if controller is not None:
        axes = joystick.get_numaxes(controller)
        for i in range(axes):
            axis = joystick.get_axis(i)
            print(f"{i}: {axis}")
        if controller.get_button(10):
            exit()



    pygame.event.pump()
    time.sleep(0.01)
