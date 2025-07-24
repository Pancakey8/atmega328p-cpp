#pragma once

#include <assert.h>
#include <stdint.h>

// Max. SRAM memory size
extern const uint16_t SRAM_MEMORY_SIZE;

// Default status register value
extern const uint8_t INITIAL_STATUS_REGISTER_FLAGS;

// General purpose registers (32)
uint8_t *CPU_memgpr(uint8_t *memory, uint16_t n);

// I/O registers (64)
uint8_t *CPU_memior(uint8_t *memory, uint16_t n);

// I/O register values
enum CPU_io_register {
  IO_PINB = 0x3,
  IO_DDRB,
  IO_PORTB,
  IO_PINC,
  IO_DDRC,
  IO_PORTC,
  IO_PIND,
  IO_DDRD,
  IO_PORTD
};

// I/O set register value
void CPU_ioregset(uint8_t *memory, enum CPU_io_register ioreg, uint8_t bit,
                  uint8_t value);

// Extended I/O registers (160)
uint8_t *CPU_memexior(uint8_t *memory, uint16_t n);

// Create memory
uint8_t *CPU_memnew();

// Free memory
void CPU_memdestroy(uint8_t **memory);

// Setup stack
uint8_t *CPU_membeginstack(uint8_t *memory);

// Push to stack
void CPU_memstackpush(uint8_t **stack, uint8_t value);

// Pop from stack, return value
uint8_t CPU_memstackpop(uint8_t **stack);

// Status register
enum CPU_status_register {
  SREG_CARRY_FLAG,
  SREG_ZERO_FLAG,
  SREG_NEGATIVE_FLAG,
  SREG_TWOSCOMP_OF_FLAG,
  SREG_SIGNBIT_FLAG,
  SREG_HALFCARRY_FLAG,
  SREG_BITCOPYSTORAGE,
  SREG_GLOBINT_ENABLE
};

// Set flag value 1 or 0
void CPU_sregset(uint8_t *sreg, enum CPU_status_register flag, uint8_t value);

// Get flag value
uint8_t CPU_sregget(uint8_t sreg, enum CPU_status_register flag);