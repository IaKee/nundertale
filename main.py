# importa as bibliotecas necessárias
import pygame
import os

# define as constantes do jogo
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600

# define as cores usadas no jogo
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)

# define o diretório de imagens
IMAGE_DIR = "imagens"

# define a classe do modelo do jogo
class GameModel:
    def __init__(self):
        self.player_x = SCREEN_WIDTH / 2
        self.player_y = SCREEN_HEIGHT / 2
        self.player_speed = 5

# define a classe do controle do jogo
class GameController:
    def __init__(self, model):
        self.model = model
    
    def update(self):
        # move o jogador com as teclas de seta
        keys = pygame.key.get_pressed()
        if keys[pygame.K_LEFT]:
            self.model.player_x -= self.model.player_speed
        if keys[pygame.K_RIGHT]:
            self.model.player_x += self.model.player_speed
        if keys[pygame.K_UP]:
            self.model.player_y -= self.model.player_speed
        if keys[pygame.K_DOWN]:
            self.model.player_y += self.model.player_speed

# define a classe da visão do jogo
class GameView:
    def __init__(self, model):
        self.model = model
        self.player_image = pygame.image.load(os.path.join(IMAGE_DIR, "player.png"))
    
    def draw(self, screen):
        # desenha o jogador na tela
        screen.blit(self.player_image, (self.model.player_x, self.model.player_y))

# inicializa o Pygame
pygame.init()

# cria a janela do jogo
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))

# cria o modelo, a visão e o controle do jogo
model = GameModel()
view = GameView(model)
controller = GameController(model)

# define o relógio do Pygame
clock = pygame.time.Clock()

# loop principal do jogo
while True:
    # processa eventos
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            quit()

    # atualiza o controle do jogo
    controller.update()

    # desenha a visão do jogo na tela
    screen.fill(BLACK)
    view.draw(screen)

    # atualiza a tela
    pygame.display.update()

    # define a taxa de quadros do jogo
    clock.tick(60)

# encerra o Pygame
pygame.quit()