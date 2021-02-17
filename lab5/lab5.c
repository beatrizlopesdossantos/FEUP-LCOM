// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include "video.h"
#include "keyboard.h"
#include "i8042.h"
#include "timer.h"
#include "i8042.h"
#include "sprite.h"

extern uint8_t scancode;
extern uint16_t mode_aux;
uint8_t size = 0;
uint8_t bytes[2];
// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (vg_init(mode) == NULL) {
    printf("Error in vg_init()\n");
    return 1;
  }

  sleep(delay);
  vg_exit();
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {

  uint8_t bit_no;
  if (kbd_subscribe_int(&bit_no) != OK) {
    printf("Error in kbd_subscribe_int()\n");
    return 1;
  }

  if (vg_init(mode) == NULL) {
    printf("Error in vg_init()\n");
    return 1;
  }
  bool make = true;
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);
  int r;
  bool esq = false;
  mode_aux = mode;
  printf("mode: %x\n", mode_aux);
  vg_draw_rectangle1(x, y, width, height, color);
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
  
  vg_exit();

  return 0;
}



int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {

  uint8_t bit_no;
  if (kbd_subscribe_int(&bit_no) != OK) {
    printf("Error in kbd_subscribe_int()\n");
    return 1;
  }

  if (vg_init(mode) == NULL) {
    printf("Error in vg_init()\n");
    return 1;
  }
  bool make = true;
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);
  int r;
  bool esq = false;
  vg_draw_pattern(mode, no_rectangles, first, step);
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
  
  vg_exit();

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  uint8_t bit_no;
  if (kbd_subscribe_int(&bit_no) != OK) {
    printf("Error in kbd_subscribe_int()\n");
    return 1;
  }

  if (vg_init(0x105) == NULL) {
    printf("Error in vg_init()\n");
    return 1;
  }
  bool make = true;
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);
  int r;
  bool esq = false;
  vg_draw_pixmap(xpm, x, y);
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
  
  vg_exit();

  return 0;


}

extern int timer_interrupt_counter;
int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  
  uint8_t bit_no_kbd, bit_no_timer;
  if (kbd_subscribe_int(&bit_no_kbd) != OK) {
    printf("Error in kbd_subscribe_int()\n");
    return 1;
  }

  if (timer_subscribe_int(&bit_no_timer) != OK) {
    printf("Error in kbd_subscribe_int()\n");
    return 1;
  }

  void *video_mem = vg_init(0x105);

  int xspeed = 0, yspeed = 0;
  int num_frames = 60 / fr_rate;

  Sprite* sprite = create_sprite(xpm, xi, yi, xspeed, yspeed);
   

  bool esq = false;
  int ipc_status;
  message msg;
  uint32_t kbd_int_bit = BIT(bit_no_kbd);
  uint32_t timer0_int_bit = BIT(bit_no_timer);
  int r, cnt = 0;
  bool make = true;
  while (esq != true) { 
 
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
            size = 0; 
          } 
          if (scancode == ESQ) {
            esq = true;
          }
        }
        if (msg.m_notify.interrupts & timer0_int_bit) {
          cnt++;
          if (cnt % num_frames == 0) {
            //if (((xi % h_res) == xf)) {

            //}
            if (speed < 0) {
              timer_int_handler();
              if (timer_interrupt_counter == abs(speed)) {
                memset(video_mem, 0, get_bytes() * get_h_res() * get_v_res());
                if (xi == xf) {
                  if (yi < yf) {
                    yi -= 1;
                  }
                  else {
                    yi += 1;
                  }
                }
                else if (yi == yf) {
                  if (xi < xf) {
                    xi -= 1;
                  }
                  else {
                    xi += 1;
                  }
                }
                sprite->x = xi;
                sprite->y = yi;
                draw_sprite(sprite);
                timer_interrupt_counter = 0;
              }
            }
            else if (speed > 0) {
              memset(video_mem, 0, get_bytes() * get_h_res() * get_v_res());
                if (xi == xf) {
                  if (abs(yf-yi) < speed) {
                    speed = abs(yf-yi);
                  }
                  if (yi < yf) {
                    yi -= speed;
                  }
                  else {
                    yi += speed;
                  }
                }
                else if (yi == yf) {
                  if (abs(xf-xi) < speed) {
                    speed = abs(xf-xi);
                  }
                  if (xi < xf) {
                    xi -= speed;
                  }
                  else {
                    xi += speed;
                  }
                }
                sprite->x = xi;
                sprite->y = yi;
                draw_sprite(sprite);
              }
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
  destroy_sprite(sprite);
  if (kbd_unsubscribe_int() != OK) {
    printf("Error in timer_unsubscribe_int()\n");
    return 1;
  }

  if (timer_unsubscribe_int() != OK) {
    printf("Error in timer_unsubscribe_int()\n");
    return 1;
  }

  vg_exit();

  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
