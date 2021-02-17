#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "utils.c"



int hook_id;
uint8_t scancode = 0;


int (kbd_subscribe_int)(uint8_t *bit_no) {
    hook_id = KBC_IRQ;
    *bit_no = hook_id;  
    printf("before sys_irqsetpolicy: hook_id = 0x%02x\n", hook_id);
    if (sys_irqsetpolicy(KBC_IRQ ,IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) == OK) {

        printf("after sys_irqsetpolicy: hook_id = 0x%x\n", hook_id);
        return 0;
    }
    else {
        printf("sys_irqsetpolicy() failed\n");
        return 1;    
    }

}

int (kbd_unsubscribe_int)() {
    if (sys_irqrmpolicy(&hook_id) != OK) {
        printf("sys_irqrmpolicy() failed\n");
        return 1;
    }
    return 0;
}

void (kbc_ih)() {
    uint8_t st;
    if (util_sys_inb(KBC_ST_REG, &st) != OK) {
        printf("Error in sys_inb()\n");
        return;
    }     
    if (st & OBFULL) {   
      if (util_sys_inb(KBC_OUT_BUF, &scancode) != OK){
        printf("Error in sys_inb()\n");
        return;
      }
      if ((st & AUX) != OK) {
        printf("Bits in satatus register failed\n");
        return;
      }   
      if ((st & (PARITY_ERROR | TIMEOUT_ERROR)) != OK) {
        printf("Bits in satatus register failed\n");
        return;
      }  

    }
}
