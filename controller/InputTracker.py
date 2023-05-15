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
                    if(event.key == K_A):
                        movement[0] -= 1
                    elif(event.key == K_D):
                        movement[0] += 1
                    elif(event.key == K_W):
                        movement[1] -= 1  
                    elif(event.key == K_S):
                        movement[1] += 1
                    
                    if(self.__allow_action):
                        if(event.key == K_Q):
                            action = 'open'
                        elif(event.key == K_E):
                            action = 'use'
                        elif(event.key == K_F):
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
                        if(event.key == K_Z):
                            action = 'open'
                        if(event.key == K_X):
                            action = 'use'
                        if(event.key == K_C):
                            action = 'close'
        return movement, action