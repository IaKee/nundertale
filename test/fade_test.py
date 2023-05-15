import pygame

# Inicializar o Pygame
pygame.init()

# Configurar a janela do jogo
screen = pygame.display.set_mode((800, 600))
pygame.display.set_caption("Fade Example")

# Criar uma superfície que cobre toda a tela
fade_surface = pygame.Surface(screen.get_size())
fade_surface.fill((255, 255, 255))  # Cor branca

# Definir a transparência inicial
alpha = 255

# Loop principal do jogo
running = True
while running:
    # Limpar a tela
    screen.fill((0, 0, 0))

    # Desenhar a cena existente
    pygame.draw.rect(screen, (255, 0, 0), (100, 100, 200, 150))  # Retângulo vermelho
    font = pygame.font.Font(None, 36)
    text = font.render("Hello, World!", True, (255, 255, 255))
    screen.blit(text, (300, 300))  # Texto

    # Desenhar a superfície de fade com a transparência atual
    fade_surface.set_alpha(alpha)
    screen.blit(fade_surface, (0, 0))

    # Atualizar a exibição
    pygame.display.flip()

    # Atualizar a transparência
    if alpha > 0:
        alpha -= 0.5
    else:
        # O fade está completo, encerrar o loop
        running = False

    # Verificar eventos
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

# Encerrar o Pygame
pygame.quit()
