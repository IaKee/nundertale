# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')

# system modules
from os import path as ospath
from os import listdir

def is_valid_target(target):
    """Checks if given target path is a valid file or folder"""
    local_path = ospath.join(target)

    if(ospath.exists(local_path)):
        return True
    return False
    
def get_file_names(target):
    """"Returns file names on given target path, else returns None"""
    if(is_valid_target(target)):

        if(ospath.exists(target)):
            local_path = ospath.join(target)
            folder_raw = listdir(local_path)            
            return [file for file in folder_raw if not ospath.isdir(local_path + '\\' + file)]
        else:
            return None

def get_folder_names(target):
    """"Returns folder names on given target path, else returns None"""
    if(is_valid_target(target)):

        if(ospath.exists(target)):
            local_path = ospath.join(target)
            folder_raw = listdir(local_path)            
            return [file for file in folder_raw if ospath.isdir(local_path + '\\' + file)]
        else:
            return None


