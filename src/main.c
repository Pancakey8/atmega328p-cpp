#include "cpu.h"
#include "flash.h"

int main() {
  uint8_t *memory = CPU_memnew();
  uint8_t *stack = CPU_membeginstack(memory);
  uint8_t status_register = INITIAL_STATUS_REGISTER_FLAGS;

  uint8_t *r0 = CPU_memgpr(memory, 0);
  *r0 = 0;

  uint8_t *r1 = CPU_memgpr(memory, 1);
  *r1 = 1;

  uint16_t *fmemory = flash_fmemnew();
  fmemory[0] = 0b0000110000000001; // ADD R0,R1
  fmemory[1] = 0b1001010000001100; // JMP..
  fmemory[2] = 0;                  // ..ADDR0
  fmemory[3] = INST_SPECIAL_END;
  uint16_t *pc = flash_getpc(fmemory);

  struct flash_opcode opcode;
  while (1) {
    opcode = flash_poppc(&pc);
    if (opcode.opcode == INST_SPECIAL_END)
      break;
    flash_runop(opcode, memory, &status_register, &stack, &pc, fmemory);
  }

  flash_fmemdestroy(&fmemory);
  CPU_memdestroy(&memory);

  return 0;
}