# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')

from CONSTANTS import ASSETS_DIR

from os import environ
# hides the default 'hello world' message from the pygame module
environ['PYGAME_HIDE_SUPPORT_PROMPT'] = '1'
from time import perf_counter
import pygame

from CONSTANTS import SCREEN_WIDTH, SCREEN_HEIGHT

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

        # fps tracking
        self.__frame_sample_size = 100
        self.__tracking_frames = True
        self.__last_frame = None
        self.__recent_frames = []
        self.__has_fps = False

    def start(self): 
        """Initializes pygame and the main game window itself"""
        pygame.init()
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        
        self.fade_solid = pygame.Surface(self.screen.get_size())

        # set game clock
        self.clock = pygame.time.Clock()
        
        # limits frames per second to set rate
        self.clock.tick(self.frame_rate)

        self.font = pygame.font.Font(ASSETS_DIR + '/fonts/8bitoperator.ttf', 11)
      
    def set_title(self, title):
        pygame.display.set_caption(title)
    
    def set_icon(self, icon):
        pygame.display.set_icon(icon)
    
    def load_sprite(self, sprite_raw, dimensions):
        self.loaded_sprites.append(pygame.image.fromstring(sprite_raw, dimensions, 'RGBA'))
    
    def get_events(self):
        return pygame.event.get()
    
    def draw_frame(self, fill: tuple[int, int, int]):
        # if enabled, draws fps on the top right
        if(self.__tracking_frames):
            if(self.__has_fps): 
                # memory leak if removed
                self.canvas.pop()  

            self.__update_frame_deltas()
            avg = self.__get_frame_average()
            frame_avg_text = f"{avg} FPS"
            self.draw_text(frame_avg_text, (100, 0), (0, 0, 0))
            self.__has_fps = True
        
        self.screen.fill(fill)
        self.screen.blit(self.frame, (0, 0))
        pygame.display.flip()

    
    def draw_canvas(self, bg: pygame.surface.Surface = None):
        if(bg != None): 
            self.frame = bg
        else:
            self.frame = pygame.surface.Surface(self.screen.get_size())
        self.frame.fill((255, 255, 255))
        self.frame.blits(self.canvas)
        self.frame.set_alpha(self.curr_alpha)

    def set_font(self, font: str, size: int):
        self.font = pygame.font.Font(font, size)
    
    def draw_text(
        self, 
        text: str, 
        position: tuple[int, int], 
        fg: tuple[int, int, int] = (255, 255, 255),
        bg: tuple[int, int, int] = None, 
        alpha: int = 255
    ):
        self.draw_generic(self.font.render(text, False, fg, bg), position, alpha)
    
    def draw_generic(self, surface: pygame.surface.Surface, position: tuple[int, int], alpha: int = 255):
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

    def quit(self):
        # destroys game window and exits
        pygame.quit()

    def __update_frame_deltas(self):
        if(len(self.__recent_frames) >= self.__frame_sample_size):
            self.__recent_frames.pop(0)  # very inneficient - could be resource intensive (O(n))
        self.__recent_frames.append(perf_counter())  # updates queue with new frame time
    def __get_frame_average(self):
        """Returns framerate average, from the list samples, as a string"""
        if(len(self.__recent_frames) < 2):  # minimal sample
            return "NaN"

        try:
            deltas = []
            for i in range(1, len(self.__recent_frames)):
                delta = self.__recent_frames[i] - self.__recent_frames[i-1]
                #print(f"{delta}")
                deltas.append(delta)
            
            total_sample_time = sum(deltas)
            sample_lenght = len(deltas)
            avg = 1/(total_sample_time/sample_lenght)
            print(avg)  # debug - remove this

            return f"{avg:.2f}"
        except ZeroDivisionError as e:
            # probably not enough of a sample
            return "NaN"

