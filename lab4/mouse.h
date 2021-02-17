#include <lcom/lcf.h>

#include <lcom/lab4.h>

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "utils.h"
#include "timer.h"

extern bool gesture;

//enum c_state{INIT, RDOWN, VERTEX, RUP, HOR_TOLERANCE};

int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();

int write_cmd_kbd();

int mouse_enable_data_reporting2(uint32_t cmd2);

void (mouse_ih)();

void packet_make();

struct mouse_ev mouse_detect_event2(struct packet *pp);

void mouse_detect_gesture(struct mouse_ev ev, uint8_t x_len, uint8_t tolerance);
