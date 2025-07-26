#include "asm.h"
#include <stddef.h>
#include <stdio.h>
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

char *get_nl_end(char *input, struct asm_token *token) {
  token->type = TT_NEWLINE_INST_END;
  token->data = NULL;
  if (input[0] == '\n')
    return input + 1;
  if (input[0] == '\r' && input[1] == '\n')
    return input + 2;

  return NULL;
}

char *get_next(char *input, struct asm_token *token) {
  char *next = get_nl_end(input, token);
  if (next)
    return next;

  input = trim_left(input);

  if (strlen(input) == 0)
    return NULL;

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

struct asm_token *expect_separator(struct asm_token *tokens,
                                   size_t tokens_size) {
  if (tokens_size == 0)
    return NULL;
  if (tokens[0].type != TT_SEPARATOR)
    return NULL;
  return tokens + 1;
}

void asm_token_destroy(struct asm_token *token) { free(token->data); }

struct asm_token *get_node_directive(struct asm_token *tokens,
                                     size_t tokens_size,
                                     struct asm_node *node) {
  if (tokens_size == 0)
    return NULL;
  if (tokens[0].type != TT_DIRECTIVE)
    return NULL;
  node->type = NT_DIRECTIVE;
  node->param = malloc(strlen((char *)tokens[0].data) + 1);
  strcpy(node->param, tokens[0].data);

  if (tokens_size > 2 && tokens[1].type == TT_NUMERIC) {
    node->node_list_size = 1;
    node->node_list = malloc(sizeof(struct asm_node));
    node->node_list[0].type = NT_NUMERIC;
    node->node_list[0].param = malloc(sizeof(int));
    *(int *)node->node_list[0].param = *(int *)tokens[1].data;

    return tokens + 2;
  } else {
    node->node_list = NULL;
    node->node_list_size = 0;
    return tokens + 1;
  }
}

struct asm_token *get_node_label(struct asm_token *tokens, size_t tokens_size,
                                 struct asm_node *node) {
  if (tokens_size < 2)
    return NULL;
  if (tokens[0].type != TT_IDENT || tokens[1].type != TT_LABEL)
    return NULL;
  node->type = NT_LABEL;
  node->param = malloc(strlen((char *)tokens[0].data) + 1);
  strcpy(node->param, tokens[0].data);
  node->node_list = NULL;
  node->node_list_size = 0;
  return tokens + 2;
}

struct asm_token *get_node_numeric(struct asm_token *tokens, size_t tokens_size,
                                   struct asm_node *node) {
  if (tokens_size == 0)
    return NULL;
  if (tokens[0].type != TT_NUMERIC)
    return NULL;
  node->type = NT_NUMERIC;
  node->param = malloc(sizeof(int));
  *(int *)node->param = *(int *)tokens[0].data;
  node->node_list = NULL;
  node->node_list_size = 0;
  return tokens + 1;
}

struct asm_token *get_node_symbol(struct asm_token *tokens, size_t tokens_size,
                                  struct asm_node *node) {
  if (tokens_size == 0)
    return NULL;
  if (tokens[0].type != TT_IDENT)
    return NULL;
  node->type = NT_SYMBOL;
  node->param = malloc(strlen((char *)tokens[0].data) + 1);
  strcpy(node->param, tokens[0].data);
  node->node_list = NULL;
  node->node_list_size = 0;
  return tokens + 1;
}

struct asm_token *get_node_instruction(struct asm_token *tokens,
                                       size_t tokens_size,
                                       struct asm_node *node) {
  if (tokens_size == 0)
    return NULL;
  if (tokens[0].type != TT_IDENT)
    return NULL;

  node->type = NT_INSTRUCTION;
  node->param = malloc(strlen((char *)tokens[0].data) + 1);
  strcpy(node->param, tokens[0].data);
  node->node_list = malloc(0);
  node->node_list_size = 0;

  struct asm_node n;
  struct asm_token *next = tokens + 1;
  size_t next_size = tokens_size - 1;

  while (1) {
    if (next_size == 0)
      break;
    if (next[0].type == TT_NEWLINE_INST_END)
      break;
    struct asm_token *tok = get_node_numeric(next, next_size, &n);
    if (tok == NULL) {
      tok = get_node_symbol(next, next_size, &n);
      if (tok == NULL)
        break;
    }
    next = tok;
    next_size--;
    node->node_list_size++;
    node->node_list = realloc(node->node_list,
                              node->node_list_size * sizeof(struct asm_node));
    node->node_list[node->node_list_size - 1] = n;

    tok = expect_separator(next, next_size);
    if (tok == NULL)
      break;
    next = tok;
    next_size--;
  }

  return next;
}

struct asm_token *get_next_node(struct asm_token *tokens, size_t tokens_size,
                                struct asm_node *node) {
  if (tokens_size == 0)
    return NULL;

  struct asm_token *trimmed = tokens;
  while (trimmed->type == TT_NEWLINE_INST_END)
    ++trimmed;
  tokens_size -= trimmed - tokens;

  struct asm_token *next = get_node_label(trimmed, tokens_size, node);
  if (next != NULL)
    return next;
  next = get_node_directive(trimmed, tokens_size, node);
  if (next != NULL)
    return next;
  next = get_node_instruction(trimmed, tokens_size, node);
  if (next != NULL)
    return next;

  return NULL;
}

size_t parse_all(char *input, struct asm_node **out) {
  struct asm_token *tokens = malloc(0);
  size_t tokens_len = 0;

  struct asm_token tok;
  char *next = input;
  while ((next = get_next(next, &tok)) != NULL) {
    ++tokens_len;
    tokens = realloc(tokens, tokens_len * sizeof(struct asm_token));
    tokens[tokens_len - 1] = tok;
  }

  *out = malloc(0);
  size_t nodes_len = 0;
  struct asm_node n;
  struct asm_token *tok_iter = tokens;
  size_t tok_iter_len = tokens_len;
  while ((tok_iter = get_next_node(tok_iter, tok_iter_len, &n)) != NULL) {
    ++nodes_len;
    *out = realloc(*out, nodes_len * sizeof(struct asm_node));
    (*out)[nodes_len - 1] = n;
    tok_iter_len = tokens_len - (tok_iter - tokens);
  }

  for (int i = 0; i < tokens_len; ++i) {
    asm_token_destroy(&tokens[i]);
  }

  return nodes_len;
}

void print_ast(struct asm_node *nodes, size_t nodes_size) {
  for (size_t i = 0; i < nodes_size; ++i) {
    switch (nodes[i].type) {
    case NT_NUMERIC:
      printf("Number(%d)\n", *(int *)nodes[i].param);
      break;

    case NT_DIRECTIVE:
      if (nodes[i].node_list_size == 1) {
        printf("Directive(%s,%d)\n", (char *)nodes[i].param,
               *(int *)nodes[i].node_list[0].param);
      } else {
        printf("Directive(%s)\n", (char *)nodes[i].param);
      }
      break;

    case NT_INSTRUCTION:
      printf("Instruction(%s)\n", (char *)nodes[i].param);
      for (size_t j = 0; j < nodes[i].node_list_size; ++j) {
        printf("  -> ");
        print_ast(nodes[i].node_list + j, 1);
      }
      break;

    case NT_LABEL:
      printf("Label(%s)\n", (char *)nodes[i].param);
      break;

    case NT_SYMBOL:
      printf("Symbol(%s)\n", (char *)nodes[i].param);
      break;

    default:
      break;
    }
  }
}