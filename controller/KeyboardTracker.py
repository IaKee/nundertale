from kb_constants import KEYBOARD_LITERALS_LIST
from keyboard import is_pressed as check_pressed
from random import choice, random, randrange

class KeyboardTracker():
    """Class used to track keyboard activity for another application, has useful
    tracking functions, such as checking which keys are currently being pressed,
    literals, and randomized number generation based on witch key is currently 
    being pressed"""
    def __init__(self, layout):

        # main list that holds codes of the keys currently being pressed
        self.__currently_pressed = []

        # other keyboard attributes
        self.__layout = layout
        self.__is_pressed = False
        self.__last_key_pressed = False
        self.__first_pressed = None

    def get_update_pressed_keys(self):
        """
        Retrieves and returns keyboard keys currently being pressed,
        key interval changes based on the layout set.

        Saves the updated list on an internal variable.

        Returns:
            list[int]: A list of integers representing the keys that are currently pressed.

        Raises:
            NotImplementedError: If the keyboard layout is not available yet.
        """
        if(self.__layout == 'qwerty'):
            keyboard_range = range(1, 128)
        else:
            raise NotImplementedError('keyboard layout not available yet')

        # resets internal list of currently pressed keys
        self.__currently_pressed.clear()

        pressed_something = False
        for i in keyboard_range:
            if(check_pressed(i)):  # checks if a certain key is pressed
                self.__currently_pressed.append(i)
                pressed_something = True
     
        if(not pressed_something):  # no key was pressed
            self.__first_pressed = None
        else:
            # checks if the first pressed key remains pressed, 
            # else changes it to another random one on the list
            if(not self.__first_pressed in self.__currently_pressed):
                self.__first_pressed = choice(self.__currently_pressed)
        return self.__currently_pressed
    def get_pressed_keys(self):
        """Returns a list of integer key codes for keys that are currently pressed."""
        return self.__currently_pressed
    def get_pressed_literals(self):
        """Returns a list of string literals corresponding to the keyboard keys currently being pressed.
        
        The list is generated by matching the key codes in self.__currently_pressed with the key code to string
        literal mappings in KEYBOARD_LITERALS_LIST. 
        
        If a key code is not found in the mapping, a string containing "INVALID CODE: ",
        followed by the code is added to the list instead.

        Returns:
        literal_list : list of str
            A list of string literals representing the keyboard keys currently being pressed.
        """
        self.__literal_list = []
        for key in self.__currently_pressed:
            for key_info in KEYBOARD_LITERALS_LIST:
                if(key == key_info['key_code']):
                    self.__literal_list.append(key_info['literal'])
                    break
            else:  # if has completed loop without finding a valid key
                self.__literal_list.append(f"INVALID CODE: {key}")
        return self.__literal_list
    def check_combo(self, combo):
        """
        Checks if all keys in the given combo are currently pressed.
        
        Args:
            combo (list): A list of character codes representing the keys in the combo.

        Returns:
            bool: True if all keys in the combo are currently pressed, False otherwise.

        Note:
            The values in the combo list must be key codes, not literals.
        """
        for key in combo:
            if key not in self.__currently_pressed:
                return False
        return True

# eof