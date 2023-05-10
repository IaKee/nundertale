# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')

from os import environ
environ['PYGAME_HIDE_SUPPORT_PROMPT'] = '1'

import pygame

class GameView:
    def __init__(self, sprite_model):
        self.sprite_manager = sprite_model

        self.game = pygame.init()
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))

        # set game clock
        self.clock = pygame.time.Clock()
        
        # set frames per second
        clock.tick(60)
        
        #self.player_image = self.sprite_manager.load_sprite("player.png")
    def set_title(self, title):
        self.game.display.set_caption(title)
    def draw_frame(self, screen):
        self.game.screen.blit(self.player_image, (self.model.player_x, self.model.player_y))
        self.game.display.update()
    def quit():
        # encerrate o jogo python
        self.game.quit()
