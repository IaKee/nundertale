# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')

# system modules
from os import path as ospath
from os import listdir

# local modules
from CONSTANTS import SPRITE_DIR
from log.logger import flog
from model.utils import is_valid_target
from model.FileData import FileData

class FileManager:
    def __init__(self):
        self.__file_lib = {}  # files currently open

    def load_file(self, target, log=False):
        """Loads image files from given target path and stores it on local sprite lib
        Returns raw sprite data

        To acess loaded sprite, use the get_sprite method"""
        
        # checks for valid file name

        target_path = SPRITE_DIR + '\\' + target
        if(is_valid_target(target_path)):
            file_name = target.rsplit('.')[0]
            
            # reads file contents to local variable, then closes file
            with open(target_path, mode='rb') as rfile:
                raw_file_data = rfile.read()
                self.__file_lib[file_name] = FileData(file_name, raw_file_data)
                
                if(log):
                    flog(f"sprite {file_name} loaded!")
            return raw_file_data  # optional
        else:
            raise FileNotFoundError(f'could not find file {target}')  
    def unload(self, target):
        self.__file_lib.pop(target)
    def get_file_raw(self, target):
        return self.__file_lib[target]

