from os import stat
from io import BytesIO

class FileData:
    def __init__(self, file_path, raw_data, geometry = (0, 0), mode = None):
        self.file_name = file_path.rsplit('\\')[-1]  
        self.file_stats = stat(file_path) # Might be useless, need to check what is in file stats
        self.mode = mode
        self.raw_data = raw_data
        self.geometry = geometry
    
    def get_raw(self):
        return self.raw_data
    def get_mode(self):
        return self.mode
    def get_geometry(self):
        """
            Returns a tuple (width, height)
        """
        return self.geometry