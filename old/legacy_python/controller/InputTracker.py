from pygame import KEYDOWN
from pygame import key

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
                    if(self.__allow_action):
                        if(event.key == K_q):
                            action = 'open'
                        elif(event.key == K_e):
                            action = 'use'
                        elif(event.key == K_f):
                            action = 'close'
                        else:
                            action = None
                else:
                
                    if(self.__allow_action):
                        if(event.key == K_z):
                            action = 'open'
                        elif(event.key == K_x):
                            action = 'use'
                        elif(event.key == K_c):
                            action = 'close'
                        else:
                            action = None
        
        presses = key.get_pressed()
        if(presses[K_a]):
            movement[0] = -1
        elif(presses[K_LEFT]):
            movement[0] = -1
        if(presses[K_d]):
            movement[0] = 1
        elif(presses[K_RIGHT]):
            movement[0] = 1
        if(presses[K_w]):
            movement[1] = -1  
        elif(presses[K_UP]):
            movement[1] = -1  
        if(presses[K_s]):
            movement[1] = 1
        elif(presses[K_DOWN]):
            movement[1] = 1
       
        return movement, action