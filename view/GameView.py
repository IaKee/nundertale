# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')


# locals
from os import environ
environ['PYGAME_HIDE_SUPPORT_PROMPT'] = '1'
from time import perf_counter
from collections import deque

import pygame
from pygame.locals import DOUBLEBUF, FULLSCREEN

from CONSTANTS import ASSETS_DIR, SCREEN_WIDTH, SCREEN_HEIGHT, FONTS_DIR

class GameView:
    def __init__(self):
        self.screen = None  # pygame.Surface object
        self.frame_rate = 60

        self.fade_solid = None
        self.fade_current_color = [0, 0, 0, 0]  # RGBA

        self.loaded_sprites = []

        self.canvas: list[tuple[pygame.surface.Surface, tuple[int, int]]] = list()
        
        self.curr_alpha = 255

        self.frame: pygame.surface.Surface = None

        # timing control
        self.fps_cap = 60
        self.fps_clock = pygame.time.Clock()

        # fps tracking
        self.__frame_sample_size = 1000
        self.__tracking_frames = True
        self.__recent_frames = deque(maxlen=self.__frame_sample_size)
        self.__has_fps = False
        self.__last_update = 0
        self.__update_delta = 0.5  # time between fps updates

    def start(self): 
        """Initializes pygame and the main game window itself"""
        pygame.init()
        self.screen = pygame.display.set_mode(size=(SCREEN_WIDTH, SCREEN_HEIGHT), depth=16)
        self.frame = pygame.surface.Surface(self.screen.get_size())
        self.fade_solid = pygame.Surface(self.screen.get_size())

        # set game clock
        self.clock = pygame.time.Clock()
        
        # limits frames per second to set rate
        self.clock.tick(self.frame_rate)
        self.default_font = pygame.font.Font(FONTS_DIR + '8bitoperator.ttf', 25)  # default font
      
    def set_title(self, title):
        pygame.display.set_caption(title)
    
    def set_icon(self, icon):
        pygame.display.set_icon(icon)
    
    def load_font(self, name="arial", size=12):
        """
        Loads a font and stores it in an internal class variable if it's not already loaded.
        
        The font is identified and stored using the following layout: self.__font_name_size
        
        Parameters:
            name (str): The name of the font file to load (default is "arial").
            size (int): The size of the font to load (default is 12).
        
        Returns:
            pygame.font.Font: The loaded font object.
        """
        attr = f"font_{name}_{size}"
        loaded_font = getattr(self, attr, None)

        if not loaded_font:
            # If the font is not already loaded, create a new font object and store it in the class attribute
            # TODO: look for default system fonts
            new_font = pygame.font.Font(FONTS_DIR + name, size) 
            setattr(self, attr, new_font)
            return new_font
        else:
            # If the font is already loaded, return the existing font object
            return loaded_font

    def load_sprite(self, sprite_raw, dimensions):
        self.loaded_sprites.append(pygame.image.fromstring(sprite_raw, dimensions, 'RGBA'))
    
    def get_events(self):
        return pygame.event.get()
    
    def draw_frame(self, fill: tuple[int, int, int]):
        # if enabled, draws fps on the top right
        if(self.__tracking_frames):
            self.__update_frame_deltas()
            
            if(perf_counter() - self.__last_update >= self.__update_delta):
                if(self.__has_fps): 
                    # memory leak if removed
                    self.canvas.pop()  
                avg = self.__get_frame_average()
                frame_avg_text = f"{avg} FPS"
                self.draw_text(frame_avg_text, (100, 0), (255, 255, 255))
                self.__has_fps = True
                self.__last_update = perf_counter()
        
        self.screen.fill(fill)
        self.screen.blit(self.frame, (0, 0))
        pygame.display.flip()

    
    def draw_canvas(self, bg: pygame.surface.Surface = None):
        if(bg != None): 
            self.frame = bg

        self.frame.fill((0, 0, 0))
        self.frame.blits(self.canvas)
        self.frame.set_alpha(self.curr_alpha)

    def draw_text(
        self, 
        text: str, 
        position: tuple[int, int], 
        font: tuple = ("arial", 12),
        fg: tuple[int, int, int] = (255, 255, 255),
        bg: tuple[int, int, int] = None, 
        alpha: int = 255
    ):
        print("aq", font)
        if(font):
            lfont = self.load_font(font[0], font[1])
        else:
            lfont = self.default_font

        self.draw_generic(lfont.render(text, False, fg, bg), position, alpha)
    
    def draw_generic(
        self, 
        surface: 
        pygame.surface.Surface, 
        position: 
        tuple[int, int], 
        alpha: int = 255
    ):
        """Appends a generic surface object to the canvas frame queue""" 
        surface.set_alpha(alpha)
        self.canvas.append((surface, position))
    
    def fade_screen_step(self, delta):
        self.curr_alpha = self.curr_alpha + delta
    
    def set_alpha(self, alpha):
        self.curr_alpha = alpha
 
    def get_alpha(self):
        return self.curr_alpha
 
    def screen_sleep(self, duration):
        pygame.time.wait(duration)

    def cap_fps(self, fps_cap):
        """
        Caps fps frfr n cap
        ⡯⡯⡯⣯⢯⢿⢽⠝⡰⡭⣫⡓⠝⠈⠑⠉⠈⠑⠙⠝⣸⢿⡽⣯⢿⣽⣻⡽⡨⡊
        ⡯⡯⣟⣽⢽⢯⢋⢔⢽⠉⢁⢠⡰⣬⡪⠬⢄⡡⡠⠄⠈⢿⡽⡯⣟⣾⣳⡯⢏⠯ 
        ⡯⡿⡽⣞⡯⡏⡢⡡⠁⠰⠘⠘⢼⡰⢔⠖⣆⠬⠐⠁⢂⠈⢻⣽⣻⣞⡷⡏⡰⢈ 
        ⡯⣟⡽⠓⠋⠁⣠⠄⠠⠑⠐⠑⠄⢀⠈⠉⡀⠈⠐⠐⠐⠄⠄⣁⠑⠑⠻⣳⢵⣳ 
        ⣈⣀⢤⢤⢶⡻⠃⠄⡢⠓⠁⠐⢀⠑⠍⢉⠃⠁⠐⠄⠂⠆⠄⠙⠽⡦⣄⠈⠙⠺ 
        ⠓⡩⠩⠩⠃⠄⠄⠄⢅⢰⠉⢰⡀⡰⣴⡢⠄⠢⣗⠁⡆⠄⠄⠄⠄⠈⠝⠩⠢⠄ 
        ⠄⠄⠁⠌⠂⡲⡄⠄⡪⣂⢉⢈⡰⣺⣺⢽⣀⠁⡀⡀⠄⠂⠄⡯⡛⠌⠄⠡⠨⠈ 
        ⣠⢤⣀⣀⡀⠄⠄⠄⢈⠪⡲⣵⠉⠈⠈⠍⠈⠁⣗⢆⠊⠄⢀⣀⣀⢤⣲⡒⡠⠄ 
        ⡯⣯⢯⣟⡾⡽⡶⣄⠄⢂⠩⡺⡥⡦⣕⣤⡢⠈⢎⠐⠄⡰⣯⢷⣻⢽⣝⡇⣴⡕ 
        ⡯⣗⣿⣺⢽⡪⡯⣻⡄⠐⠨⡘⠌⠉⠈⠈⠨⢑⢀⠊⢠⢿⢽⣝⠮⠋⠐⠘⠑⠁ 
        ⠯⢗⣗⡯⣷⢝⠮⠳⢉⠄⠑⢔⢄⠄⠄⠄⠄⢀⢌⠂⣈⠋⠓⢁⣁⣤⣶⣗⣷⢿ 
        ⣤⣄⣀⣀⣀⣀⣤⣴⢽⣏⠄⠘⢜⡘⠜⡌⢇⠕⠄⢠⡯⣟⣽⣻⣽⣟⣾⣽⡾⣿ 
        ⣿⢽⣯⢿⣻⣽⢷⣻⣽⣞⣦⣄⣀⣀⠁⡈⣀⡠⡴⡯⡯⣗⣯⡿⣾⣽⢾⣷⣻⣟ 
        ⡯⣿⢾⣟⣯⣿⣻⣯⡷⣟⣞⢾⣝⡾⡽⣮⡳⡯⣫⢯⢯⣻⣞⣿⣳⣟⡿⣞⣷⣻ 
        ⢝⣞⣯⣟⣷⢯⣷⢿⣽⣟⣾⣳⣗⡯⣟⡾⡽⣝⣗⡯⣗⢷⣻⣽⢷⣻⣟⣽⢞⢮
        """
        self.fps_cap = fps_cap

    def fps_tick(self):
        self.fps_clock.tick(self.fps_cap)

    def quit(self):
        # destroys game window and exits
        pygame.quit()

    def __update_frame_deltas(self):
        """
        Internal method: updates internal frame timestamp list.
        
        Updates the internal list of recent frames by appending the current frame's timestamp.
        This method uses a collections.deque, which provides more efficient performance compared
        to using standard Python lists, where the complexity of appending an element is O(n).
        By setting a maximum length to the deque, the append operation has a complexity of O(1).
        """
        self.__recent_frames.append(perf_counter())  # updates queue with new frame time
    def __get_frame_average(self):
        """Internal method: returns framerate average"""
        if(len(self.__recent_frames) < 2):  # minimal sample
            return "NaN"

        try:
            deltas = []
            for i in range(1, len(self.__recent_frames)):
                delta = self.__recent_frames[i] - self.__recent_frames[i-1]
                deltas.append(delta)
            
            total_sample_time = sum(deltas)
            sample_lenght = len(deltas)
            avg = 1/(total_sample_time/sample_lenght)

            return f"{avg:.2f}"
        except ZeroDivisionError as e:
            # probably not enough of a sample
            return "NaN"

