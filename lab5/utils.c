#include <lcom/lcf.h>
#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb == NULL)
    return 1;

  *lsb = (uint8_t) val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb == NULL)
    return 1;

  *msb = (uint8_t) ((val >> 8) & 0x00FF);
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t v;

  if (sys_inb(port, &v) != OK) {
    printf("Error in sys_inb()\n");
    return 1;
  }
  
  *value = v;

  return 0;
}
