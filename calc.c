#include "calc.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define compare_size 2

bool valid_arguments(int base, int width) {
  return (base == 2 || base == 10 || base == 16) &&
         (width == 8 || width == 16 || width == 32);
}

int main(int argc, char **argv) {

  struct scan_table_st scan_table;   // table of tokens
  struct parse_table_st parse_table; // table of parse nodes
  struct parse_node_st *parse_tree;  // tree (pointers only) of parse nodes

  char input[SCAN_INPUT_LEN];
  int len;

  bool eFound = false;
  int expr_index;

  bool bFound = false;
  int base = 10;

  bool wFound = false;
  int width = 32;

  for (int i = 0; i < argc - 1; i++) {
    if (i + 1 < argc && !strncmp(argv[i], "-e", compare_size)) {
      eFound = true;
      expr_index = i + 1;
    }
    if (i + 1 < argc && !strncmp(argv[i], "-b", compare_size)) {
      bFound = true;
      base = atoi(argv[i + 1]);
    }
    if (i + 1 < argc && !strncmp(argv[i], "-w", compare_size)) {
      wFound = true;
      width = atoi(argv[i + 1]);
    }
  }

  if (!eFound || base == 0 || width == 0) {
    printf("usage: ./project04 -e \"exr\"\n");
    printf(" example: ./project04 -e \"1 + 2\"\n");
    printf("usage: ./project04 -b \"base\" -e \"expr\"\n");
    printf(" example: ./project04 -b 16 -e \"10 / 0b110\"\n");
    printf("usage: ./project04 -e \"expr\" -b \"base\" -w \"width\"\n");
    printf(" example: ./project04 -e \"0xA + 1\" -b 2 -w 8");
    exit(-1);
  }

  strncpy(input, argv[expr_index], SCAN_INPUT_LEN);
  len = strnlen(input, SCAN_INPUT_LEN);

  scan_table_init(&scan_table);             // Initalizes the scan table
  scan_table_scan(&scan_table, input, len); // Scans the input into th table

  parse_table_init(&parse_table);          // Initalizes the parser
  parse_tree = parse_program(&scan_table); // Parses the scanned tokens

  uint32_t result = eval_tree_get_result(parse_tree); // Gets the result

  display_result(result, base, width);
  return 0;
}
