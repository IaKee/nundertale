# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')

from model.GameSprite import GameSprite
from model.FileManager import FileManager

from CONSTANTS import SPRITE_DIR

class PlayerModel():
    def __init__(self):
        self.position = [0, 0]
        self.movespeed = (1, 1)
        self.file_manager = FileManager()
        self.sprite: GameSprite = GameSprite(self.file_manager.load_image(f'{SPRITE_DIR}/player.png'))

    def mov(self, x, y):
        if( -1 <= x*y <= 1):
            self.position[0] += x * self.movespeed[0]
            self.position[1] += y * self.movespeed[1]
        else:
            raise Exception(f'player movement vector should be between (-1, -1) and (1, 1), was ({x}, {y})')
        
    def set_pos(self, x, y):
        self.position = (x, y)
        