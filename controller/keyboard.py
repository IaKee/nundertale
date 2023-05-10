class GameController:
    def __init__(self, model):
        self.model = model
    
    def update(self):
        # check and move player based on arrow keys
        keys = pygame.key.get_pressed()
        if keys[pygame.K_LEFT]:
            self.model.player_x -= self.model.player_speed
        if keys[pygame.K_RIGHT]:
            self.model.player_x += self.model.player_speed
        if keys[pygame.K_UP]:
            self.model.player_y -= self.model.player_speed
        if keys[pygame.K_DOWN]:
            self.model.player_y += self.model.player_speed