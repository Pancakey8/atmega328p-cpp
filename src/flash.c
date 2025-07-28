#include "flash.h"

#include <stdbool.h>
#include <stdlib.h>

const uint16_t FLASH_MEMORY_SIZE = 0x8000;

uint16_t *flash_fmemnew() { return calloc(FLASH_MEMORY_SIZE / 2, 2); }

void flash_fmemdestroy(uint16_t **fmemory) {
  free(*fmemory);
  *fmemory = NULL;
}

uint16_t *flash_getpc(uint16_t *fmemory) { return fmemory; }