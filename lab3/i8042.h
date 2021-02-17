#ifndef _LCOM_I8254_H_
#define _LCOM_I8254_H_
#include <minix/sysutil.h>

#include <lcom/lcf.h>



#define TIMER_SEL0      0x00              
#define PARITY_ERROR    BIT(7)            
#define TIMEOUT_ERROR   BIT(6)           
#define AUX             BIT(5)         
#define IBFULL          BIT(1)
#define OBFULL          BIT(0)

#define KBC_ST_REG  0x64
#define KBC_CMD_REG 0x64
#define KBC_OUT_BUF 0x60

#define KBC_IRQ 1
#define ESQ                0x81
#define FIRST_OF_TWO_BYTES 0xE0

#define READ_CMD_B   0x20    /* Reads command byte */
#define WRITE_CMD_B  0x60    /* Writes command byte (must pass command byte as argument to port 0x60) */
#define CHECK_KBC    0xAA    /* Self-test: returns 0x55 if OK, returns 0xFC if error */
#define CHECK_KB_IFC 0xAB    /* Check Keyboard interface : returns 0 if OK */
#define DISABLE_KBD  0xAD    /* Disables Keyboard interface */
#define ENABLE_KBS   0xAE    /* Enables Keyboard interface */


#define DELAY_US    20000

#define TIMER0_IRQ 0

#endif //I8042_H
