#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8042.h"

uint8_t timer_interrupt_counter = 0;
int hook_id1 =TIMER0_IRQ;
int (timer_subscribe_int)(uint8_t *bit_no) {

  *bit_no = hook_id1;  

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

void (timer_int_handler)() {
 timer_interrupt_counter++;
}

