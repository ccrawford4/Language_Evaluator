#include "calc.h"
#define HEX_LEN 8

// Traverses the tree to compute the expression
uint32_t eval_expr(struct parse_node_st *node, int level) {
  switch (node->type) {
  // Any number value will be returned
  case EX_INTVAL:
    return node->intval.value;

  // Operators with two operands will have to find its children
  case EX_OPER2: {
    uint32_t left = eval_expr(node->oper2.left, level + 1);
    uint32_t right = eval_expr(node->oper2.right, level + 1);
    switch (node->oper2.oper) {
    case OP_PLUS:
      return left + right;
    case OP_MINUS:
      return left - right;
    case OP_MULT:
      return left * right;
    case OP_DIV:
      return left / right;
    case OP_RIGHT_SHIFT:
      return left >> right;
    case OP_LEFT_SHIFT:
      return left << right;
    case OP_AND:
      return left & right;
    case OP_OR:
      return left | right;
    case OP_EXOR:
      return left ^ right;
    default:
      printf("ERROR! Invalid Operand\n");
      exit(-1);
    }
  }
  // Uniary operators will find one child operand
  case EX_OPER1: {
    uint32_t operand = eval_expr(node->oper1.operand, level + 1);
    switch (node->oper1.oper) {
    case OP_MINUS:
      return -operand;
    case OP_NOT:
      return ~operand;
    default:
      printf("ERROR! Invalid Operand\n");
      exit(-1);
    }
  }
  // Throw error for invalid operands
  default:
    printf("ERROR! Invalid Operand\n");
    exit(-1);
  }
}

// Reverses a string
void reverse_string(char *string) {
  int l = 0;
  int r = strlen(string) - 1;
  while (l < r) {
    int temp = string[l];
    string[l] = string[r];
    string[r] = temp;
    l++;
    r--;
  }
}

// Converts an int to string given a base
char *int_to_string(uint32_t value, char *buffer, int base) {
  if (value == 0) {
    buffer[0] = '0';
    buffer[1] = '\0';
    return buffer;
  }

  int index = 0;
  while (value > 0) {
    int remainder = value % base;
    if (remainder < 10) {
      buffer[index++] = remainder + '0';
    } else {
      buffer[index++] = remainder - 10 + 'A';
    }
    value /= base;
  }

  buffer[index] = '\0';
  reverse_string(buffer);
  return buffer;
}

// Display the result given a base
// Accounts for leading 0s
void display_result(uint32_t value, int base, int width) {
  char buffer[SCAN_TOKEN_LEN];
  char *string = int_to_string(value, buffer, base);
  int size = strnlen(string, SCAN_TOKEN_LEN);

  int num_zeros = 0;
  if (base == 2) {
    num_zeros = width - size;
  } else if (base == 16) {
    if (width == 32) {
      num_zeros = HEX_LEN - size;
    } else {
      num_zeros = width - size;
    }
  }

  if (base == 2) {
    printf("0b");
  } else if (base == 16) {
    printf("0x");
  }

  int i = 0;
  while (i < num_zeros) {
    printf("0");
    i++;
  }
  printf("%s\n", string);
}

// Inital function that gets called using the root of the AST tree
// Calls the eval_expr function to get the result and returns it
uint32_t eval_tree_get_result(struct parse_node_st *np) {
  uint32_t result = eval_expr(np, 0);
  return result;
}
