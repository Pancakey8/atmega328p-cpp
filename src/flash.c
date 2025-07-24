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
  struct flash_opcode opcode;

  if ((**pc & IMASK_ADD) == INST_ADD) {
    float rd = (**pc & 0b0000000111110000) >> 4;
    float rr = (**pc & 0b0000000000001111) + ((**pc & 0b0000001000000000) >> 5);

    opcode =
        (struct flash_opcode){.param1 = rd, .param2 = rr, .opcode = INST_ADD};
  } else if ((**pc & IMASK_ADC) == INST_ADC) {
    float rd = (**pc & 0b0000000111110000) >> 4;
    float rr = (**pc & 0b0000000000001111) + ((**pc & 0b0000001000000000) >> 5);

    opcode =
        (struct flash_opcode){.param1 = rd, .param2 = rr, .opcode = INST_ADC};
  } else if (**pc == INST_SPECIAL_END) {
    opcode = (struct flash_opcode){.opcode = INST_SPECIAL_END};
  }

  (*pc)++;
  return opcode;
}

void flash_runop(struct flash_opcode op, uint8_t *memory,
                 uint8_t *status_register, uint8_t **stack) {
  switch (op.opcode) {
  case INST_ADD: {
    uint8_t *rd = CPU_memgpr(memory, op.param1);
    uint8_t *rs = CPU_memgpr(memory, op.param2);
    uint8_t rd7 = *rd >> 7;
    uint8_t rs7 = *rs >> 7;
    uint16_t res16 = (uint16_t)*rd + (uint16_t)*rs;
    uint8_t R = (uint8_t)(res16 & 0xFF);
    uint8_t R7 = R >> 7;
    CPU_sregset(status_register, SREG_CARRY_FLAG,
                (rd7 & rs7) | (rs7 & ~R7) | (~R7 & rd7));
    *rd = R;
    break;
  }
  case INST_ADC: {
    uint8_t *rd = CPU_memgpr(memory, op.param1);
    uint8_t *rs = CPU_memgpr(memory, op.param2);
    uint8_t rd7 = *rd >> 7;
    uint8_t rs7 = *rs >> 7;
    uint8_t C = CPU_sregget(*status_register, SREG_CARRY_FLAG);
    uint16_t res16 = (uint16_t)*rd + (uint16_t)*rs + (uint16_t)C;
    uint8_t R = (uint8_t)(res16 & 0xFF);
    uint8_t R7 = R >> 7;
    CPU_sregset(status_register, SREG_CARRY_FLAG,
                (rd7 & rs7) | (rs7 & ~R7) | (~R7 & rd7));
    *rd = R;
    break;
  }

  default:
    break;
  }
}