// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "mouse.h"
#include "i8042.h"
#include "utils.h"


extern uint8_t packet[3];
uint32_t counter = 0;
extern struct packet pp;
extern uint8_t byte;
extern uint8_t timer_interrupt_counter;
extern bool gesture;


// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {

  int ipc_status;
  message msg;
  uint8_t bit_no;
  int r;
  //int counter = 0;
  if (mouse_enable_data_reporting2(ENABLE) != OK) {
    printf("Error in mouse_enable_data_reporting2()\n");
    return 1;
  }

  if (mouse_subscribe_int(&bit_no) != OK) {
    printf("Error in mouse_subscribe_int()\n");
    return 1;
  }
  uint32_t mouse_int_bit = BIT(bit_no);

  while(cnt > 0) {
 /* Get a request message. */
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.m_notify.interrupts & mouse_int_bit) { /* subscribed interrupt */
          mouse_ih();
          if (counter == 0 && (byte & CHECK)) {
            packet[0] = byte;
            counter++;
          }
          else if (counter == 1) {
            packet[counter] = byte;
            counter++;
          }
          else if (counter == 2) {
            packet[counter] = byte;
            cnt--;
            packet_make();
            mouse_print_packet(&pp);
            counter = 0;
          }
        }
        break;
      default:
        break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
 /* no standard messages expected: do nothing */
    }
  }
  if (mouse_unsubscribe_int() != OK) {
    printf("Error in mouse_unsubscribe_int()\n");
    return 1;
  }
  if (mouse_enable_data_reporting2(DISABLE) != OK) {
    printf("Error in mouse_enable_data_reporting2()\n");
    return 1;
  }

  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {

  uint8_t bit_no_mouse, bit_no_timer;

  if (idle_time < 0) {
    return 1;
  }

  else if (idle_time == 0) {
    return 0;
  }

  if (mouse_enable_data_reporting2(ENABLE) != OK) {
    printf("Error in mouse_enable_data_reporting2()\n");
    return 1;
  }
  if (timer_subscribe_int(&bit_no_timer) != OK) {
    printf("Error in timer_subscribe_int()\n");
    return 1;
  }
  if (mouse_subscribe_int(&bit_no_mouse) != OK) {
    printf("Error in mouse_subscribe_int()\n");
    return 1;
  }



  int ipc_status;
  message msg;
  int r;
  uint32_t mouse_int_bit = BIT(bit_no_mouse);
  uint32_t timer0_int_bit = BIT(bit_no_timer);

  while (timer_interrupt_counter < (idle_time * sys_hz())) { 
 
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d",r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */

        if (msg.m_notify.interrupts & mouse_int_bit) { // KBD int?
          mouse_ih();
          if (counter == 0 && (byte & CHECK)) {
            packet[0] = byte;
            counter++;
          }
          else if (counter == 1) {
            packet[counter] = byte;
            counter++;
          }
          else if (counter == 2) {
            packet[counter] = byte;
            packet_make();
            mouse_print_packet(&pp);
            counter = 0;
          }
          timer_interrupt_counter = 0;
        }

        if (msg.m_notify.interrupts & timer0_int_bit) { // Timer0 int?
          timer_int_handler();
          /*if (timer_interrupt_counter % 60 == 0) {
            timer_print_elapsed_time();
          }*/
        }
        

        break;
      default:
        break; /* no other notifications expected: do nothing */
      }
    }
    else { 
    }
  }

  if (mouse_unsubscribe_int() != OK) {
    printf("Error in timer_unsubscribe_int()\n");
    return 1;
  }

  if (timer_unsubscribe_int() != OK) {
    printf("Error in timer_unsubscribe_int()\n");
    return 1;
  }

  if (mouse_enable_data_reporting2(DISABLE) != OK) {
    printf("Error in mouse_enable_data_reporting2()\n");
    return 1;
  }

  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  int ipc_status; 
  message msg;
  uint8_t bit_no = 0;
  int r;

  if (mouse_subscribe_int(&bit_no) != OK) {
    printf("Error in mouse_subscribe_int()\n");
    return 1;
  }
  if (mouse_enable_data_reporting2(ENABLE) != OK) {
    printf("Error in mouse_enable_data_reporting2()\n");
    return 1;
  }
  uint32_t irq_set = BIT(bit_no);
  while(!gesture) {
 /* Get a request message. */
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
          mouse_ih();
          if (counter == 0 && (byte & CHECK)) {
            packet[0] = byte;
            counter++;
          }
          else if (counter == 1) {
            packet[counter] = byte;
            counter++;
          }
          else if (counter == 2) {
            packet[counter] = byte;
            packet_make();
            mouse_print_packet(&pp);
            struct mouse_ev event= mouse_detect_event2(&pp);
            mouse_detect_gesture(event, x_len, tolerance);
          }
        }
        break;
      default:
        break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
 /* no standard messages expected: do nothing */
    }
  }

    if (mouse_enable_data_reporting2(DISABLE) != OK) {
    printf("Error in mouse_enable_data_reporting2()\n");
    return 1;
  }
  if (mouse_unsubscribe_int() != OK) {
    printf("Error in mouse_unsubscribe_int()\n");
    return 1;
  }
  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* To be completed */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
