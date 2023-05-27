from pygame import KEYDOWN

# movement key constants
from pygame import K_UP, K_LEFT, K_DOWN, K_RIGHT
from pygame import K_w, K_a, K_s, K_d  # alt

# interaction key constants
from pygame import K_z, K_x, K_c
from pygame import K_e, K_q, K_f

# other key constants
from pygame import K_ESCAPE, K_p


class InputTracker():
    def __init__(self):
        self.__alt_layout = False
        self.__allow_action = False

        self.__pressed_keys = []

    def process_keypresses(self, events):
        movement = [0, 0]
        action = None
        for event in events:
            if(event.type == KEYDOWN):
                if(self.__alt_layout):
                    if(event.key == K_a):
                        movement[0] -= 1
                    elif(event.key == K_d):
                        movement[0] += 1
                    elif(event.key == K_w):
                        movement[1] -= 1  
                    elif(event.key == K_s):
                        movement[1] += 1
                    
                    if(self.__allow_action):
                        if(event.key == K_q):
                            action = 'open'
                        elif(event.key == K_e):
                            action = 'use'
                        elif(event.key == K_f):
                            action = 'close'
                else:
                    if(event.key == K_LEFT):
                        movement[0] -= 1
                    if(event.key == K_RIGHT):
                        movement[0] += 1
                    if(event.key == K_UP):
                        movement[1] -= 1  
                    if(event.key == K_DOWN):
                        movement[1] += 1
                        
                    if(self.__allow_action):
                        if(event.key == K_z):
                            action = 'open'
                        if(event.key == K_x):
                            action = 'use'
                        if(event.key == K_c):
                            action = 'close'
        return movement, action