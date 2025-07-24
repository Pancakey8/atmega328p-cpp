#include "cpu.h"
#include <stdlib.h>

const uint16_t SRAM_MEMORY_SIZE = 0x800;
const uint8_t INITIAL_STATUS_REGISTER_FLAGS = 0b00000000;

uint8_t *CPU_memgpr(uint8_t *memory, uint16_t n) {
  assert(n <= 0x1F);
  return &memory[n];
}

uint8_t *CPU_memior(uint8_t *memory, uint16_t n) {
  assert(n <= 0x3F);
  return &memory[0x20 + n];
}

uint8_t *CPU_memexior(uint8_t *memory, uint16_t n) {
  assert(n <= 0xA0);
  return &memory[0x60 + n];
}

uint8_t *CPU_memnew() { return calloc(SRAM_MEMORY_SIZE, 1); }

void CPU_memdestroy(uint8_t **memory) {
  free(*memory);
  *memory = NULL;
}

uint8_t *CPU_membeginstack(uint8_t *memory) {
  return &memory[SRAM_MEMORY_SIZE - 1];
}

void CPU_memstackpush(uint8_t **stack, uint8_t value) {
  **stack = value;
  stack--;
}

uint8_t CPU_memstackpop(uint8_t **stack) {
  uint8_t value = **stack;
  **stack = 0x0;
  stack++;
  return value;
}

void CPU_sregset(uint8_t *sreg, enum CPU_status_register flag, uint8_t value) {
  if (value == 1) {
    *sreg |= 1 << flag;
  } else if (value == 0) {
    *sreg &= ~(1 << flag);
  }
}

void CPU_ioregset(uint8_t *memory, enum CPU_io_register ioreg, uint8_t bit,
                  uint8_t value) {
  uint8_t *ioreg_mem = CPU_memior(memory, ioreg);
  if (value == 1) {
    *ioreg_mem |= 1 << bit;
  } else if (value == 0) {
    *ioreg_mem &= ~(1 << bit);
  }
}

uint8_t CPU_sregget(uint8_t sreg, enum CPU_status_register flag) {
  return (sreg & (1 << flag)) >> flag;
}