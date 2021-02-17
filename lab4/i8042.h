#ifndef _LCOM_I8254_H_
#define _LCOM_I8254_H_
#include <minix/sysutil.h>

#include <lcom/lcf.h>

#define TIMER0_IRQ 0  

#define Y_OVFL  BIT(7)
#define X_OVFL  BIT(6)         
#define MSB_Y   BIT(5)            
#define MSB_X   BIT(4)           
#define CHECK   BIT(3)         
#define MB      BIT(2)
#define RB      BIT(1)
#define LB      BIT(0)

#define ACK         0xFA
#define NACK        0xFE

#define OUT_BUF     0x60

#define KBC_ST_REG  0x64
#define KBC_CMD_REG 0x64
#define KBC_OUT_BUF 0x60

#define MOUSE_IRQ 12
#define FIRST_OF_TWO_BYTES 0xE0

#define READ_CMD_B          0x20    /* Reads command byte */
#define WRITE_CMD_B         0x60    /* Writes command byte (must pass command byte as argument to port 0x60) */
#define DISABLE_MOUSE       0xA7    /* Disable mouse */
#define ENABLE_MOUSE        0xA8    /* Enable mouse */
#define CHECK_MOUSE_IN      0xA9    /* Check mouse interface */
#define WRITE_BYTE_MOUSE    0xD4    /* Writes byte to mouse */

#define ENABLE      0xF4
#define DISABLE     0xF5
#define SET_STREAM  0xEA

#define ACK         0xFA
#define NACK        0xFE
#define ERROR       0xFC

#define OBF             BIT(0)
#define IBF             BIT(1)
#define PAR_ERROR       BIT(7)
#define TIMEOUT_ERROR   BIT(6)

#define DELAY_US    20000

#endif //I8042_H
