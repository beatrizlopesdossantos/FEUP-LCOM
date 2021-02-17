#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <math.h>
#include "video.h"

static uint16_t h_res;
static uint16_t v_res;
static uint8_t bits_per_pixel;
static uint8_t bytes_per_pixel;
static uint8_t red_mask, blue_mask, green_mask;
static uint8_t red_pos, green_pos, blue_pos;
static char *video_mem; 
uint16_t mode_aux = 0;

uint16_t get_h_res() {
    return h_res;
}
uint16_t get_v_res() {
    return v_res;
}
uint8_t get_bytes() {
    return bytes_per_pixel;
}
int map_vram(vbe_mode_info_t vbe, uint32_t numPixel) {
    int r1;
    struct minix_mem_range mr;  /* physical memory range */
    unsigned int vram_base = vbe.PhysBasePtr; /* VRAM’s physical addresss */
    unsigned int vram_size = numPixel * bytes_per_pixel; /* VRAM’s size, but you can use the frame-buffer size, instead */
    
    /* Allow memory mapping */
    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;
    if( OK != (r1 = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
        panic("sys_privctl (ADD_MEM) failed: %d\n", r1);
        return 1;
    }
    /* Map memory */
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
    if(video_mem == MAP_FAILED) {
        panic("couldn’t map video memory");
        return 1;
    }
    return 0;
}

void *(vg_init)(uint16_t mode) {
    uint32_t numPixel;

    vbe_mode_info_t vbe;
    vbe_get_mode_info_mine(mode, &vbe);

    h_res = vbe.XResolution;
    v_res = vbe.YResolution;
    bits_per_pixel = vbe.BitsPerPixel;
    bytes_per_pixel = ceil((float)bits_per_pixel / 8.0);
    red_mask = vbe.RedMaskSize;
    green_mask = vbe.GreenMaskSize;
    blue_mask = vbe.BlueMaskSize;
    red_pos = vbe.RedFieldPosition;
    green_pos = vbe.GreenFieldPosition;
    blue_pos = vbe.BlueFieldPosition;

    numPixel = h_res * v_res;
    int r1;
    struct minix_mem_range mr;  /* physical memory range */
    unsigned int vram_base = vbe.PhysBasePtr; /* VRAM’s physical addresss */
    unsigned int vram_size = numPixel * bytes_per_pixel; /* VRAM’s size, but you can use the frame-buffer size, instead */
    printf("%d",bits_per_pixel);
    /* Allow memory mapping */
    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;
    if( OK != (r1 = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
        panic("sys_privctl (ADD_MEM) failed: %d\n", r1);
    }
    /* Map memory */
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
    if(video_mem == MAP_FAILED) {
        panic("couldn’t map video memory");
    }

    /*if (map_vram(vbe, numPixel) != OK) {
        printf("Error in map_vram");
        return NULL;
    }*/
    
    reg86_t r;
    memset(&r, 0, sizeof(r));
    r.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
    r.bx = 1<<14|mode; // set bit 14: linear framebuffer
    r.intno = 0x10;
    if (sys_int86(&r) != OK ) {
        printf("set_vbe_mode: sys_int86() failed \n");
        return NULL;
    }
    return video_mem;
}

void vg_color_pixel(uint16_t x, uint16_t y, uint32_t color) {

    char *aux_mem = video_mem + (y * h_res * bytes_per_pixel) + x * bytes_per_pixel;
    if (mode_aux == 0x110) {
        color = color & 0x7FFF;
    }
    for (int j = 0; j < bytes_per_pixel; j++) {
        *aux_mem = (color >> (8 * j));
        aux_mem++;
    }
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (int i = 0; i < len; i++) {
        vg_color_pixel(x + i, y, color);
    }
    return 0;

}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (int k = 0; k < height; k++) {
        vg_draw_hline(x, y + k, width, color);
    }
    return 0;
}

int (vg_draw_rectangle1)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    uint16_t len = 1;
    for (int k = 0; k < height; k++) {
        vg_draw_hline(x, y + k, len, color);
        if (len == width) {
            break;
        }        
        len++;
        x--;
        
    }

    return 0;
}

uint32_t color_indexed(uint8_t no_rectangles, uint32_t first, uint8_t step, uint8_t row, uint8_t col) {
    return (first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel);
}

uint32_t color_direct(uint8_t no_rectangles, uint32_t first, uint8_t step, uint8_t row, uint8_t col) {
    uint32_t color = 0;
    uint8_t red, green, blue;
    red = first >> red_pos % BIT(red_mask);
    green = first >> green_pos % BIT(green_mask);
    blue = first >> blue_mask % BIT(blue_mask);

    red = (red + col * step) % (1 << red_mask);
    green = (green + row * step) % (1 << green_mask);
    blue = (blue + (col + row) * step) % (1 << blue_mask);

    color = (red << red_pos | green << green_pos | blue << blue_pos);
    
    return color;

}

int vg_draw_pattern(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
    uint16_t width, height;
    uint32_t color;
    width = h_res/no_rectangles;
    height = v_res/no_rectangles;

    for (int i = 0; i < no_rectangles; i++) {
        for (int j = 0; j < no_rectangles; j++) {

            if (mode == INDEXED) {
                color = color_indexed(no_rectangles, first, step, i, j); 
            }
            else {
                color = color_direct(no_rectangles, first, step, i, j);
            }
            vg_draw_rectangle(j * width, i * height, width, height, color);

        }
    }
    return 0;
}


int vg_draw_pixmap(xpm_map_t xpm, uint16_t x, uint16_t y) {

    xpm_image_t img;
    uint8_t *map;
    //int count = 0;
    uint32_t color;
    // get the pixmap from the XPM
    map = xpm_load(xpm, XPM_INDEXED, &img);
    // copy it to graphics memory
    for (int i = 0; i < img.height; i++) {
        for (int j = 0; j < img.width; j++) {
            color = map[j + i * img.width];
            //count++;
            vg_color_pixel(x + j, y + i, color);
        }
    }
    free(map);
    return 0;
}

int (vbe_get_mode_info_mine)(uint16_t mode, vbe_mode_info_t* vmi_p) {
    mmap_t map;
    reg86_t r;
    phys_bytes buf;

    memset(&r, 0, sizeof(r));
    if (lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL) {
        return 1;
    }

    buf = map.phys;

    r.ax = 0x4F01;
    r.es = PB2BASE(buf);
    r.di = PB2OFF(buf);
    r.cx = mode;
    r.intno = 0x10;

    if (sys_int86(&r) != OK ) {
        printf("get_vbe_mode_info: sys_int86() failed \n");
        return 1;
    }
    *vmi_p = *(vbe_mode_info_t*) map.virt;
    lm_free(&map);
    return 0;
}

