# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')

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

    def start(self): 
        """Initializes pygame and the main game window itself"""
        pygame.init()
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        
        self.fade_solid = pygame.Surface(self.screen.get_size())

        # set game clock
        self.clock = pygame.time.Clock()
        
        # limits frames per second to set rate
        self.clock.tick(self.frame_rate)
      
    def set_title(self, title):
        pygame.display.set_caption(title)
    def load_sprite(self, sprite_raw, dimensions):
        self.loaded_sprites.append(pygame.image.fromstring(sprite_raw, dimensions, 'RGBA'))
    def get_events(self):
        return pygame.event.get()
    def draw_frame(self):
        pygame.display.flip()
    def quit(self):
        # destroys game window and exits
        self.game.quit()
    def draw(self, screen):
        # desenha o jogador na tela
        screen.blit(self.player_image, (self.model.player_x, self.model.player_y))
