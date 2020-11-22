// https://rosettacode.org/wiki/Arithmetic_evaluation/C
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
 
unsigned int G_STRING_ITERATOR = 0;
 
/*
 * expr        := term term_tail
 * term_tail   := add_op term term_tail | e
 * term        := factor factor_tail
 * factor_tail := mult_op factor factor_tail | e
 * factor      := ( expr ) | number
 * add_op      := + | -
 * mult_op     := * | /
 */
 
typedef union {
  int terminal;
  struct expression* expr[2];
} Data;
 
typedef struct expression {
  char op;
  Data data;
} Expr;
 
void parse_error(const char* string) {
  unsigned int i;
  fprintf(stderr, "Unexpected symbol '%c' at position %u.\n\n", string[G_STRING_ITERATOR], G_STRING_ITERATOR);
  fprintf(stderr, "String: '%s'\n", string);
  fprintf(stderr, "Problem: ");
  for(i = 0; i < G_STRING_ITERATOR; ++i) {
    fprintf(stderr, " ");
  }
  fprintf(stderr, "^\n");
  exit(1);
}
 
/* Will "consume" a character from the input,
 * (such as +, -, *, etc.) and return it.
 * By consume, I'm really just moving the pointer
 * forward and disregarding the character for
 * future purposes.
 */
char consume_char(const char* string, char c) {
  if(string[G_STRING_ITERATOR] != c) {
    parse_error(string);
  }
  ++G_STRING_ITERATOR;
  return c;
}
 
/* Same as consume_char, except for integers.
 */
int consume_int(const char* string) {
  int i;
 
  if(!isdigit(string[G_STRING_ITERATOR])) {
    parse_error(string);
  }
 
  /* I don't have to pass in the start of the string
   * into atoi, but only where I want it to start
   * scanning for an integer.
   */
  i = atoi(string + G_STRING_ITERATOR);
  while(isdigit(string[G_STRING_ITERATOR])) {
    ++G_STRING_ITERATOR;
  }
  return i;
}
 
Expr* expression(const char* string);
 
Expr* factor(const char* string, Expr* expr) {
  if(string[G_STRING_ITERATOR] == '(') {
    expr->op = consume_char(string, '(');
    expr->data.expr[0] = expression(string);
    consume_char(string, ')');
  } else if(isdigit(string[G_STRING_ITERATOR])) {
    expr->op = 'd';
    expr->data.terminal = consume_int(string);
  }
  return expr;
}
 
Expr* factor_tail(const char* string, Expr* expr) {
  Expr* new_expr;
 
  switch(string[G_STRING_ITERATOR]) {
  case '*':
  case '/':
    if(NULL == (new_expr = (Expr*)malloc(sizeof(Expr)))) {
      exit(1);
    }
    if(NULL == (new_expr->data.expr[1] = (Expr*)malloc(sizeof(Expr)))) {
      exit(1);
    }
    new_expr->op = consume_char(string, string[G_STRING_ITERATOR]);
    new_expr->data.expr[0] = expr;
 
    new_expr->data.expr[1] = factor(string, new_expr->data.expr[1]);
    new_expr = factor_tail(string, new_expr);
    return new_expr;
  case '+':
  case '-':
  case ')':
  case 0:
    return expr;
  default:
    parse_error(string);
  }
}
 
Expr* term(const char* string, Expr* expr) {
  if(string[G_STRING_ITERATOR] == '(' || isdigit(string[G_STRING_ITERATOR])) {
    expr = factor(string, expr);
    expr = factor_tail(string, expr);
    return expr;
  } else {
    parse_error(string);
  }
}
 
Expr* term_tail(const char* string, Expr* expr) {
  Expr* new_expr;
 
  switch(string[G_STRING_ITERATOR]) {
  case '+':
  case '-':
    if(NULL == (new_expr = (Expr*)malloc(sizeof(Expr)))) {
      exit(1);
    }
    if(NULL == (new_expr->data.expr[1] = (Expr*)malloc(sizeof(Expr)))) {
      exit(1);
    }
    new_expr->op = consume_char(string, string[G_STRING_ITERATOR]);
    new_expr->data.expr[0] = expr;
 
    new_expr->data.expr[1] = term(string, new_expr->data.expr[1]);
    new_expr = term_tail(string, new_expr);
    return new_expr;
  case ')':
  case 0:
    return expr;
  default:
    parse_error(string);
  }
}
 
Expr* expression(const char* string) {
  Expr* expr;
 
  if(string[G_STRING_ITERATOR] == '(' || isdigit(string[G_STRING_ITERATOR])) {
    if(NULL == (expr = (Expr*)malloc(sizeof(Expr)))) {
      exit(1);
    }
 
    expr = term(string, expr);
    expr = term_tail(string, expr);
    return expr;
  } else {
    parse_error(string);
  }
}
 
/* Runs through the AST, evaluating and freeing
 * the tree as it goes.
 */
int evaluate(Expr* expr) {
  int ret;
 
  switch(expr->op) {
  case '(':
    ret = evaluate(expr->data.expr[0]);
    free(expr->data.expr[0]);
    break;
  case '*':
    ret =
      evaluate(expr->data.expr[0])
      *
      evaluate(expr->data.expr[1])
      ;
    free(expr->data.expr[0]);
    free(expr->data.expr[1]);
    break;
  case '/':
    ret =
      evaluate(expr->data.expr[0])
      /
      evaluate(expr->data.expr[1])
      ;
    free(expr->data.expr[0]);
    free(expr->data.expr[1]);
    break;
  case '+':
    ret =
      evaluate(expr->data.expr[0])
      +
      evaluate(expr->data.expr[1])
      ;
    free(expr->data.expr[0]);
    free(expr->data.expr[1]);
    break;
  case '-':
    ret =
      evaluate(expr->data.expr[0])
      -
      evaluate(expr->data.expr[1])
      ;
    free(expr->data.expr[0]);
    free(expr->data.expr[1]);
    break;
  case 'd':
    ret = expr->data.terminal;
    break;
  default:
    exit(1);
  }
  return ret;
}


// https://stackoverflow.com/a/16871811/7032856
#include <string.h>
char* readinput()
{
#define CHUNK 200
   char* input = NULL;
   char tempbuf[CHUNK];
   size_t inputlen = 0, templen = 0;
   do {
       fgets(tempbuf, CHUNK, stdin);
       tempbuf[strcspn(tempbuf, "\r")] = 0;
       tempbuf[strcspn(tempbuf, "\n")] = 0;
       templen = strlen(tempbuf);
       input = realloc(input, inputlen+templen+1);
       strcpy(input+inputlen, tempbuf);
       inputlen += templen;
    } while (templen==CHUNK-1 && tempbuf[CHUNK-2]!='\n');
    return input;
}



/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */
#include "calculator.h"

void
calculator_prog_1(char *host)
{
	CLIENT *clnt;
	int  *result_1;
	Argument  add_1_arg;
	int  *result_2;
	Argument  subtract_1_arg;
	int  *result_3;
	Argument  multiply_1_arg;
	int  *result_4;
	Argument  divide_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, CALCULATOR_PROG, CALCULATOR_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = add_1(&add_1_arg, clnt);
	if (result_1 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_2 = subtract_1(&subtract_1_arg, clnt);
	if (result_2 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_3 = multiply_1(&multiply_1_arg, clnt);
	if (result_3 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_4 = divide_1(&divide_1_arg, clnt);
	if (result_4 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}




int
calculate_at_server(const char *host, char function_type, int a, int b)
{
	CLIENT *clnt;
	int  *result;
	Argument  arg;
	
	arg.a = a;
	arg.b = b;

#ifndef	DEBUG
	clnt = clnt_create (host, CALCULATOR_PROG, CALCULATOR_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */


	switch(function_type) {
	case '+':
		result = add_1(&arg, clnt);
		break;
	case '-':
		result = subtract_1(&arg, clnt);
		break;
	case '*':
		result = multiply_1(&arg, clnt);
		break;
	case '/':
		result = divide_1(&arg, clnt);
		break;
	default:
		exit(1);
		break;
	}
	if (result == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	} else {
		return *result;
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}
/* Runs through the AST, evaluating and freeing
 * the tree as it goes.
 */
int evaluate_at_server(const char* host, Expr* expr) {
  int ret;
 
  switch(expr->op) {
  case '(':
    ret = evaluate_at_server(host, expr->data.expr[0]);
    free(expr->data.expr[0]);
    break;
  case '*':
  case '/':
  case '+':
  case '-':
    ret =
    calculate_at_server(
    	host,
    	expr->op,
    	evaluate_at_server(host, expr->data.expr[0]),
    	evaluate_at_server(host, expr->data.expr[1])
    );
    free(expr->data.expr[0]);
    free(expr->data.expr[1]);
    break;
  case 'd':
    ret = expr->data.terminal;
    break;
  default:
    exit(1);
  }
  return ret;
}

int
main (int argc, char *argv[])
{
	const char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	
	while(1) {
		// TODO: fix 2nd loop Unexpected symbol '' at position
		char* expression_string = readinput();
		Expr* expr = expression(expression_string);
    	printf("%s = %d\n", expression_string, evaluate_at_server(host, expr));
    	free(expression_string);
    	free(expr);
	}
	
exit (0);
}
