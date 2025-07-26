#pragma once

enum asm_token_type {
  TT_IDENT,
  TT_DIRECTIVE,
  TT_NUMERIC,
  TT_SEPARATOR,
  TT_LABEL
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

char *get_next(char *input, struct asm_token *token);