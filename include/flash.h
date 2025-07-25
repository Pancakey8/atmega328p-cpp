#pragma once
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
  INST_ADD,
  INST_ADC,
  INST_AND,
  INST_ASR,
  INST_JMP,
  INST_CP,
  INST_BREQ,
  INST_BRLT,
  INST_BRGE,
  INST_BRNE,
  INST_BRSH,
  INST_SPECIAL_END = 0xFF
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
                 uint8_t *status_register, uint8_t **stack, uint16_t **pc,
                 uint16_t *fmemory);

// Free flash memory
void flash_fmemdestroy(uint16_t **fmemory);