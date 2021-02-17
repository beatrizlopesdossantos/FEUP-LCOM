#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "video.h"
#include "sprite.h"

/** Creates a new sprite with pixmap "pic", with specified
* position (within the screen limits) and speed;
* Does not draw the sprite on the screen
* Returns NULL on invalid pixmap.
*/
Sprite *create_sprite(xpm_map_t pic, int x, int y, int xspeed, int yspeed) {
    //allocate space for the "object"
    Sprite *sp = (Sprite *) malloc ( sizeof(Sprite));
    xpm_image_t img;
    if( sp == NULL )
        return NULL;
    // read the sprite pixmap
    sp->map = (char*)xpm_load(pic, XPM_INDEXED, &img);
    if( sp->map == NULL ) {
        free(sp);
        return NULL;
    }
    sp->width = img.width;
    sp->height = img.height;
    sp->x = x;
    sp->y = y;
    sp->xspeed = xspeed;
    sp->yspeed = yspeed;
    return sp;
}

void destroy_sprite(Sprite *sprite) {
    if( sprite == NULL )
        return;
    if( sprite ->map )
        free(sprite->map);
    free(sprite);
    sprite = NULL; // XXX: pointer is passed by value
    // should do this @ the caller
}

int draw_sprite(Sprite *sprite) {
    int width = sprite->width;
    int height = sprite->height;
    uint32_t color;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            color = sprite->map[i*width +j];
            vg_color_pixel(sprite->x + j, sprite->y + i, color);
        }
    }
    return 0;
}

