from model import SpriteManager
from view import GameView
from controller import GameController

# create the game model, viewer and the controller
model = GameModel()
view = GameView(model)
controller = GameController(model)

# game loop
while True:
    # event manager
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            quit()

    controller.update()

    # render
    screen.fill(BLACK)
    view.draw(screen)

