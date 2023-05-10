# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')

# local imports
from SpriteManager import *

class GameView:
    def __init__(self, sprite_model):
        self.sprite_manager = sprite_model

        self.player_image = self.sprite_manager.load_sprite("player.png")
    
    def draw_frame(self, screen):
        screen.blit(self.player_image, (self.model.player_x, self.model.player_y))
