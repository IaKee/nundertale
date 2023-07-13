from json import load

from CONSTANTS import CONFIG_DIR

class ConfigManager():
    def __init__(self, user_configs=None):
        self.__user_configs = user_configs
    
    def load(self):
        """Reads settings from the corresponding json file"""
        if(not self.__user_configs):
            # user cofigs not set
            path = f"{CONFIG_DIR}\\user.json"
        else:
            path = f"{CONFIG_DIR}\\defaults.json"

        with open(file=path, mode='r', encoding='utf-8'):
            #TODO: this