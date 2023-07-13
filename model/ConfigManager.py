from json import load as jsonload
from json import dump as jsondump
from os import path as ospath

from log.logger import flog
from CONSTANTS import CONFIG_FILE_PATH, CONFIG_DIR

class ConfigManager():
    def __init__(self):
        None
    
    def load(self):
        """Reads settings from the corresponding json file"""
        if(not ospath.exists(CONFIG_FILE_PATH)):
            # default configs here
            self.set_defaults()
            return

        try:
            with open(file=CONFIG_FILE_PATH, mode="r", encoding="utf-8") as jf:
                json_content = jsonload(jf)

                # all configs are dinamically extracted here
                for key, value in json_content.items():
                    setattr(self, key, value)

        except Exception as e:
            flog("could not load game configs", mtype="err")
            raise Exception(e)
    
    def set_defaults(self):
        """TODO: Default configs should be set here (rather than another file)"""
        self.lang = "en"

    def save_configs(self):
        """TODO: Default configs should be set here (rather than another file)"""
        configs = {}
        configs["lang"] = self.lang

        with open(file=CONFIG_FILE_PATH, mode="w", encoding="utf-8") as jf:
            json_content = jsondump(configs)

    