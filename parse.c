// parse.c - parsing and parse tree construction

#include "calc.h"

#define binary_base 2
#define hex_base 16
#define dec_base 10

struct oper {
  enum scan_token_enum id;
  enum parse_oper_enum oper;
};

// Oper map to store token_enums
// with their associated parse_enums
struct oper oper_map[] = {
    {TK_PLUS, OP_PLUS},
    {TK_MINUS, OP_MINUS},
    {TK_MULT, OP_MULT},
    {TK_DIV, OP_DIV},
    {TK_RIGHT_SHIFT, OP_RIGHT_SHIFT},
    {TK_LEFT_SHIFT, OP_LEFT_SHIFT},
    {TK_AND, OP_AND},
    {TK_NOT, OP_NOT},
    {TK_OR, OP_OR},
    {TK_EXOR, OP_EXOR},
};

void parse_table_init(struct parse_table_st *parse_table) {
  parse_table->root = NULL;
}

// Allocate a parse node from the table of parse nodes
struct parse_node_st *parse_node_new() {
  return calloc(1, sizeof(struct parse_node_st));
}

void parse_error(char *err) {
  printf("parse_error: %s\n", err);
  exit(-1);
}

// These are names of operators for printing
char *parse_oper_strings[] = {"PLUS", "MINUS", "MULT", "DIV"};

// Print the dots which represent tree depth in the output
void parse_tree_print_indent(int level) {
  level *= 2;
  for (int i = 0; i < level; i++) {
    printf(".");
  }
}

// Traverse the tree recursively to print out the structs
void parse_tree_print_expr(struct parse_node_st *node, int level) {
  parse_tree_print_indent(level);
  printf("EXPR ");

  if (node->type == EX_INTVAL) {
    printf("INTVAL %d\n", node->intval.value);
  } else if (node->type == EX_OPER2) {
    printf("OPER2 %s\n", parse_oper_strings[node->oper2.oper]);
    parse_tree_print_expr(node->oper2.left, level + 1);
    parse_tree_print_expr(node->oper2.right, level + 1);
  } else if (node->type == EX_OPER1) {
    printf("OPER1 %s\n", parse_oper_strings[node->oper1.oper]);
    parse_tree_print_expr(node->oper1.operand, level + 1);
  }
}

void parse_tree_print(struct parse_node_st *np) {
  parse_tree_print_expr(np, 0);
}

// Parse the "program" part of the EBNF
// A program is composed of an expression followed by EOT
struct parse_node_st *parse_program(struct scan_table_st *scan_table) {
  struct parse_node_st *root;

  root = parse_expression(scan_table);

  if (!scan_table_accept(scan_table, TK_EOT)) {
    parse_error("Expecting EOT");
  }

  return root;
}

// Expressions are defined in the EBNF as an operator followed
// by zero or more operator operand pairs
struct parse_node_st *parse_expression(struct scan_table_st *scan_table) {
  struct scan_token_st *token;
  struct parse_node_st *node1, *node2;

  node1 = parse_operand(scan_table);

  while (true) {
    token = scan_table_get(scan_table, 0);
    for (size_t i = 0; i < sizeof(oper_map) / sizeof(oper_map[0]); i++) {
      if (oper_map[i].id == token->id) {
        scan_table_accept(scan_table, TK_ANY);
        node2 = parse_node_new();
        node2->type = EX_OPER2;
        node2->oper2.oper = oper_map[i].oper;
        node2->oper2.left = node1;
        node2->oper2.right = parse_operand(scan_table);
        node1 = node2;
      }
    }
    break;
  }

  return node1;
}

// Inserts a number value into the tree
void insert_number_value(struct scan_token_st *token,
                         struct parse_node_st *node) {
  struct parse_node_st *int_node = parse_node_new();
  int_node->type = EX_INTVAL;
  int base;

  switch (token->id) {
  case TK_INTLIT:
    base = dec_base;
    break;
  case TK_BINLIT:
    base = binary_base;
    break;
  case TK_HEXLIT:
    base = hex_base;
    break;
  default:
    printf("ERROR! Invalid Token Id.\n");
  }

  if (node->type == EX_OPER1) {
    int_node->intval.value = string_to_int(token->name, base);
    node->oper1.operand = int_node;

  } else {
    node->intval.value = string_to_int(token->name, base);
  }
}

// Returns true if the next value is a number value
bool is_number_value(struct scan_table_st *scan_table) {
  return scan_table_accept(scan_table, TK_INTLIT) ||
         scan_table_accept(scan_table, TK_HEXLIT) ||
         scan_table_accept(scan_table, TK_BINLIT);
}

// Parse operands, which are defined in the EBNF to be
// integer literals or unary minus or expressions
struct parse_node_st *parse_operand(struct scan_table_st *scan_table) {
  struct scan_token_st *token;
  struct parse_node_st *node;

  // Accounts for uniary operators ie - and ~
  if (scan_table_accept(scan_table, TK_MINUS) ||
      scan_table_accept(scan_table, TK_NOT)) {
    node = parse_node_new();
    node->type = EX_OPER1;
    token = scan_table_get(scan_table, -1);

    if (token->id == TK_MINUS) {
      node->oper1.oper = OP_MINUS;
    } else {
      node->oper1.oper = OP_NOT;
    }

    token = scan_table_get(scan_table, 0);

    if (is_number_value(scan_table)) {
      insert_number_value(token, node);

    } else if (scan_table_accept(scan_table, TK_LPAREN)) {
      struct parse_node_st *operand_node = parse_node_new();
      operand_node = parse_expression(scan_table);
      if (scan_table_accept(scan_table, TK_RPAREN)) {
        node->oper1.operand = operand_node;
        return node;
      } else {
        printf("ERROR! No matching right parentheses\n");
      }
    } else {
      printf(
          "ERROR! Uninary minus was not followed with a value or expression\n");
    }

    // Accounts for regular number values
  } else if (is_number_value(scan_table)) {
    token = scan_table_get(scan_table, -1);
    node = parse_node_new();
    node->type = EX_INTVAL;

    insert_number_value(token, node);

    // Accounts for parentheses
  } else if (scan_table_accept(scan_table, TK_LPAREN)) {
    node = parse_expression(scan_table);
    if (scan_table_accept(scan_table, TK_RPAREN)) {
      return node;
    } else {
      parse_error("Missing closing parenthesis");
    }
  } else {
    parse_error("Bad operand");
  }

  return node;
}
