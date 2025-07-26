#pragma once

#include "cpu.h"
#include <stddef.h>
enum asm_token_type {
  TT_IDENT,
  TT_DIRECTIVE,
  TT_NUMERIC,
  TT_SEPARATOR,
  TT_LABEL,
  TT_NEWLINE_INST_END
};

struct asm_token {
  enum asm_token_type type;
  void *data;
};

char *trim_left(char *input);
char *get_ident(char *input, struct asm_token *token);
char *get_directive(char *input, struct asm_token *token);
char *get_binary(char *input, struct asm_token *token);
char *get_hex(char *input, struct asm_token *token);
char *get_decimal(char *input, struct asm_token *token);
char *get_separator(char *input, struct asm_token *token);
char *get_label(char *input, struct asm_token *token);
char *get_nl_end(char *input, struct asm_token *token);

char *get_next(char *input, struct asm_token *token);

void asm_token_destroy(struct asm_token *token);

enum asm_node_type {
  NT_DIRECTIVE,
  NT_INSTRUCTION,
  NT_LABEL,
  NT_NUMERIC,
  NT_SYMBOL
};

struct asm_node {
  enum asm_node_type type;
  void *param;
  struct asm_node *node_list;
  size_t node_list_size;
};

struct asm_token *get_node_directive(struct asm_token *tokens,
                                     size_t tokens_size, struct asm_node *node);
struct asm_token *get_node_label(struct asm_token *tokens, size_t tokens_size,
                                 struct asm_node *node);
struct asm_token *get_node_numeric(struct asm_token *tokens, size_t tokens_size,
                                   struct asm_node *node);
struct asm_token *get_node_symbol(struct asm_token *tokens, size_t tokens_size,
                                  struct asm_node *node);
struct asm_token *get_node_instruction(struct asm_token *tokens,
                                       size_t tokens_size,
                                       struct asm_node *node);
struct asm_token *get_next_node(struct asm_token *tokens, size_t tokens_size,
                                struct asm_node *node);

size_t parse_all(char *input, struct asm_node **out);

void print_ast(struct asm_node *nodes, size_t nodes_size);