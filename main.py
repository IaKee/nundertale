# local imports
from model.SpriteManager import SpriteManager
from view.GameView import GameView
from controller.GameController import GameController

# debugging modules
from log.logger import *
from log.ErrorHandler import *
from sys import exit, exc_info
from traceback import format_exc

def main():
    error_handler = ErrorHandler()

    try:
        # instantiates models
        sprites = SpriteManager()
        #sounds = SoundManager()
        #music = MusicManager()
        #images = ImageManager()

        # instantiates controller
        controller = GameController()

        # instantiates view
        game_view = GameView(sprites)

        # links controller to the view module
        controller.link_view(game_view)
        controller.link_sprite_model(sprites)

        # initializes main game loop
        controller.main_loop()
    
    except Exception as e:
        # retrieves error information
        exc_type, exc_obj, exc_tb = exc_info()
        error_file = str(exc_tb.tb_frame).split(',')[1].replace('\\\\', '\\')
        
        # error message body
        error_description = f'A critical error has ocurred while running the game!\n'\
            f'Error type: {exc_type}\n'\
            f'What happened: {exc_obj}\n'\
            f"Where it happened: {error_file}, at line {exc_tb.tb_lineno}"\
            f"Why it happned: Bad coding xD\n\n"\
            f'Full error traceback:\n {format_exc()}'
        
        # display a pop up message containing error information
        #error_handler.display_message(
        #    title = f'Nundertale - Critical error!', 
        #    description = error_description,
        #    msg_type = 'error')

        # write a crash report file
        error_handler.write_log(error_description)
        
        flog(f'Critical error! Full traceback: {format_exc()}', mtype='error')

if(__name__ == '__main__'):
    main()

# create the game model, viewer and the controller
#model = GameModel()
#view = GameView(model)
#controller = GameController(model)
