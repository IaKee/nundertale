from sys import exit

from KeyboardTracker import *
from CONSTANTS import *


class GameController:
    def __init__(self):
        self.player_coords = {"x": 0, "y": 0}
        self.player_speed = 0
        
    def read_keyboard(self):
        if self.view.game.event.type == self.view.game.KEYDOWN:
            if self.view.game.event.key == self.view.game.K_LEFT:
                self.player_coords['x'] -= 10
            elif self.view.game.event.key == self.view.game.K_RIGHT:
                self.player_coords['x'] += 10
            elif self.view.game.event.key == self.view.game.K_UP:
                self.player_coords['y'] -= 10
            elif self.view.game.event.key == self.view.game.K_DOWN:
                self.player_coords['y'] += 10

    def link_view(self, reference):
        self.view = reference
    def link_sprite_model(self, reference):
        self.sprite_model = reference
    def main_loop(self):

        while True:
            # event manager
            for event in self.view.game.event.get():
                if event.type == self.view.game.QUIT:
                    self.quit()

            self.view.screen.fill(C_BLACK)

            # render new frame
            self.view.draw_frame()
    def quit(self):
        """Closes game"""
        self.view.quit()
        exit()