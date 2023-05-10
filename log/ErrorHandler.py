# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')

# general use imports
from tkinter import Tk, messagebox
from datetime import datetime
from os import makedirs
from os import path as ospath

# local modules
from CONSTANTS import CRASH_REPORT_DIR

class ErrorHandler(Tk):
    """Tk window class used to display error messages.
    Needs to be closed manually along with the new app"""
    def __init__(self, *args, **kwargs):
        # initializes super
        Tk.__init__(self, *args, **kwargs)
        
        # internal variables
        self.__version = '0.0'

        self.withdraw()  # turns this dummy window invisible
    def write_log(self, content):
        """Writes given message to a file on the same directory as the main application"""
        # datetime object containing current date and time
        now = datetime.now()

        # formats datetime string to use on file name
        dt_string = now.strftime("%d%m%Y-%H%M%S")
        
        # if crash report folder doesn't exist create it
        if not ospath.exists(CRASH_REPORT_DIR):
            makedirs(CRASH_REPORT_DIR)

        file_name = f'{CRASH_REPORT_DIR}\\crash_report-{dt_string}.txt'
        with open(file_name, 'w') as crash_report:
            crash_report.write(content)

    def display_message(self, title, description, msg_type, *args):
        """Summons a message window and keeps it on top untill it is closed"""
        # keeps the summoned message windows on top until closed
        self.wm_attributes("-topmost", 1) 
    
        # gets the function from messagebox module given its string name
        selected_function = getattr(messagebox, f'show{msg_type}')
        selected_function(title=title, message=description)
    def close(self):
        self.quit()
        self.destroy()

#eof
