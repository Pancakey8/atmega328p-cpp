#include "flash.h"
#include "cpu.h"

#include <stdbool.h>
#include <stdlib.h>

const uint16_t FLASH_MEMORY_SIZE = 0x8000;

uint16_t *flash_fmemnew() { return calloc(FLASH_MEMORY_SIZE / 2, 2); }

void flash_fmemdestroy(uint16_t **fmemory) {
  free(*fmemory);
  *fmemory = NULL;
}

uint16_t *flash_getpc(uint16_t *fmemory) { return fmemory; }

uint16_t opc_gethigh6(uint16_t opc) { return opc >> 10; }

uint16_t opc_getrd(uint16_t opc) { return (opc & 0b0000000111110000) >> 4; }

uint16_t opc_getrr(uint16_t opc) {
  return (opc & 0b0000000000001111) + ((opc & 0b0000001000000000) >> 5);
}

struct flash_opcode flash_poppc(uint16_t **pc) {
  struct flash_opcode retval;

  uint16_t high6 = opc_gethigh6(**pc);
  uint16_t rd = opc_getrd(**pc);
  uint16_t rr = opc_getrr(**pc);

  retval.param1 = rd;
  retval.param2 = rr;

  // Irregulars group 1
  if (high6 == 0b100101) {
    if (rr == 0b00101) {
      retval.opcode = INST_ASR;
    } else if (rr == 0b01100 ||
               rr == 0b01101) { // JMP claims 22-bit K, we only need 14, thus we
                                // can only reference the latter 16 bits. Ignore
                                // rest.
      (*pc)++;
      retval.param1 = **pc;
      retval.opcode = INST_JMP;
    }
    // Irregulars group 2
  } else if (high6 == 0b111100 || high6 == 0b111101) {
    uint8_t branch_code = (rr & 0b111);
    retval.param1 = (**pc >> 3) & 0b1111111;

    if (high6 == 0b111100) {
      switch (branch_code) {
      case 0b001:
        retval.opcode = INST_BREQ;
        break;
      case 0b100:
        retval.opcode = INST_BRLT;
        break;
      }
    } else if (high6 == 0b111101) {
      switch (branch_code) {
      case 0b100:
        retval.opcode = INST_BRGE;
        break;
      case 0b001:
        retval.opcode = INST_BRNE;
        break;
      case 0b000:
        retval.opcode = INST_BRSH;
        break;
      }
    }
  }
  // two param regulars
  else if (high6 == 0b000011) {
    retval.opcode = INST_ADD;
  } else if (high6 == 0b000111) {
    retval.opcode = INST_ADC;
  } else if (high6 == 0b001000) {
    retval.opcode = INST_AND;
  } else if (high6 == 0b000101) {
    retval.opcode = INST_CP;
  }

  if (**pc == INST_SPECIAL_END)
    retval.opcode = INST_SPECIAL_END;

  (*pc)++;
  return retval;
}

void flash_runop(struct flash_opcode op, uint8_t *memory,
                 uint8_t *status_register, uint8_t **stack, uint16_t **pc,
                 uint16_t *fmemory) {
  switch (op.opcode) {
  case INST_ADC:
  case INST_ADD: {
    uint8_t *rd = CPU_memgpr(memory, op.param1);
    uint8_t *rr = CPU_memgpr(memory, op.param2);
    uint16_t res16 = (uint16_t)*rd + (uint16_t)*rr;
    uint8_t R = (uint8_t)(res16 & 0xFF);
    if (op.opcode == INST_ADC) {
      uint8_t C = CPU_sregget(*status_register, SREG_CARRY_FLAG);
      res16 += (uint16_t)C;
    }
    uint8_t rd7 = *rd >> 7;
    uint8_t rr7 = *rr >> 7;
    uint8_t R7 = R >> 7;
    CPU_sregset(status_register, SREG_CARRY_FLAG,
                (rd7 & rr7) | (rr7 & ~R7) | (~R7 & rd7));
    uint8_t rd3 = (*rd >> 3) & 1;
    uint8_t rr3 = (*rr >> 3) % 1;
    uint8_t R3 = (R >> 3) & 1;
    CPU_sregset(status_register, SREG_HALFCARRY_FLAG,
                (rd3 & rr3) | (rr3 & ~R3) | (~R3 & rd3));
    CPU_sregset(status_register, SREG_NEGATIVE_FLAG, R7);
    CPU_sregset(status_register, SREG_ZERO_FLAG, R == 0);
    CPU_sregset(status_register, SREG_TWOSCOMP_OF_FLAG,
                (rd7 & rr7 & ~R7) | (~rd7 & ~rr7 & R7));
    *rd = R;
    break;
  }

  case INST_AND: {
    uint8_t *rd = CPU_memgpr(memory, op.param1);
    uint8_t *rr = CPU_memgpr(memory, op.param2);
    *rd = *rd & *rr;
    CPU_sregset(status_register, SREG_TWOSCOMP_OF_FLAG, 0);
    CPU_sregset(status_register, SREG_ZERO_FLAG, *rd == 0);
    CPU_sregset(status_register, SREG_NEGATIVE_FLAG, *rd >> 7);
    break;
  }

  case INST_ASR: {
    uint8_t *rd = CPU_memgpr(memory, op.param1);
    uint8_t C = *rd & 1;
    CPU_sregset(status_register, SREG_CARRY_FLAG, C);
    uint8_t rd7 = *rd >> 7;
    uint8_t rd_min_7 = *rd & ~(1 << 7);
    *rd = (rd7 << 7) + (rd_min_7 >> 1);
    CPU_sregset(status_register, SREG_ZERO_FLAG, *rd == 0);
    uint8_t N = rd7 == 1;
    CPU_sregset(status_register, SREG_NEGATIVE_FLAG, N);
    CPU_sregset(status_register, SREG_TWOSCOMP_OF_FLAG, C ^ N);
    break;
  }

  case INST_JMP: {
    *pc = &fmemory[op.param1];
    break;
  }

  case INST_CP: {
    uint8_t *rd = CPU_memgpr(memory, op.param1);
    uint8_t *rr = CPU_memgpr(memory, op.param2);
    uint8_t R = *rd - *rr;
    uint8_t R7 = R >> 7;
    uint8_t rd7 = *rd >> 7;
    uint8_t rr7 = *rr >> 7;
    uint8_t rd3 = (*rd >> 3) & 1;
    uint8_t rr3 = (*rr >> 3) % 1;
    uint8_t R3 = (R >> 3) & 1;

    CPU_sregset(status_register, SREG_HALFCARRY_FLAG,
                (~rd3 & rr3) | (rr3 & R3) | (R3 & ~rd3));
    CPU_sregset(status_register, SREG_TWOSCOMP_OF_FLAG,
                (rd7 & ~rr7 & ~R7) | (~rd7 & rr7 & R7));
    CPU_sregset(status_register, SREG_NEGATIVE_FLAG, R7 == 1);
    CPU_sregset(status_register, SREG_ZERO_FLAG, R == 0);
    CPU_sregset(status_register, SREG_CARRY_FLAG,
                (~rd7 & rr7) | (rr7 & R7) | (R7 & ~rd7));
    break;
  }

  case INST_BREQ:
  case INST_BRLT:
  case INST_BRGE:
  case INST_BRNE:
  case INST_BRSH: {
    uint8_t flag;

    switch (op.opcode) {
    case INST_BREQ:
      flag = CPU_sregget(*status_register, SREG_ZERO_FLAG);
      break;
    case INST_BRLT:
      flag = CPU_sregget(*status_register, SREG_SIGNBIT_FLAG);
      break;
    case INST_BRGE:
      flag = !CPU_sregget(*status_register, SREG_SIGNBIT_FLAG);
      break;
    case INST_BRNE:
      flag = !CPU_sregget(*status_register, SREG_ZERO_FLAG);
      break;
    case INST_BRSH:
      flag = !CPU_sregget(*status_register, SREG_CARRY_FLAG);
    default:
      break;
    }

    if (flag == 1) {
      (*pc) += op.param1;
    }
    break;
  }

  default:
    break;
  }

  uint8_t V = CPU_sregget(*status_register, SREG_TWOSCOMP_OF_FLAG);
  uint8_t N = CPU_sregget(*status_register, SREG_NEGATIVE_FLAG);
  CPU_sregset(status_register, SREG_SIGNBIT_FLAG, N ^ V);
}