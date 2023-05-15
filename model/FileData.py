from os import stat

class FileData:
    def __init__(self, file_name, raw_data, dimensions=None):
        self.file_name = file_name # Might be useless, need to check what is in file stats
        self.file_stats = stat(file_name)
        self.raw_data = raw_data
        self.dimensions = dimensions # Might be useless, need to check what is in file stats