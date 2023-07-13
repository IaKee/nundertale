# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')

from CONSTANTS import ASSETS_DIR

from os import environ
import os
# hides the default 'hello world' message from the pygame module
environ['PYGAME_HIDE_SUPPORT_PROMPT'] = '1'

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
