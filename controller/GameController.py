from .InputTracker import InputTracker

from CONSTANTS import *
class GameController:
    def __init__(self):
        self.player_coords = {"x": 0, "y": 0}
        self.player_speed = 0
        self.current_level = 0

        self.input_tracker = InputTracker()
        self.__game_should_close = False
        self.__close_source = None

    def get_update_events(self):
        """Updates events from view"""
        self.__events = self.view.get_events()

    def process_events(self):
        """Reads keyboard/controller input from internal events variable"""
        self.movement, self.action = self.input_tracker.process_keypresses(self.__events)

    def link_view(self, reference):
        """References view (mvc) to this level"""
        self.view = reference

    def link_model(self, reference):
        """References model (mvc) to this level"""
        self.sprite_model = reference

    def main_loop(self):
        """Main logic game loop"""
        while(not self.__game_should_close):
            self.get_update_events()
            self.process_events()

            self.view.draw_frame()
    def quit(self):
        """Closes game"""
        self.view.quit()
        exit()