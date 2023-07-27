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
        self.loaded_fonts = {}

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
    
    def load_font(self, font_string):
        """Loads a font and stores it in an internal class variable if it's not already loaded.

        The format_str string should have the format "fontname_size_style", but each part is optional.
        If the format is invalid, a ValueError will be raised.

        Parameters:
            format_str (str): The string with the font format to be loaded. The default is "arial_12_regular".

        Returns:
            pygame.font.Font: The loaded font object.
        """

        # font defaults

        # checks if the font is already loaded, if so, return it
        if font_string in self.loaded_fonts:
            return self.loaded_fonts[font_string]

        font_size = 12
        font_style = ""

        parts = font_string.split("_")
        n_parts = len(parts)
        if n_parts == 1:
            font_name = parts[0]
        elif n_parts == 2:
            try:
                font_name = parts[0]
                font_size = int(parts[1])
            except ValueError:
                raise ValueError("Invalid font size")
        elif n_parts == 3:
            font_name = parts[0]
            font_size = int(parts[1])
            font_style = parts[2]
        elif n_parts > 3:
            raise ValueError("Invalid font format!")

        # Load the font and store it in the dictionary of loaded fonts
        if(pygame.font.match_font(font_name)):
            font = pygame.font.SysFont(font_name, font_size, bold="bold" in font_style, italic="italic" in font_style)
        else:
            font = pygame.font.Font(FONTS_DIR + font_name, font_size,  bold="bold" in font_style, italic="italic" in font_style) 
        
        self.loaded_fonts[font_string] = font
        return font

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
                self.draw_text(
                    text = frame_avg_text, 
                    font = "arial_12",
                    position = (SCREEN_WIDTH-55, 5), 
                    fg = (255, 255, 255))
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
        font: str = None,
        fg: tuple[int, int, int] = (255, 255, 255),
        bg: tuple[int, int, int] = None, 
        alpha: int = 255
    ):
        
        if(font):
            lfont = self.load_font(font)
        else:
            lfont = self.default_font

        self.draw_generic(lfont.render(text, False, fg, bg), position, alpha)
    
    def draw_sprite(self, sprite, pos, alpha = 255):
        self.draw_generic(sprite.loaded_image, pos, alpha)
        pass

    def draw_generic(
        self, 
        surface: pygame.surface.Surface, 
        position: tuple[int, int], 
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

