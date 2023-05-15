from pygame import image

class GameSprite():
    def __init__(self, raw_image, width, height):
        self.width = width
        self.height = height
        self.mask_list = []
        self.loaded_image = image.fromstring(raw_image, (width, height), "RGBA")

    def check_mask_colision(self, other_sprite):
        """Checks if any of this sprites masks are coliding with any other sprites masks"""
        for mask in self.mask_list:
            for other_mask in other_sprite.mask_list:
                    if(mask.overlap(other_mask)):
                        return True
        return False