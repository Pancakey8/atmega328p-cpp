#include "asm.h"
#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    perror("fopen failed");
    return NULL;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    perror("fseek failed");
    fclose(file);
    return NULL;
  }

  long size = ftell(file);
  if (size < 0) {
    perror("ftell failed");
    fclose(file);
    return NULL;
  }
  rewind(file);

  char *buffer = malloc(size + 1);
  if (buffer == NULL) {
    perror("malloc failed");
    fclose(file);
    return NULL;
  }

  size_t read_size = fread(buffer, 1, size, file);
  if (read_size != size) {
    perror("fread failed");
    free(buffer);
    fclose(file);
    return NULL;
  }

  buffer[size] = '\0';

  fclose(file);
  return buffer;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    perror("File argument missing");
    return 1;
  }

  char *example = read_file(argv[1]);
  if (example == NULL) {
    perror("File can't be read");
    return 1;
  }

  struct asm_node *nodes;
  size_t nodes_len = parse_all(example, &nodes);

  for (size_t i = 0; i < nodes_len; ++i) {
    switch (nodes[i].type) {
    case NT_DIRECTIVE:
      break;
    case NT_LABEL:
      break;
    case NT_INSTRUCTION:
      break;
    }
  }
}