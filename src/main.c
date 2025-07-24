#include "cpu.h"
#include "flash.h"
#include <stdio.h>

int main() {
  uint8_t *memory = CPU_memnew();
  uint8_t *stack = CPU_membeginstack(memory);
  uint8_t status_register = INITIAL_STATUS_REGISTER_FLAGS;

  // 1080 = 0x0438
  uint8_t *r0 = CPU_memgpr(memory, 0);
  *r0 = 0x38;

  uint8_t *r1 = CPU_memgpr(memory, 1);
  *r1 = 0x04;

  // 1060 = 0x0424
  uint8_t *r2 = CPU_memgpr(memory, 2);
  *r2 = 0x24;

  uint8_t *r3 = CPU_memgpr(memory, 3);
  *r3 = 0x04;

  uint16_t *fmemory = flash_fmemnew();
  fmemory[0] = 0b0000110000000010; // ADD R0,R2
  fmemory[1] = 0b0001110000010011; // ADC R1,R3
  fmemory[2] = 0xFF;
  uint16_t *pc = flash_getpc(fmemory);

  struct flash_opcode opcode;
  while (1) {
    opcode = flash_poppc(&pc);
    if (opcode.opcode == INST_SPECIAL_END)
      break;
    flash_runop(opcode, memory, &status_register, &stack);
  }

  printf("r1:r0=%d, C=%d\n", ((uint16_t)*r1 << 8) + (uint16_t)*r0,
         CPU_sregget(status_register, SREG_CARRY_FLAG));

  flash_fmemdestroy(&fmemory);
  CPU_memdestroy(&memory);

  return 0;
}