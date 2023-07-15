from os import environ
environ['PYGAME_HIDE_SUPPORT_PROMPT'] = '1'
from pygame import Surface, image, mask, SRCALPHA

class GameSprite():
    def __init__(self, filedata):
        self.mask_list = []
        self.loaded_image = image.fromstring(filedata.get_raw(), filedata.get_geometry(), filedata.get_mode())

    def add_mask(self, mask_rect):
        """Adds colision mask to sprite object"""
        mask_surface = Surface((self.width, self.height), SRCALPHA)
        mask_surface.blit(self.loaded_image, (0, 0))
        mask_surface.fill((255, 255, 255, 0), mask_rect)
        mask = mask.from_surface(mask_surface)
        self.mask_list.append(mask)

    def check_mask_colision(self, other_sprite):
        """Checks if any of this sprites masks are coliding with any other sprites masks"""
        for mask in self.mask_list:
            for other_mask in other_sprite.mask_list:
                    if(mask.overlap(other_mask)):
                        return True
        return False