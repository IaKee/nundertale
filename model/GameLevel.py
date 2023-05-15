from model.FileManager import FileManager
from model.GameSprite import GameSprite
from log.logger import flog
class GameLevel:
    def __init__(self, sprite_path_list):
        self.file_manager = FileManager()
        self.sprite_path_list = sprite_path_list
        self.initial_player_position = {"x": 0, "y": 0} 
        self.sprite_list = []
    
    def load_level_sprites(self):
        for sprite in self.sprite_path_list:
            raw = self.file_manager.load_file(sprite)
            width, height = raw.shape
            self.sprite_list.append(GameSprite(raw, width, height))

    def set_initial_player_pos(self, x: int, y: int):
        self.initial_player_position = {x, y}