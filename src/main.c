#include "cpu.h"
#include "flash.h"
#include <stdio.h>

int main() {
  uint8_t *memory = CPU_memnew();
  uint8_t *stack = CPU_membeginstack(memory);
  uint8_t status_register = INITIAL_STATUS_REGISTER_FLAGS;

  uint8_t *r0 = CPU_memgpr(memory, 0);
  *r0 = 0;

  uint8_t *r1 = CPU_memgpr(memory, 1);
  *r1 = 1;

  uint8_t *r2 = CPU_memgpr(memory, 2);
  *r2 = 10;

  uint16_t *fmemory = flash_fmemnew();
  // CP r0,r2
  // BRLT +1
  // END
  // ADD r0,r1
  // JMP 0
  fmemory[0] = 0b0001010000000010;
  fmemory[1] = 0b1111000000001100;
  fmemory[2] = INST_SPECIAL_END;
  fmemory[3] = 0b0000110000000001;
  fmemory[4] = 0b1001010000001100;
  fmemory[5] = 0b0000000000000000;
  uint16_t *pc = flash_getpc(fmemory);

  struct flash_opcode opcode;
  while (1) {
    opcode = flash_poppc(&pc);
    if (opcode.opcode == INST_SPECIAL_END)
      break;
    flash_runop(opcode, memory, &status_register, &stack, &pc, fmemory);
    printf("%d\n", *r0);
  }

  flash_fmemdestroy(&fmemory);
  CPU_memdestroy(&memory);

  return 0;
}