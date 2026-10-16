from os import path
from inspect import stack

def flog(*args, **kwargs):
    # color constants
    REDSTART = '\033[41m'
    YELLOWSTART = '\033[43m'
    BLUESTART = '\033[44m'
    COLOREND = '\033[0m'

    good_keys = set(['mtype', 'background'])
    refereced_keys = set(kwargs)
    bad_keys = refereced_keys - good_keys

    if(bad_keys):
        for key in bad_keys:
            raise KeyError(f"unknown option --'{key}'")

    text_color = BLUESTART
    if('background' in kwargs):
        text_color = kwargs['background']
    else:
        if('mtype' in kwargs):
            if(kwargs['mtype'].startswith('er')):
                text_color = REDSTART
            elif(kwargs['mtype'].startswith('in')):
                text_color = BLUESTART
            elif(kwargs['mtype'].startswith('wa')):
                text_color = YELLOWSTART
            else:
                raise ValueError(f'unknown type "{kwargs["type"]}"')
    output = ''
    for word in args:  output += ' ' + word
    try:
        caller_class = stack[1][0].f_locals["self"].__class__.__name__
    except:
        caller_class = ''
    caller_script = stack()[1][1].split('\\')[-1].split('.')[0]
    caller_line = stack()[1][2]
    caller_function = stack()[1][3]

    message = f'{text_color}From {caller_script}(@L{caller_line}).{caller_function}:{COLOREND}{output}'
    print(message)