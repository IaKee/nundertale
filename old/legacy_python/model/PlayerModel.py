# changes path hierachy level to import other modules
from sys import path
path.insert(1, '.')

from model.GameSprite import GameSprite
from model.FileManager import FileManager
from pygame.math import Vector2, clamp

from CONSTANTS import SPRITE_DIR

class PlayerModel():
    def __init__(self):
        self.position = Vector2(0, 0)
        self.movespeed = 1
        self.file_manager = FileManager()
        self.sprite: GameSprite = GameSprite(self.file_manager.load_image(f'{SPRITE_DIR}/player.png'))

    def mov(self, mov_vec2):
           #clamped_mov = [clamp(mov_vec2[0], -1, 1), clamp(mov_vec2[1], -1, 1)]
           #print(self.position + clamped_mov)
           print(f'{self.position} -> {mov_vec2}')
           self.position.move_towards_ip(mov_vec2, self.movespeed)
        
    def set_pos(self, pos_vec2):
        self.position = pos_vec2
        