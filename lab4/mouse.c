#include <lcom/lcf.h>

#include <lcom/lab4.h>

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"


int hook_id;
uint8_t byte;
uint8_t packet[3];
struct packet pp;

int (mouse_subscribe_int)(uint8_t *bit_no) {
    hook_id = MOUSE_IRQ;
    *bit_no = hook_id;

    if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) == OK) {
        printf("after sys_irqsetpolicy: hook_id = 0x%x\n", hook_id);
        return 0;
    }
    else {
        printf("sys_irqsetpolicy() failed\n");
        return 1;
    }
}

int (mouse_unsubscribe_int)() {
    if (sys_irqrmpolicy(&hook_id) != OK) {
        printf("sys_irqmpolicy() failed\n");
        return 1;
    }
    return 0;
}

int write_cmd_kbd() {
    int try = 0;
    uint8_t st;
    while (try < 4) {
        if (util_sys_inb(KBC_ST_REG, &st) != OK) {
            printf ("Error in util_sys_inb()\n");
            return 1;
        }
        if (!(st & IBF)) {
            if (sys_outb(KBC_CMD_REG, WRITE_BYTE_MOUSE) != OK) {
                printf("Error in sys_outb()\n");
                return 1;
            }
            return 0;
        }
        try++;
        tickdelay(micros_to_ticks(DELAY_US));        
    }
    printf("To many failed tries.\n");
    return 1;

}


int mouse_enable_data_reporting2(uint32_t cmd2) {
    uint8_t ack = 0;
    int try = 0;
    uint8_t st;    
    while (ack != ACK) {
        if (write_cmd_kbd() != OK) {
            printf("Error in write command");
            return 1;
        }
        while (try < 4) {
            if (util_sys_inb(KBC_ST_REG, &st) != OK) {
                printf ("Error in util_sys_inb()\n");
                return 1;
            }

            if (!(st & IBF)) {
                if (sys_outb(OUT_BUF, cmd2) != OK) {
                    printf("Error in sys_outb()\n");
                    return 1;
                }

                tickdelay(micros_to_ticks(DELAY_US));

                if (util_sys_inb(OUT_BUF, &ack) != OK) {
                    printf("Error in sys_outb()\n");
                    return 1;
                }
                printf("%x\n", ack);
                break;
            }

            try++;
            tickdelay(micros_to_ticks(DELAY_US));
            
        }
        if (try == 4) {
            printf("To many failed tries.\n");
            return 1;            
        }

        if (ack == NACK) {
            continue;
        }
        else if (ack == ERROR) {
            return 1;
        }
    }

    return 0;

}

void (mouse_ih)() {
    uint8_t status;
    if (util_sys_inb(KBC_ST_REG, &status) != OK) {
        printf("Error in util_sys_inb()\n");
        return;
    }
    if (status & OBF) {
        if ((status & (PAR_ERROR | TIMEOUT_ERROR)) == 0) {
            if (util_sys_inb(OUT_BUF, &byte) != OK) {
                printf("Error in util_sys_inb()\n");
            return;
            }
        }
    }
    tickdelay(micros_to_ticks(DELAY_US));
}

void packet_make() {
    pp.bytes[0] = packet[0];
    pp.bytes[1] = packet[1];
    pp.bytes[2] = packet[2];
    pp.rb = (packet[0] & RB);
    pp.mb = (packet[0] & MB);
    pp.lb = (packet[0] & LB);
    pp.x_ov = (packet[0] & X_OVFL);
    pp.y_ov = (packet[0] & Y_OVFL);
    if (packet[0] & MSB_X) {
        pp.delta_x = packet[1] - 256;
    }
    else {
        pp.delta_x = packet[1];
    }
    if (packet[0] & MSB_Y) {
        pp.delta_y = packet[2] - 256;
    }
    else {
        pp.delta_y = packet[2];
    }
}


struct mouse_ev mouse_detect_event2(struct packet *pp) {
    struct mouse_ev event;
    bool lb = false, rb = false;
    if (pp->lb && !(pp->rb || pp->mb) && !lb) {
        event.type = LB_PRESSED;
        lb = true;
    }
    else if (!(pp->lb || pp->rb || pp->mb) && lb) {
        event.type = LB_RELEASED;
        lb = false;
    }
    else if (pp->rb && !(pp->lb || pp->mb) && !rb) {
        event.type = RB_PRESSED;
        rb = true;
    }
    else if (!(pp->rb || pp->lb || pp->mb) && rb) {
        event.type = RB_RELEASED;
        rb = false;
    }//definir mp
    else if (pp->delta_x && pp->delta_y) {
        event.type = MOUSE_MOV;
    }
    else {
        event.type = BUTTON_EV;
    }
    event.delta_x = pp->delta_x;
    event.delta_y = pp->delta_y;
    return event;
}

/*typedef enum {INIT, DRAW, COMP} state_t;
typedef enum {RDOW, RUP, MOVE} ev_type_t;*/
bool gesture = false;
enum c_state{INIT, RDOWN, VERTEX, RUP, HOR_TOLERANCE};
void mouse_detect_gesture(struct mouse_ev ev, uint8_t x_len, uint8_t tolerance) {
    enum c_state state = INIT;
    uint8_t x_aux = 0;;
    switch (state) {
        case INIT:
            if (ev.type == LB_PRESSED) {
                state = RUP;
                x_aux = 0;
            }
            break;
        case RUP:
            x_aux += ev.delta_x;
            if (ev.type == LB_RELEASED && x_aux >= x_len) {
                state = VERTEX;
            }
            else if (ev.type != MOUSE_MOV) {
                state = INIT;
            }
            else if ((ev.delta_x < -tolerance) || (ev.delta_y < -tolerance)) {
                state = INIT;
            }
            else if (abs(ev.delta_x/ev.delta_x) < 1){
                state = INIT;
            }
            break;
        case VERTEX:
            if (ev.type == RB_PRESSED) {
                x_aux = 0;                
                state = RDOWN;
            }
            else if (ev.type == LB_PRESSED) {
                x_aux = 0;                
                state = RUP;
            }
            else if ((ev.delta_x < -tolerance) || (ev.delta_y < -tolerance) || ev.type == BUTTON_EV) {
                state = INIT;
            }
            break;
        case RDOWN:
            x_aux += ev.delta_x;
            if (ev.type == RB_RELEASED && x_aux >= x_len) {
                gesture = true;
                state = HOR_TOLERANCE;
            }
            else if (ev.type != MOUSE_MOV) {
                state = INIT;
            }
            else if ((ev.delta_x < -tolerance) || (ev.delta_y < -tolerance)) {
                state = INIT;
            }
            else if (abs(ev.delta_x/ev.delta_x) < 1){
                state = INIT;
            }
            break;
        default:
            break;
    }
}


