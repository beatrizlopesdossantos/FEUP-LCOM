#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "utils.h"

uint8_t get_bytes();

uint16_t get_v_res();

uint16_t get_h_res();

void *(vg_init)(uint16_t mode);

int map_vram(vbe_mode_info_t vbe, uint32_t numPixel);

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

void vg_color_pixel(uint16_t x, uint16_t y, uint32_t color);

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

int vg_draw_pattern(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);

uint32_t color_direct(uint8_t no_rectangles, uint32_t first, uint8_t step, uint8_t row, uint8_t col);

uint32_t color_indexed(uint8_t no_rectangles, uint32_t first, uint8_t step, uint8_t row, uint8_t col);

int vg_draw_pixmap(xpm_map_t xpm, uint16_t x, uint16_t y);

int (vbe_get_mode_info_mine)(uint16_t mode, vbe_mode_info_t* vmi_p);

int (vg_draw_rectangle1)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
