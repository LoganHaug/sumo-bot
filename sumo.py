import bluetooth
import pygame

import time

from numpy import interp

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


sock = None
def send_vals(left_speed: int, right_speed: int, ang: int):
    if sock is not None:
        sock.send('#'.encode()
                  + left_speed.to_bytes(1, "little", signed=True) 
                  + '+'.encode()
                  + right_speed.to_bytes(1, "little", signed=True) 
                  + '+'.encode()
                  + ang.to_bytes(1, "big") 
                  + '$'.encode())        

with open("mac.txt", "r") as f:
    esp_mac = f.readline().strip()
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
fork_pos = 100 
# ps4 controller axis mapping: 0 = left stick horiz, 1 = left stick vert, 2 = left trigger, 3 = right stick horiz, 4 = right stick vert, 5 = right trigger
while True:
    # hotplug the controller (dualshock 4)
    for event in pygame.event.get():
        if event.type == pygame.JOYDEVICEADDED:
            controller = pygame.joystick.Joystick(event.device_index)
        if event.type == pygame.JOYDEVICEREMOVED:
            controller = None
    if controller is not None:
        left_speed = int(constrain(interp(-1 * deadzone(controller.get_axis(1)), [-1, 1], [-127, 127]), -127, 127, 0))
        right_speed = int(constrain(interp(-1 * deadzone(controller.get_axis(4)), [-1, 1], [-127, 127]), -127, 127, 0))
        # lower fork
        if controller.get_axis(2) > -0.5 and controller.get_axis(5) < -0.5:
            fork_pos = constrain(fork_pos, 0, 255, -5) 
        elif controller.get_axis(5) > -0.5 and controller.get_axis(2) < -0.5:
            fork_pos = constrain(fork_pos, 0, 255, 5)
        print(f"ls: {left_speed}\trs: {right_speed}\tang: {fork_pos}")
        send_vals(left_speed, right_speed, fork_pos)
        if controller.get_button(10):
            exit()


    pygame.event.pump()
    time.sleep(0.05)
