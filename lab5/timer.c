#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int hook_id1;
int timer_interrupt_counter = 0;


int (timer_subscribe_int)(uint8_t *bit_no) {

  hook_id1 = TIMER0_IRQ;
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

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  
  if (freq < 19 || freq > TIMER_FREQ) {
    printf("Error, the input for freq is invalid\n");
    return 1;
  }

  if (timer < 0 || timer > 2) {
    printf("Error, invalid input in timer\n");
    return 1;
  }

  uint8_t status;
  if (timer_get_conf(timer, &status) != OK) {
    printf("Error in timer_get_conf()\n");
    return 1;
  }

  status = status & 0x0F;
  status = status | TIMER_LSB_MSB;

  uint16_t newfreq = TIMER_FREQ / freq;

  uint8_t nmsb, nlsb;

  util_get_LSB(newfreq, &nlsb);
  util_get_MSB(newfreq, &nmsb);

  switch(timer){
    case 0:
      status = status | TIMER_SEL0;
      break;
    case 1:
      status = status | TIMER_SEL1;
      break;
    case 2:
      status = status | TIMER_SEL2;
      break;
  }

  if (sys_outb(TIMER_CTRL, status) != OK) {    
    printf("Error in sys_outb()\n");
    return 1;
  }

  if (sys_outb(TIMER_0 + timer, nlsb) != OK) {
    printf("Error in sys_outb()\n");
    return 1;
  }

  if (sys_outb(TIMER_0 + timer, nmsb) != OK) {
    printf("Error in sys_outb()\n");
    return 1;
  }

  return 0; 
}

void (timer_int_handler)() {
 timer_interrupt_counter++;
}

