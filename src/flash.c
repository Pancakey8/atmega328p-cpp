#include "flash.h"

#include <stdlib.h>

const uint16_t FLASH_MEMORY_SIZE = 0x8000;

uint16_t *flash_fmemnew() { return calloc(FLASH_MEMORY_SIZE / 2, 2); }

void flash_fmemdestroy(uint16_t **fmemory) {
  free(*fmemory);
  *fmemory = NULL;
}

uint16_t *flash_getpc(uint16_t *fmemory) { return fmemory; }

struct flash_opcode flash_poppc(uint16_t **pc) {
  pc++;

  if ((**pc & IMASK_ADD) == INST_ADD) {
    float rd = (**pc & 0b0000000111110000) >> 4;
    float rr = (**pc & 0b0000000000001111) + ((**pc & 0b0000001000000000) >> 5);

    return (struct flash_opcode){
        .param1 = rd, .param2 = rr, .opcode = INST_ADD};
  }

  return (struct flash_opcode){};
}