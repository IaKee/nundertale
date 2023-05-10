# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')

# system modules
from os import path as ospath
from os import listdir

# other modules
from pygame import image as pygameimage

# local modules
from CONSTANTS import SPRITE_DIR
from utils import *

class SpriteManager:
    def __init__(self):
        self.__sprite_lib = []  # sprites currently open

    def load_sprite(self, target):
        """Loads image files from given target path and stores it on local sprite lib
        Returns sprite handle
        To acess use the get_sprite method"""
        
        # checks for valid file name
        if(is_valid_target(target)):
            file_name = target.rsplit('.')[0]
            raw_file = pygameimage.load(SPRITE_DIR + '\\' + target)             # change this function to view.py
            self.__sprite_lib.append({"name": file_name, "raw": raw_file})
            return raw_file
        else:
            raise FileNotFoundError(f'could not load {target}')  

    def unload_sprite(self, target):
        """Unloads sprites from internal memory"""
        for sprite in self.__sprite_lib:
            if(sprite['name'] == target):
                sprite['raw'].unload()
                self.__sprite_lib.remove(sprite)
        else:
            raise FileNotFoundError(f'could not find {target}')
        
    def get_sprite(self, target):
        """Returns raw image file from internal sprite lib, 
        
        Else raises an exception"""
        
        for sprite in self.__sprite_lib:
            if(sprite['name'+] == target):
                return sprite['raw']
        else:
            raise FileNotFoundError(f'could not find {target}')

def test():
    
    #dummy = SpriteManager()

    print(get_folder_names(SPRITE_DIR))

test()

