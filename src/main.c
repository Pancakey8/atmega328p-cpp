#include "asm.h"
#include "cpu.h"
#include "flash.h"
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

int main() {
  char *example = read_file("./example.asm");
  if (example == NULL) {
    return 1;
  }

  struct asm_node *nodes;
  size_t nodes_len = parse_all(example, &nodes);

  print_ast(nodes, nodes_len);

  return 0;
}