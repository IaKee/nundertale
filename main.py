# local modules
from CONSTANTS import *

# initialize pygame
pygame.init()

# create game window
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))

# create the game model, viewer and the controller
model = GameModel()
view = GameView(model)
controller = GameController(model)

# set game clock
clock = pygame.time.Clock()

# game loop
while True:                                         # mudar pra quando der erro enfiar tudo no cu
    # event manager
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            quit()

    controller.update()

    # render
    screen.fill(BLACK)
    view.draw(screen)

    # update game window
    pygame.display.update()

    # set frames per second
    clock.tick(60)

# encerrate o jogo python
pygame.quit()