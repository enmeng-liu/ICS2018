#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256,
	TK_NUMBER,
 	TK_EQ

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
	{"[0-9]+", TK_NUMBER}, //numbers
	{"\\(", '('},					//left parenthesis
	{"\\)", ')'},					//right parenthesis
  {"\\+", '+'},         // plus
	{"\\-", '-'},				  // minus
	{"\\*", '*'},					//multiply
	{"/"  , '/'},					//divide
  {"==", TK_EQ}         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
					case TK_NOTYPE:
							//printf("--Spaces are thrown away!--\n");
							break;			//directly throw away the spaces
					case '(':
							tokens[nr_token].type = '('; 
							nr_token ++;
						 // printf("-- ( is recognized!--\n");
							break;
					case ')':
						  tokens[nr_token].type = ')';
						  nr_token ++;
						 // printf("-- ) is recognized!--\n");
						  break;
					case '+':
						  tokens[nr_token].type = '+';
						  nr_token ++;
						// printf("-- + is recognized!--\n");
					   break;	 
					case '-':
							tokens[nr_token].type = '-'; 
							nr_token ++;
						 // printf("-- - is recognized!--\n");
							break;
					case '*':
							tokens[nr_token].type = '*'; 
							nr_token ++;
						 // printf("-- * is recognized!--\n");
							break;
					case '/':
							tokens[nr_token].type = '/'; 
							nr_token ++;
						 // printf("-- / is recognized!--\n");
							break;
					case TK_NUMBER:
							tokens[nr_token].type = TK_NUMBER;
							nr_token ++;
						  if(substr_len > 10){
								panic("Too long number!\n");
							}	
							else {
								strncpy(tokens[nr_token-1].str, substr_start,substr_len);
							  //unsigned temp;
								//temp = (unsigned)atol(tokens[nr_token-1].str);
						  	//	printf("--%u is recognized!--\n", temp);
							}
							break;
          default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p, int q){
	//judge if the pair of parentheses can be thrown away, not just matched
	if(tokens[p].type != '(' || tokens[q].type != ')' ){
		return false;
	//This expression do not need to throw away any parentheses
	}
	else  {
		int brackets = 0;
		for(int i = p; i <= q; ++i){
			if(tokens[i].type == '(') brackets ++;
			else if(tokens[i].type == ')') brackets --;
			if(brackets < 0) {
					panic("Parentheses not match! Illegal expression!");
					assert(0);
			}
		}
		if(brackets !=0) {
			panic("Parentheses not match! Illegal expression!");
			assert(0);
		}
		else if(check_parentheses(p+1, q-1) == false){
		      	printf("Find pair that cannot be thrown away\n");
		      	return false;
	      	} 
		     else {
						printf("Find pair that can be thrown away!\n");
			   		return true;			
				 }
	}
}

static int find_main_op(int p, int q){
		int i = p;
		int op = 0, op_prior = 0;
		int prior[150]={};// the priority of the operators
		prior['+'] = prior['-'] = 1;
		prior['*'] = prior['/'] = 2;
		while(i <= q){
			if(tokens[i].type == TK_NUMBER) i++;
			else if(tokens[i].type == '('){
							while(i <= q && tokens[i].type != ')') i++;
							if(i >= q) {
									panic("WTF!");
									assert(0);
							}
		       }
					 else if(prior[tokens[i].type] <= op_prior){
									 op = i;
									 op_prior = prior[tokens[i].type];
									 //find the latest operator with leatest priority
								 	 // print("Main operator changes to %c\n", tokens[i].type);
					      }
		}
		printf("**The main operator is %c**\n", tokens[op].type);
		return op;
}

uint32_t eval(int p ,int q){
	if(p > q){
		printf("**Somethig wrong with the bounds!**\n");
		assert(0);
	}	
	else if(p == q){
		   		if(tokens[p].type != TK_NUMBER){
							printf("**Cannot read a single nondigit token!**\n");
							assert(0);
					}
					else {
							int temp = (unsigned)atol(tokens[p].str);
							return temp;
					}
	}
			 else if(check_parentheses(p,q) == true){
					  	return eval (p+1, q-1);
						//The expression is surrounded by a pair of parentheses that can be thrown away
			      } 
			 			else {
							int op = find_main_op(p,q); //find the position of the main operator
							uint32_t val1 = eval(p,op-1);
							uint32_t val2 = eval(op+1,q);
							//calculate the two parts of expressions recursively
							switch(tokens[op].type){
									case '+': return val1 + val2;
									case '-': return val1 - val2;
									case '*': return val1 * val2;
									case '/':
												if(val2 == 0){
														panic("Divide by 0");
														assert(0);
												}
												else return val1 / val2;
									default: panic("Unkown calculating error!");
													 assert(0);
							}

						}
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
	uint32_t result = eval(0,nr_token-1);
	return result;

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
