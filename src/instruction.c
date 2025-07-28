#include "instruction.h"
#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *instruction_patterns[] = {
    [INST_ADC] = "0001 11rd dddd rrrr",   // Add with Carry
    [INST_ADD] = "0000 11rd dddd rrrr",   // Add
    [INST_ADIW] = "1001 0110 KKdd KKKK",  // Add Immediate to Word
    [INST_AND] = "0010 00rd dddd rrrr",   // Logical AND
    [INST_ANDI] = "0111 KKKK dddd KKKK",  // AND with Immediate
    [INST_ASR] = "1001 010d dddd 0101",   // Arithmetic Shift Right
    [INST_BCLR] = "1001 0100 1sss 1000",  // Bit Clear in SREG
    [INST_BLD] = "1111 100d dddd 0bbb",   // Bit Load from T
    [INST_BRD] = "",                      // Alias for BLD (not used)
    [INST_BRBC] = "1111 0kkk kkkk ksss",  // Branch if Bit in SREG Cleared
    [INST_BRBS] = "1111 1kkk kkkk ksss",  // Branch if Bit in SREG Set
    [INST_BRCC] = "",                     // Alias for BRSH
    [INST_BRCS] = "",                     // Alias for BRLO
    [INST_BREAK] = "1001 0101 1001 1000", // Break
    [INST_BREQ] = "1111 00kk kkkk k001",  // Branch if Equal
    [INST_BRGE] = "1111 01kk kkkk k100",  // Branch if Greater or Equal
    [INST_BRHC] = "1111 01kk kkkk k101",  // Branch if Half Carry Cleared
    [INST_BRHS] = "1111 00kk kkkk k101",  // Branch if Half Carry Set
    [INST_BRID] = "1111 01kk kkkk k111",  // Branch if Global Interrupt Disabled
    [INST_BRIE] = "1111 00kk kkkk k111",  // Branch if Global Interrupt Enabled
    [INST_BRLT] = "1111 00kk kkkk k100",  // Branch if Less Than
    [INST_BRMI] = "1111 00kk kkkk k010",  // Branch if Minus
    [INST_BRNE] = "1111 01kk kkkk k001",  // Branch if Not Equal
    [INST_BRPL] = "1111 01kk kkkk k010",  // Branch if Plus
    [INST_BRSH] = "1111 01kk kkkk k000",  // Branch if Same or Higher
    [INST_BRTC] = "1111 01kk kkkk k110",  // Branch if T Flag Cleared
    [INST_BRTS] = "1111 00kk kkkk k110",  // Branch if T Flag Set
    [INST_BRVC] = "1111 01kk kkkk k011",  // Branch if Overflow Cleared
    [INST_BRVS] = "1111 00kk kkkk k011",  // Branch if Overflow Set
    [INST_BSET] = "1001 0100 0sss 1000",  // Bit Set in SREG
    [INST_BST] = "1111 101d dddd 0bbb",   // Bit Store from Bit in Register
    [INST_CALL] = "1001 010k kkkk 111k",  // Call (32-bit)
    [INST_CBI] = "1001 1000 AAAA Abbb",   // Clear Bit in I/O Register
    [INST_CLC] = "",                      // Alias for BCLR 0
    [INST_CLH] = "",                      // Alias for BCLR 5
    [INST_CLI] = "",                      // Alias for BCLR 7
    [INST_CLN] = "",                      // Alias for BCLR 1
    [INST_CLR] = "",                      // Alias for EOR Rd, Rd
    [INST_CLS] = "",                      // Alias for BCLR 2
    [INST_CLT] = "",                      // Alias for BCLR 6
    [INST_CLV] = "",                      // Alias for BCLR 3
    [INST_CLZ] = "",                      // Alias for BCLR 4
    [INST_COM] = "1001 010d dddd 0000",   // One's Complement
    [INST_CP] = "0001 01rd dddd rrrr",    // Compare
    [INST_CPC] = "0000 01rd dddd rrrr",   // Compare with Carry
    [INST_CPI] = "0011 KKKK dddd KKKK",   // Compare with Immediate
    [INST_CPSE] = "0001 00rd dddd rrrr",  // Compare, Skip if Equal
    [INST_DEC] = "1001 010d dddd 1010",   // Decrement
    [INST_DES] = "1001 0100 KKKK 1011",   // Data Encryption Standard
    [INST_ELPM] = "1001 0101 1000 0110",  // Extended Load Program Memory to R0
    [INST_ELPMZ] = "1001 000d dddd 0110", // ELPM Rd, Z
    [INST_ELPMZP] = "1001 000d dddd 0111", // ELPM Rd, Z+
    [INST_EOR] = "0010 01rd dddd rrrr",    // Exclusive OR
    [INST_FMUL] = "0000 0011 dddd rrrr",   // Fractional Multiply Unsigned
    [INST_FMULS] = "0000 0011 dddd rrrr",  // Fractional Multiply Signed
    [INST_FMULSU] =
        "0000 0011 dddd rrrr", // Fractional Multiply Signed w/ Unsigned
    [INST_ICALL] = "1001 0101 0000 1001",  // Indirect Call (Z)
    [INST_IJMP] = "1001 0100 0000 1001",   // Indirect Jump (Z)
    [INST_IN] = "1011 0AAd dddd AAAA",     // Load from I/O
    [INST_INC] = "1001 010d dddd 0011",    // Increment
    [INST_JMP] = "1001 010k kkkk 110k",    // Jump (32-bit)
    [INST_LAC] = "1001 001r rrrr 0110",    // Load and Clear
    [INST_LAS] = "1001 001r rrrr 0101",    // Load and Set
    [INST_LAT] = "1001 001r rrrr 0111",    // Load and Toggle
    [INST_LD_X] = "1000 000d dddd 1100",   // Load from X
    [INST_LD_XP] = "1000 000d dddd 1101",  // Load from X, post-increment
    [INST_LD_MX] = "1000 000d dddd 1110",  // Load from X, pre-decrement
    [INST_LD_Y] = "1000 000d dddd 1000",   // Load from Y
    [INST_LD_YP] = "1000 000d dddd 1001",  // Load from Y, post-increment
    [INST_LD_MY] = "1000 000d dddd 1010",  // Load from Y, pre-decrement
    [INST_LDD_YQ] = "10q0 q0dd dddd 1qqq", // Load from Y + q (displacement)
    [INST_LD_Z] = "1000 000d dddd 0000",   // Load from Z
    [INST_LD_ZP] = "1000 000d dddd 0001",  // Load from Z, post-increment
    [INST_LD_MZ] = "1000 000d dddd 0010",  // Load from Z, pre-decrement
    [INST_LDD_ZQ] = "10q0 q0dd dddd 0qqq", // Load from Z + q (displacement)
    [INST_LDI] = "1110 KKKK dddd KKKK",    // Load Immediate
    [INST_LDS] = "1001 000d dddd 0000",    // Load Direct from SRAM (32-bit)
    [INST_LPM] = "1001 0101 1000 0100",    // Load Program Memory to R0
    [INST_LPMZ] = "1001 000d dddd 0100",   // LPM Rd, Z
    [INST_LPMZP] = "1001 000d dddd 0101",  // LPM Rd, Z+
    [INST_LSL] = "",                       // Alias for ADD Rd, Rd
    [INST_LSR] = "1001 010d dddd 0110",    // Logical Shift Right
    [INST_MOV] = "0010 11rd dddd rrrr",    // Copy Register
    [INST_MOVW] = "0000 0001 dddd rrrr",   // Copy Register Word
    [INST_MUL] = "1001 11rd dddd rrrr",    // Multiply Unsigned
    [INST_MULS] = "0000 0010 dddd rrrr",   // Multiply Signed
    [INST_MULSU] = "0000 0011 dddd rrrr",  // Multiply Signed with Unsigned
    [INST_NEG] = "1001 010d dddd 0001",    // Two’s Complement
    [INST_NOP] = "0000 0000 0000 0000",    // No Operation
    [INST_OR] = "0010 10rd dddd rrrr",     // Logical OR
    [INST_ORI] = "0110 KKKK dddd KKKK",    // OR with Immediate
    [INST_OUT] = "1011 1AAr rrrr AAAA",    // Store to I/O
    [INST_POP] = "1001 000d dddd 1111",    // Pop from Stack
    [INST_PUSH] = "1001 001d dddd 1111",   // Push to Stack
    [INST_RCALL] = "1101 kkkk kkkk kkkk",  // Relative Call
    [INST_RET] = "1001 0101 0000 1000",    // Return from Subroutine
    [INST_RETI] = "1001 0101 0001 1000",   // Return from Interrupt
    [INST_RJMP] = "1100 kkkk kkkk kkkk",   // Relative Jump
    [INST_ROL] = "",                       // Alias for ADD Rd, Rd + Carry
    [INST_ROR] = "1001 010d dddd 0111",    // Rotate Right through Carry
    [INST_SBC] = "0000 10rd dddd rrrr",    // Subtract with Carry
    [INST_SBCI] = "0100 KKKK dddd KKKK",   // Subtract Immediate with Carry
    [INST_SBI] = "1001 1010 AAAA Abbb",    // Set Bit in I/O Register
    [INST_SBIC] = "1111 110A AAAA Abbb",   // Skip if Bit in I/O Cleared
    [INST_SBIS] = "1111 111A AAAA Abbb",   // Skip if Bit in I/O Set
    [INST_SBIW] = "1001 0111 KKdd KKKK",   // Subtract Immediate from Word
    [INST_SBR] = "",                       // Alias for ORI
    [INST_SBRC] = "1111 110r rrrr 0bbb",   // Skip if Bit in Register Cleared
    [INST_SBRS] = "1111 111r rrrr 0bbb",   // Skip if Bit in Register Set
    [INST_SEC] = "",                       // Alias for BSET 0
    [INST_SEH] = "",                       // Alias for BSET 5
    [INST_SEI] = "",                       // Alias for BSET 7
    [INST_SEN] = "",                       // Alias for BSET 1
    [INST_SER] = "",                       // Alias for LDI Rd, 0xFF
    [INST_SES] = "",                       // Alias for BSET 2
    [INST_SET] = "",                       // Alias for BSET 6
    [INST_SEV] = "",                       // Alias for BSET 3
    [INST_SEZ] = "",                       // Alias for BSET 4
    [INST_SLEEP] = "1001 0101 1000 1000",  // Sleep
    [INST_SPM] = "1001 0101 1110 1000",    // Store to Program Memory
    [INST_ST_X] = "1000 001r rrrr 1100",   // Store to X
    [INST_ST_XP] = "1000 001r rrrr 1101",  // Store to X, post-increment
    [INST_ST_MX] = "1000 001r rrrr 1110",  // Store to X, pre-decrement
    [INST_ST_Y] = "1000 001r rrrr 1000",   // Store to Y
    [INST_ST_YP] = "1000 001r rrrr 1001",  // Store to Y, post-increment
    [INST_ST_MY] = "1000 001r rrrr 1010",  // Store to Y, pre-decrement
    [INST_STD_YQ] = "10q0 q1rr rrrr 1qqq", // Store to Y + q
    [INST_ST_Z] = "1000 001r rrrr 0000",   // Store to Z
    [INST_ST_ZP] = "1000 001r rrrr 0001",  // Store to Z, post-increment
    [INST_ST_MZ] = "1000 001r rrrr 0010",  // Store to Z, pre-decrement
    [INST_STD_ZQ] = "10q0 q1rr rrrr 0qqq", // Store to Z + q
    [INST_STS] = "1001 001r rrrr 0000",    // Store Direct to SRAM (32-bit)
    [INST_SUB] = "0001 10rd dddd rrrr",    // Subtract
    [INST_SUBI] = "0101 KKKK dddd KKKK",   // Subtract Immediate
    [INST_SWAP] = "1001 010d dddd 0010",   // Swap Nibbles
    [INST_TST] = "",                       // Alias for AND Rd, Rd
    [INST_WDR] = "1001 0101 1010 1000",    // Watchdog Reset
    [INST_XCH] = "1000 001r rrrr 0100",    // Exchange
};

struct instruction instruction_unmask(uint16_t instruction, char *pattern) {
  struct instruction instr = {.opcode = 0, .param1 = 0, .param2 = 0};
  char param1 = 0, param2 = 0;

  uint8_t bitpos = 15;
  for (size_t i = 0; i < strlen(pattern); ++i) {
    if (pattern[i] == ' ')
      continue;

    if (pattern[i] == '0' || pattern[i] == '1') {
      instr.opcode |= (pattern[i] - '0') << bitpos;
    } else {
      if (!param1 || param1 == pattern[i]) {
        param1 = pattern[i];
        instr.param1 = (instr.param1 << 1) | (instruction >> bitpos & 1);
      } else if (!param2 || param2 == pattern[i]) {
        param2 = pattern[i];
        instr.param2 = (instr.param2 << 1) | (instruction >> bitpos & 1);
      } else {
        // Only reachable if instruction code is messed up...
        perror("PARAMS ARE FCKED");
        abort();
      }
    }
    --bitpos;
  }

  return instr;
}

uint16_t instruction_mask(struct instruction instruction, char *pattern) {
  uint16_t bitcode = 0;

  char param1 = '\0';
  uint8_t param1_size = 0;
  char param2 = '\0';
  uint8_t param2_size = 0;

  size_t pattern_len = strlen(pattern);

  uint8_t bitpos = 15;
  for (size_t i = 0; i < pattern_len; ++i) {
    if (pattern[i] == ' ')
      continue;

    if (pattern[i] == '0' || pattern[i] == '1') {
      bitcode |= (pattern[i] - '0') << bitpos;
    } else {
      if (!param1 || pattern[i] == param1) {
        param1 = pattern[i];
        ++param1_size;
      } else if (!param2 || pattern[i] == param2) {
        param2 = pattern[i];
        ++param2_size;
      } else {
        perror("PARAMS ARE FCKED");
        abort();
      }
    }
    --bitpos;
  }

  size_t param1_pos = param1_size - 1;
  size_t param2_pos = param2_size - 1;
  bitpos = 15;
  for (size_t i = 0; i < pattern_len; ++i) {
    if (pattern[i] == ' ')
      continue;
    if (pattern[i] == '0' || pattern[i] == '1') {
      --bitpos;
      continue;
    }
    if (param1 == pattern[i]) {
      bitcode |= (instruction.param1 >> param1_pos & 1) << bitpos;
      --param1_pos;
    } else { // param2
      bitcode |= (instruction.param2 >> param2_pos & 1) << bitpos;
      --param2_pos;
    }
    --bitpos;
  }

  return bitcode;
}

int instruction_pattern_match(uint16_t instruction, char *pattern) {
  uint16_t opcode = 0;

  char param1 = '\0';
  uint8_t param1_size = 0;
  char param2 = '\0';
  uint8_t param2_size = 0;

  size_t pattern_len = strlen(pattern);

  uint8_t bitpos = 15;
  for (size_t i = 0; i < pattern_len; ++i) {
    if (pattern[i] == ' ')
      continue;

    if (pattern[i] == '0' || pattern[i] == '1') {
      opcode |= (pattern[i] - '0') << bitpos;
    } else {
      if (!param1 || pattern[i] == param1) {
        param1 = pattern[i];
        ++param1_size;
      } else if (!param2 || pattern[i] == param2) {
        param2 = pattern[i];
        ++param2_size;
      } else {
        perror("PARAMS ARE FCKED");
        abort();
      }
    }
    --bitpos;
  }

  bitpos = 15;
  size_t param1_pos = param1_size;
  size_t param2_pos = param2_size;
  for (size_t i = 0; i < pattern_len; ++i) {
    if (pattern[i] == ' ')
      continue;
    if (pattern[i] == '0' || pattern[i] == '1') {
      uint8_t inst_bit = instruction >> bitpos & 1;
      if (inst_bit != pattern[i] - '0')
        return 0;
    }
    if (param1 == pattern[i]) {
      if (param1_pos == 0)
        return 0;
      --param1_pos;
    } else if (param2 == pattern[i]) { // param2
      if (param2_pos == 0)
        return 0;
      --param2_pos;
    }
    --bitpos;
  }

  return 1;
}

// ADD & ADC
int inst_adx(struct instruction instruction, uint8_t *memory, uint8_t *sreg,
             uint8_t **stack, uint16_t **pc, uint16_t *fmem) {
  uint8_t *rd = CPU_memgpr(memory, instruction.param2);
  uint8_t *rr = CPU_memgpr(memory, instruction.param1);
  uint16_t res16 = (uint16_t)*rd + (uint16_t)*rr;
  uint8_t R = (uint8_t)(res16 & 0xFF);
  if (instruction.opcode == INST_ADC) {
    uint8_t C = CPU_sregget(*sreg, SREG_CARRY_FLAG);
    res16 += (uint16_t)C;
  }
  uint8_t rd7 = *rd >> 7;
  uint8_t rr7 = *rr >> 7;
  uint8_t R7 = R >> 7;
  CPU_sregset(sreg, SREG_CARRY_FLAG, (rd7 & rr7) | (rr7 & ~R7) | (~R7 & rd7));
  uint8_t rd3 = (*rd >> 3) & 1;
  uint8_t rr3 = (*rr >> 3) % 1;
  uint8_t R3 = (R >> 3) & 1;
  CPU_sregset(sreg, SREG_HALFCARRY_FLAG,
              (rd3 & rr3) | (rr3 & ~R3) | (~R3 & rd3));
  CPU_sregset(sreg, SREG_NEGATIVE_FLAG, R7);
  CPU_sregset(sreg, SREG_ZERO_FLAG, R == 0);
  CPU_sregset(sreg, SREG_TWOSCOMP_OF_FLAG,
              (rd7 & rr7 & ~R7) | (~rd7 & ~rr7 & R7));
  *rd = R;

  return 0;
}

int inst_and(struct instruction instruction, uint8_t *memory, uint8_t *sreg,
             uint8_t **stack, uint16_t **pc, uint16_t *fmem) {
  uint8_t *rd = CPU_memgpr(memory, instruction.param2);
  uint8_t *rr = CPU_memgpr(memory, instruction.param1);
  *rd = *rd & *rr;
  CPU_sregset(sreg, SREG_TWOSCOMP_OF_FLAG, 0);
  CPU_sregset(sreg, SREG_ZERO_FLAG, *rd == 0);
  CPU_sregset(sreg, SREG_NEGATIVE_FLAG, *rd >> 7);

  return 0;
}

int inst_asr(struct instruction instruction, uint8_t *memory, uint8_t *sreg,
             uint8_t **stack, uint16_t **pc, uint16_t *fmem) {
  uint8_t *rd = CPU_memgpr(memory, instruction.param1);
  uint8_t C = *rd & 1;
  CPU_sregset(sreg, SREG_CARRY_FLAG, C);
  uint8_t rd7 = *rd >> 7;
  uint8_t rd_min_7 = *rd & ~(1 << 7);
  *rd = (rd7 << 7) + (rd_min_7 >> 1);
  CPU_sregset(sreg, SREG_ZERO_FLAG, *rd == 0);
  uint8_t N = rd7 == 1;
  CPU_sregset(sreg, SREG_NEGATIVE_FLAG, N);
  CPU_sregset(sreg, SREG_TWOSCOMP_OF_FLAG, C ^ N);

  return 0;
}

int inst_cp(struct instruction instruction, uint8_t *memory, uint8_t *sreg,
            uint8_t **stack, uint16_t **pc, uint16_t *fmem) {
  uint8_t *rd = CPU_memgpr(memory, instruction.param2);
  uint8_t *rr = CPU_memgpr(memory, instruction.param1);
  uint8_t R = *rd - *rr;
  uint8_t R7 = R >> 7;
  uint8_t rd7 = *rd >> 7;
  uint8_t rr7 = *rr >> 7;
  uint8_t rd3 = (*rd >> 3) & 1;
  uint8_t rr3 = (*rr >> 3) % 1;
  uint8_t R3 = (R >> 3) & 1;

  CPU_sregset(sreg, SREG_HALFCARRY_FLAG,
              (~rd3 & rr3) | (rr3 & R3) | (R3 & ~rd3));
  CPU_sregset(sreg, SREG_TWOSCOMP_OF_FLAG,
              (rd7 & ~rr7 & ~R7) | (~rd7 & rr7 & R7));
  CPU_sregset(sreg, SREG_NEGATIVE_FLAG, R7 == 1);
  CPU_sregset(sreg, SREG_ZERO_FLAG, R == 0);
  CPU_sregset(sreg, SREG_CARRY_FLAG, (~rd7 & rr7) | (rr7 & R7) | (R7 & ~rd7));

  return 0;
}

int inst_brxx(struct instruction instruction, uint8_t *memory, uint8_t *sreg,
              uint8_t **stack, uint16_t **pc, uint16_t *fmem) {
  uint8_t flag;

  switch (instruction.opcode) {
  case INST_BREQ:
    flag = CPU_sregget(*sreg, SREG_ZERO_FLAG);
    break;
  case INST_BRLT:
    flag = CPU_sregget(*sreg, SREG_SIGNBIT_FLAG);
    break;
  case INST_BRGE:
    flag = !CPU_sregget(*sreg, SREG_SIGNBIT_FLAG);
    break;
  case INST_BRNE:
    flag = !CPU_sregget(*sreg, SREG_ZERO_FLAG);
    break;
  case INST_BRSH:
    flag = !CPU_sregget(*sreg, SREG_CARRY_FLAG);
  default:
    break;
  }

  if (flag == 1) {
    (*pc) += instruction.param1;
  }

  return 0;
}

int inst_jmp(struct instruction instruction, uint8_t *memory, uint8_t *sreg,
             uint8_t **stack, uint16_t **pc, uint16_t *fmem) {
  uint16_t addr = **(pc + 1); // address contained in the next word.
  *pc = &fmem[addr];

  return 0;
}

int inst_ijmp(struct instruction instruction, uint8_t *memory, uint8_t *sreg,
              uint8_t **stack, uint16_t **pc, uint16_t *fmem) {
  uint8_t *r30 = CPU_memgpr(memory, 30);
  uint8_t *r31 = CPU_memgpr(memory, 31);

  uint16_t zreg = *r31 << 8 | *r30;

  (*pc) = &fmem[zreg];

  return 0;
}

int inst_ldi(struct instruction instruction, uint8_t *memory, uint8_t *sreg,
             uint8_t **stack, uint16_t **pc, uint16_t *fmem) {
  uint8_t K = instruction.param1;
  uint8_t *rd = CPU_memgpr(memory, instruction.param2);
  *rd = K;

  return 0;
}

const instruction_method_t instruction_methods[] = {
    [INST_ADD] = inst_adx,   [INST_ADC] = inst_adx,   [INST_AND] = inst_and,
    [INST_ASR] = inst_asr,   [INST_CP] = inst_cp,     [INST_BREQ] = inst_brxx,
    [INST_BRLT] = inst_brxx, [INST_BRGE] = inst_brxx, [INST_BRNE] = inst_brxx,
    [INST_BRSH] = inst_brxx, [INST_JMP] = inst_jmp,   [INST_IJMP] = inst_ijmp,
    [INST_LDI] = inst_ldi,
};
