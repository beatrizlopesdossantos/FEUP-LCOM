#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "keyboard.c"



extern uint8_t scancode;
uint8_t size = 0;
uint8_t bytes[2];
extern int timer_interrupt_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t bit_no;
  if (kbd_subscribe_int(&bit_no) != OK) {
    printf("Error in kbd_subscribe_int()\n");
    return 1;
  }
  bool make = true;
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);
  int r;
  bool esq = false;
  while (esq != true) { 
 
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d",r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & irq_set) {
          kbc_ih();
          if (scancode == FIRST_OF_TWO_BYTES) {
            bytes[0] = scancode;
            size = 1;  
          }
          else {
            bytes[size] = scancode;
            size++;
            if ((scancode & BIT(7))) {
              make = false;
            }
            else {
              make = true;
            }
            kbd_print_scancode(make, size, bytes);
            size = 0; 

          } 
          if (scancode == ESQ) {
            esq = true;
          }
        }
        break;
      default:
        break;
      }
    }

    else { 
    }
  }


  if (kbd_unsubscribe_int() != OK) {
    printf("Error in timer_unsubscribe_int()\n");
    return 1;
  }
  
  if (kbd_print_no_sysinb(counter) != OK) {
    return 1;
  }

  return 0;
}
  

int(kbd_test_poll)() {
  counter = 0;
  bool make2;
  int size2 = 0;
  uint8_t bytes2[2];
  scancode = 0;
  while (scancode != ESQ) {
    if (kbc_poll_h() == 0) {
      printf("%x\n", scancode);
      if (scancode == FIRST_OF_TWO_BYTES) {
        bytes2[0] = scancode;
        size2 = 1;
      }
      else {
        bytes2[size] = scancode;
        size2++;
        if ((scancode & BIT(7))) {
          make2 = false;
        }
        else {
          make2 = true;
          }
        kbd_print_scancode(make2, size2, bytes2);
        size2 = 0; 
      }
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }

  if (kbd_poll_cmd() != OK) {
    return 1;
  }

  if (kbd_print_no_sysinb(counter) != OK) {
    return 1;
  }
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t bit_no_kbd, bit_no_timer;
  if (kbd_subscribe_int(&bit_no_kbd) != OK) {
    printf("Error in kbd_subscribe_int()\n");
    return 1;
  }
  if (timer_subscribe_int(&bit_no_timer) != OK) {
    printf("Error in kbd_subscribe_int()\n");
    return 1;
  }
  bool esq = false;
  int ipc_status;
  message msg;
  uint32_t kbd_int_bit = BIT(bit_no_kbd);
  uint32_t timer0_int_bit = BIT(bit_no_timer);
  int r;
  bool make = true;
  while (esq != true && (timer_interrupt_counter < (60*n))) { 
 
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d",r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */


        if (msg.m_notify.interrupts & kbd_int_bit) { // KBD int?
          //counter = 0;
          kbc_ih();
          if (scancode == FIRST_OF_TWO_BYTES) {
            bytes[0] = scancode;
            size = 1;  
          }
          else {
            bytes[size] = scancode;
            size++;
            if ((scancode & BIT(7))) {
              make = false;
            }
            else {
              make = true;
            }
            kbd_print_scancode(make, size, bytes);
            timer_interrupt_counter = 0;
            size = 0; 
          } 
          if (scancode == ESQ) {
            esq = true;
          }
        }
        if (msg.m_notify.interrupts & timer0_int_bit) { // Timer0 int?
          timer_int_handler();
          if (timer_interrupt_counter % 60 == 0) {
            timer_print_elapsed_time();
          }
        }
        
        //timer_interrupt_counter = 0;
        break;
      default:
        break; /* no other notifications expected: do nothing */
      }
    }
    else { 
    }
  }

  if (kbd_unsubscribe_int() != OK) {
    printf("Error in timer_unsubscribe_int()\n");
    return 1;
  }

  if (timer_unsubscribe_int() != OK) {
    printf("Error in timer_unsubscribe_int()\n");
    return 1;
  }

  if (kbd_print_no_sysinb(counter) != OK) {
    return 1;
  }

  return 0;
}
