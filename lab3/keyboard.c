#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "utils.c"



int hook_id, hook_id1;
uint8_t scancode = 0;
int timer_interrupt_counter = 0;


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

int (kbc_poll_h)() {
  uint8_t st;
  if (util_sys_inb(KBC_ST_REG, &st) != OK) {
    printf("Error in sys_inb()\n");
    return 1;
  }     
  if (st & OBFULL) {   
    if (util_sys_inb(KBC_OUT_BUF, &scancode) != OK){
      printf("Error in sys_inb()\n");
      return 1;
    }
    if ((st & AUX) != OK) {
      printf("Bits in satatus register failed\n");
      return 1;
    }   
    if ((st & (PARITY_ERROR | TIMEOUT_ERROR)) != OK) {
      printf("Bits in satatus register failed\n");
      return 1;
    }
    return 0;  
  }
    
 
  return 1;
}


int (kbd_poll_cmd)() {
  uint8_t cmd;
  if (sys_outb(KBC_ST_REG, READ_CMD_B) != OK) {
    printf("\n");
    return 1;
  }
  if (util_sys_inb(WRITE_CMD_B, &cmd) != OK) {
    printf("\n");
    return 1;
  }
  cmd = cmd | BIT(0);
  if (sys_outb(KBC_ST_REG, KBC_OUT_BUF) != OK) {
    printf("\n");
    return 1;
  }
  if (sys_outb(KBC_OUT_BUF, cmd) != OK) {
    printf("\n");
    return 1;
  }
  return 0;
}


void (timer_int_handler)() {
 timer_interrupt_counter++;
}

int (timer_subscribe_int)(uint8_t *bit_no) {

  hook_id1 = TIMER0_IRQ;
  *bit_no = hook_id;  

  printf("before sys_irqsetpolicy: hook_id = 0x%02x\n", hook_id1);

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id1) == OK) {

    printf("after sys_irqsetpolicy: hook_id = 0x%x\n", hook_id1);
    return 0;
  }

  else {
    printf("sys_irqsetpolicy() failed\n");
    return 1;    
  }

}
int (timer_unsubscribe_int)() {

  if (sys_irqrmpolicy(&hook_id1) != OK) {
    printf("sys_irqrmpolicy() failed\n");
    return 1;
  }

  return 0;
}
