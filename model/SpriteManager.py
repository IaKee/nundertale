# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')

# system modules
from os import path as ospath
from os import listdir

# local modules
from CONSTANTS import SPRITE_DIR
from model.utils import is_valid_target

class SpriteManager:
    def __init__(self):
        self.__sprite_lib = {}  # sprites currently open

    def load_sprite(self, target):
        """Loads image files from given target path and stores it on local sprite lib
        Returns raw sprite data

        To acess loaded sprite, use the get_sprite method"""
        
        # checks for valid file name

        target_path = SPRITE_DIR + '\\' + target
        if(is_valid_target(target_path)):
            file_name = target.rsplit('.')[0]
            
            # reads file contents to local variable, then closes file
            with open(target_path, mode='rb') as sprite_file:
                raw_file_data = sprite_file.read()
        
            self.__sprite_lib[file_name] = raw_file_data
            return raw_file_data
        else:
            raise FileNotFoundError(f'could not find file {target}')  
    def unload_sprite(self, target):
        self.__sprite_lib.pop(target)
    def get_sprite_raw(self, target):
        return self.__sprite_lib[target]

def test():
    
    dummy = SpriteManager()
    dummy.load_sprite('player.png')

test()

