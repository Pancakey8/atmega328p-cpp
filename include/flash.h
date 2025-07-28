#pragma once
#include <stddef.h>
#include <stdint.h>

// Maximum flash (programmable) memory size
extern const uint16_t FLASH_MEMORY_SIZE;

// Create flash memory
uint16_t *flash_fmemnew();

// Get flash program counter
uint16_t *flash_getpc(uint16_t *fmemory);

// Free flash memory
void flash_fmemdestroy(uint16_t **fmemory);