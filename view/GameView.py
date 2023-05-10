# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')

from os import environ
environ['PYGAME_HIDE_SUPPORT_PROMPT'] = '1'

import pygame

from CONSTANTS import SCREEN_WIDTH, SCREEN_HEIGHT

class GameView:
    def __init__(self, sprite_model):
        self.sprite_manager = sprite_model

        self.game = pygame.init()
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))

        # set game clock
        self.clock = pygame.time.Clock()
        
        # set frames per second
        self.clock.tick(60)
        
    def set_title(self, title):
        self.game.display.set_caption(title)

    def load_sprite(self, target):
        self.player_image = self.sprite_manager.load_sprite("player.png")

    def draw_frame(self, screen):

        self.game.screen.blit(self.player_image, (self.model.player_x, self.model.player_y))
        self.game.display.update()
    def quit():

        # encerrate o jogo python
        self.game.quit()


 self.player_image = pygame.image.load(os.path.join(IMAGE_DIR, "player.png"))

   def draw(self, screen):
        # desenha o jogador na tela
        screen.blit(self.player_image, (self.model.player_x, self.model.player_y))
