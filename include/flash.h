#pragma once
#include "cpu.h"
#include <stddef.h>
#include <stdint.h>

// Maximum flash (programmable) memory size
extern const uint16_t FLASH_MEMORY_SIZE;

// Create flash memory
uint16_t *flash_fmemnew();

// Get flash program counter
uint16_t *flash_getpc(uint16_t *fmemory);

// Instructions
enum flash_instruction {
  INST_ADD = 0b0000110000000000,
  INST_ADC = 0b0001110000000000,
  INST_SPECIAL_END = 0xFF
};

// Instruction masks
enum flash_instruction_mask {
  IMASK_ADD = 0b1111110000000000,
  IMASK_ADC = 0b1111110000000000
};

// Opcode data
struct flash_opcode {
  uint16_t param1;
  uint16_t param2;
  enum flash_instruction opcode;
};

// Increment program counter, get operation
struct flash_opcode flash_poppc(uint16_t **pc);

// Execute operation
void flash_runop(struct flash_opcode op, uint8_t *memory,
                 uint8_t *status_register, uint8_t **stack);

// Free flash memory
void flash_fmemdestroy(uint16_t **fmemory);