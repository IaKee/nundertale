PROGRAM_DESCRIPTION = "This program tracks user input from a keyboard and"\
    " reports the keystrokes to a remote client over a network connection."\
    " It supports customization of the keyboard layout and the frequency of"\
    " keystroke reporting."
PROGRAM_VERSION = "Keyboard Tracker v0.0"

KEYBOARD_LITERALS_LIST = [
    # 1st row
    {'key_code': 1, 'literal': "ESCAPE"},
    {'key_code': 59, 'literal': "F1"},
    {'key_code': 60, 'literal': "F2"},
    {'key_code': 61, 'literal': "F3"},
    {'key_code': 62, 'literal': "F4"},
    {'key_code': 63, 'literal': "F5"},
    {'key_code': 64, 'literal': "F6"},
    {'key_code': 65, 'literal': "F7"},
    {'key_code': 66, 'literal': "F8"},
    {'key_code': 67, 'literal': "F9"},
    {'key_code': 68, 'literal': "F10"},
    {'key_code': 87, 'literal': "F11"},
    {'key_code': 88, 'literal': "F12"},

    # 2nd row
    
    {'key_code': 41, 'literal': "BACKQUOTE"},
    {'key_code': 2, 'literal': "1"},
    {'key_code': 3, 'literal': "2"},
    {'key_code': 4, 'literal': "3"},
    {'key_code': 5, 'literal': "4"},
    {'key_code': 6, 'literal': "5"},
    {'key_code': 7, 'literal': "6"},
    {'key_code': 8, 'literal': "7"},
    {'key_code': 9, 'literal': "8"},
    {'key_code': 10, 'literal': "9"},
    {'key_code': 11, 'literal': "0"},
    {'key_code': 12, 'literal': "MINUS"},
    {'key_code': 13, 'literal': "EQUALS"},
    {'key_code': 14, 'literal': "BACKSPACE"},

    # 3rd row
    {'key_code': 15, 'literal': "TAB"},
    {'key_code': 16, 'literal': "Q"},
    {'key_code': 17, 'literal': "W"},
    {'key_code': 18, 'literal': "E"},
    {'key_code': 19, 'literal': "R"},
    {'key_code': 20, 'literal': "T"},
    {'key_code': 21, 'literal': "Y"},
    {'key_code': 22, 'literal': "U"},
    {'key_code': 23, 'literal': "I"},
    {'key_code': 24, 'literal': "O"},
    {'key_code': 25, 'literal': "P"},
    {'key_code': 26, 'literal': "LBRACKET"},
    {'key_code': 27, 'literal': "RBRACKET"},
    {'key_code': 43, 'literal': "BACKSLASH"},

    # 4th row
    {'key_code': 58, 'literal': "CAPSLOCK"},
    {'key_code': 30, 'literal': "A"},
    {'key_code': 31, 'literal': "S"},
    {'key_code': 32, 'literal': "D"},
    {'key_code': 33, 'literal': "F"},
    {'key_code': 34, 'literal': "G"},
    {'key_code': 35, 'literal': "H"},
    {'key_code': 36, 'literal': "J"},
    {'key_code': 37, 'literal': "K"},
    {'key_code': 38, 'literal': "L"},
    {'key_code': 39, 'literal': "SEMICOLON"},
    {'key_code': 40, 'literal': "APOSTROPHE"},
    {'key_code': 28, 'literal': "ENTER"},

    # 5th row
    {'key_code': 42, 'literal': "LSHIFT"},
    {'key_code': 44, 'literal': "Z"},
    {'key_code': 45, 'literal': "X"},
    {'key_code': 46, 'literal': "C"},                     
    {'key_code': 47, 'literal': "V"},
    {'key_code': 48, 'literal': "B"},
    {'key_code': 49, 'literal': "N"},
    {'key_code': 50, 'literal': "M"},
    {'key_code': 51, 'literal': "COMMA"},
    {'key_code': 52, 'literal': "DOT"},
    {'key_code': 53, 'literal': "FSLASH"},
    {'key_code': 54, 'literal': "RSHIFT"},
    
    # 6th row
    {'key_code': 29, 'literal': "CONTROL"},
    {'key_code': 91, 'literal': "WINDOWS"},
    {'key_code': 56, 'literal': "LALT"},
    {'key_code': 57, 'literal': "SPACE"},                     
    {'key_code': 93, 'literal': "MENU"},
    
    # middle
    {'key_code': 70, 'literal': "SCROLLLOCK"},
    {'key_code': 69, 'literal': "PAUSE"},
    {'key_code': 82, 'literal': "INSERT"},
    {'key_code': 83, 'literal': "DELETE"},

    # numpad
    {'key_code': 55, 'literal': "ASTERISK"},
    {'key_code': 74, 'literal': "NUMMINUS"},
    {'key_code': 71, 'literal': "NUM7"},
    {'key_code': 72, 'literal': "NUM8"},
    {'key_code': 73, 'literal': "NUM9"},
    {'key_code': 78, 'literal': "NUMPLUS"},
    {'key_code': 75, 'literal': "NUM4"},
    {'key_code': 76, 'literal': "NUM5"},
    {'key_code': 77, 'literal': "NUM6"},
    {'key_code': 79, 'literal': "NUM1"},
    {'key_code': 80, 'literal': "NUM2"},
    {'key_code': 81, 'literal': "NUM3"},
    {'key_code': 82, 'literal': "NUM0"},
    {'key_code': 83, 'literal': "NUMDOT"}]