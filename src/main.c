#include "asm.h"
#include "cpu.h"
#include "flash.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  char *buf = 0;
  size_t len = 0;
  FILE *f = fopen("./example.asm", "rb");

  fseek(f, 0, SEEK_END);
  len = ftell(f);
  fseek(f, 0, SEEK_SET);
  buf = malloc(len);
  fread(buf, 1, len, f);
  fclose(f);

  struct asm_token tok;

  char *next = buf;
  while ((next = get_next(next, &tok)) != NULL) {
    if (tok.type == TT_IDENT)
      printf("Ident(%s)\n", (char *)tok.data);
    if (tok.type == TT_DIRECTIVE)
      printf("Directive(%s)\n", (char *)tok.data);
    if (tok.type == TT_NUMERIC)
      printf("Numeric(%d)\n", *(int *)tok.data);
    if (tok.type == TT_SEPARATOR)
      printf("Separator(,)\n");
    if (tok.type == TT_LABEL)
      printf("Label(:)\n");
    free(tok.data);
  }

  /*
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
  */

  return 0;
}