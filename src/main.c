#include "cpu.h"
#include "flash.h"
#include <stdio.h>

int main() {
  uint8_t *memory = CPU_memnew();
  uint8_t *stack = CPU_membeginstack(memory);
  uint8_t status_register = INITIAL_STATUS_REGISTER_FLAGS;

  uint16_t *fmemory = flash_fmemnew();
  fmemory[0] = 0b0000110000000001; // ADD R0,R1
  uint16_t *pc = flash_getpc(fmemory);

  struct flash_opcode opcode = flash_poppc(&pc);

  flash_fmemdestroy(&fmemory);
  CPU_memdestroy(&memory);

  return 0;
}