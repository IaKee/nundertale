from json import load

from CONSTANTS import LANG_DIR

class FileData:
    def __init__(self, lang_code):
        self.__lang_code = lang_code  
    
    def set_lang(self, lang_code):
        self.__lang_code = lang_code  
    def get_lang(self):
        return self.__lang_code

    def load_lang_from_json(self):
        """Reads language files from json and stores as local attributes on this class"""

        def load_data(self, data, prefix=''):
            """Recursively extracts data from dictionaries to internal attributes"""
            for key, value in data.items():
                if isinstance(value, dict):
                    # if the found value is a dictionary, recursively calls this method
                    self.load_data(value, prefix=f"{prefix}{key}_")
                else:
                    # if the found value is in fact a simple value, sets the attribute in the class
                    setattr(self, f"{prefix}{key}", value)

        if(not self.__lang_code):
            raise ValueError("invalidlang")

        lang_file_path = LANG_DIR + f"\\{self.__lang_code}.json"
        try:
            with open(file=lang_file_path, mode='r', encoding="utf-8") as jf:
                json_contents = load(jf)

                # recursive calls to iterate through dictionaries
                load_data(json_contents)
        except Exception as e:
            # raises
            raise Exception(e)