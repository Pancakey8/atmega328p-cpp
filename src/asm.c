#include "asm.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int is_ws(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' ||
         c == '\f';
}

int is_alpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

int is_digit(char c) { return '0' <= c && c <= '9'; }

int is_end(char c) { return c == '\0' || is_ws(c) || c == ',' || c == ':'; }

char *trim_left(char *input) {
  size_t len = 0;
  while (is_ws(input[len])) {
    len++;
  }
  return input + len;
}

char *get_directive(char *input, struct asm_token *token) {
  if (input[0] != '.')
    return NULL;
  input++;
  input = get_ident(input, token);
  token->type = TT_DIRECTIVE;
  return input;
}

char *get_ident(char *input, struct asm_token *token) {
  if (!is_alpha(input[0]))
    return NULL;
  size_t len = 1;
  while (!is_end(input[len])) {
    if (!is_alpha(input[len]) && !is_digit(input[len]))
      return NULL;
    len++;
  }
  token->type = TT_IDENT;
  token->data = calloc(len + 1, 1);
  memcpy(token->data, input, len);
  ((char *)token->data)[len] = '\0';
  return input + len;
}

char *get_binary(char *input, struct asm_token *token) {
  if (!(input[0] == '0' && input[1] == 'b'))
    return NULL;
  input += 2;
  size_t len = 0;
  while (!is_end(input[len])) {
    if (!(input[len] == '0' || input[len] == '1'))
      return NULL;
    len++;
  }
  token->type = TT_NUMERIC;
  token->data = calloc(1, sizeof(int));
  *(int *)token->data = 0;
  for (size_t i = 0; i < len; ++i) {
    *(int *)token->data = *(int *)token->data * 2 + (input[i] - '0');
  }
  return input + len;
}

char *get_hex(char *input, struct asm_token *token) {
  if (!(input[0] == '0' && input[1] == 'x'))
    return NULL;
  input += 2;
  size_t len = 0;
  while (!is_end(input[len])) {
    int checkafl = 'a' <= input[len] && input[len] <= 'f';
    int checkafu = 'A' <= input[len] && input[len] <= 'F';
    if (!(checkafl || checkafu || is_digit(input[len])))
      return NULL;
    len++;
  }
  token->type = TT_NUMERIC;
  token->data = calloc(1, sizeof(int));
  *(int *)token->data = 0;
  for (size_t i = 0; i < len; ++i) {
    int value = 0;
    if ('a' <= input[i] && input[i] <= 'f') {
      value = 10 + input[i] - 'a';
    }
    if ('A' <= input[i] && input[i] <= 'F') {
      value = 10 + input[i] - 'A';
    }
    if (is_digit(input[i])) {
      value = input[i] - '0';
    }
    *(int *)token->data = *(int *)token->data * 16 + value;
  }
  return input + len;
}

char *get_decimal(char *input, struct asm_token *token) {
  size_t len = 0;
  while (input[len] != '\0' && !is_ws(input[len])) {
    if (!is_digit(input[len]))
      return NULL;
    len++;
  }
  token->type = TT_NUMERIC;
  token->data = calloc(1, sizeof(int));
  *(int *)token->data = 0;
  for (size_t i = 0; i < len; ++i) {
    *(int *)token->data = *(int *)token->data * 10 + (input[i] - '0');
  }
  return input + len;
}

char *get_separator(char *input, struct asm_token *token) {
  if (input[0] != ',')
    return NULL;
  token->type = TT_SEPARATOR;
  token->data = NULL;
  return input + 1;
}

char *get_label(char *input, struct asm_token *token) {
  if (input[0] != ':')
    return NULL;
  token->type = TT_LABEL;
  token->data = NULL;
  return input + 1;
}

char *get_next(char *input, struct asm_token *token) {
  input = trim_left(input);

  if (strlen(input) == 0)
    return NULL;

  char *next;

  next = get_directive(input, token);
  if (next)
    return next;
  next = get_ident(input, token);
  if (next)
    return next;
  next = get_binary(input, token);
  if (next)
    return next;
  next = get_decimal(input, token);
  if (next)
    return next;
  next = get_hex(input, token);
  if (next)
    return next;
  next = get_separator(input, token);
  if (next)
    return next;
  next = get_label(input, token);
  if (next)
    return next;

  return NULL;
}