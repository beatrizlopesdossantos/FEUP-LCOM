#include <lcom/lcf.h>
#include <lcom/lab5.h>

typedef struct {
    int x, y; // current position
    int width, height; // dimensions
    int xspeed, yspeed; // current speed
    char *map; // the pixmap
} Sprite;

Sprite *create_sprite(xpm_map_t pic, int x, int y, int xspeed, int yspeed);

void destroy_sprite(Sprite *sp);

int draw_sprite(Sprite *sprite);

