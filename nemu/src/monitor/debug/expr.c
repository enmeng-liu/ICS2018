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
	TK_HEX,
	TK_REG,
 	TK_EQ,
	TK_NEQ,
	TK_LE,
	TK_GE,
	TK_L,
	TK_G,
	TK_AND,
	TK_OR,
	TK_DEREF	//dereferrence

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},     //spaces
	{"\\b0[xX][0-9a-fA-F]+\\b",TK_HEX},	 //hex numbers
	{"[0-9]+", TK_NUMBER}, //numbers
	{"\\$[eE][a-zA-Z][xXiIpP]", TK_REG}, //registers
	{"\\(", '('},					//left parenthesis
	{"\\)", ')'},					//right parenthesis
  {"\\+", '+'},         //plus
	{"\\-", '-'},				  //minus
	{"\\*", '*'},					//multiply
	{"/"  , '/'},					//divide
  {"==", TK_EQ},        //equal
	{"!=", TK_NEQ},				//not equal
	{"<=", TK_LE},				//less than or equal
	{">=", TK_GE},				//greater than or equal	
	{"<" , TK_L},					//less than
	{">" , TK_G},					//greater than
	{"&&", TK_AND},				//logic and
	{"\\|\\|", TK_OR}					//logic or
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
	long long value;
} Token;

Token tokens[65600] = {};
int nr_token;

static uint32_t reg_read(char *regname){
	if(strcasecmp(regname,"eax") == 0) return cpu.eax;
	if(strcasecmp(regname,"ecx") == 0) return cpu.ecx;
	if(strcasecmp(regname,"edx") == 0) return cpu.edx;
	if(strcasecmp(regname,"ebx") == 0) return cpu.ebx;
	if(strcasecmp(regname,"esp") == 0) return cpu.esp;
	if(strcasecmp(regname,"ebp") == 0) return cpu.ebp;
	if(strcasecmp(regname,"esi") == 0) return cpu.esi;
	if(strcasecmp(regname,"edi") == 0) return cpu.edi;
	if(strcasecmp(regname,"eip") == 0) return cpu.eip;
	else {panic("Register reading failure!"); return 0;}
}

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

       // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
           // i, rules[i].regex, position, substr_len, substr_len, substr_start);
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
					case TK_EQ:
							tokens[nr_token].type = TK_EQ;
							nr_token ++;
							break;
					case TK_NEQ:
							tokens[nr_token].type = TK_NEQ;
							nr_token ++;
							break;
					case TK_LE:
							tokens[nr_token].type = TK_LE;
							nr_token ++;
							break;
					case TK_GE:
							tokens[nr_token].type = TK_GE;
							nr_token ++;
							break;
					case TK_L:
							tokens[nr_token].type = TK_L;
							nr_token ++;
							break;
					case TK_G:
							tokens[nr_token].type = TK_G;
							nr_token ++;
							break;
					case TK_AND:
							tokens[nr_token].type = TK_AND;
							nr_token ++;
							break;
					case TK_OR:
							tokens[nr_token].type = TK_OR;
							nr_token ++;
							break;
					case TK_HEX:
							tokens[nr_token].type = TK_NUMBER;
							nr_token ++;
							int templen_hex = strlen(tokens[nr_token-1].str);
						  if(substr_len > 100){
								panic("Too long number!\n");
							}	
							else {
								for(int k = 0; k < templen_hex; ++k) tokens[nr_token-1].str[k] = '\0';
								strncpy(tokens[nr_token-1].str, substr_start,substr_len);
								//Log("Get heximal number!");
								tokens[nr_token-1].value = strtol(tokens[nr_token-1].str+2,NULL,16);
								//printf("--Get heximal number %lld in decimal\n", tokens[nr_token-1].value);
							}
							break;
					case TK_NUMBER:
							tokens[nr_token].type = TK_NUMBER;
							nr_token ++;
							int templen = strlen(tokens[nr_token-1].str);
						  if(substr_len > 100){
								panic("Too long number!\n");
							}	
							else {
								for(int k = 0; k < templen; ++k) tokens[nr_token-1].str[k] = '\0';
								strncpy(tokens[nr_token-1].str, substr_start,substr_len);
						    tokens[nr_token-1].value = atoll(tokens[nr_token-1].str);
								//printf("--Get decimal number %lld\n", tokens[nr_token-1].value);
								//directly change into long long type
							}
							break;
					case TK_REG:
							tokens[nr_token].type = TK_NUMBER;
							nr_token ++;
						  if(substr_len > 4){
								panic("Wrong format of registers!\n");
							}	
							else {
								for(int k = 0; k < 4; ++k) tokens[nr_token-1].str[k] = '\0';
								strncpy(tokens[nr_token-1].str, substr_start+1,3);
								//Log("Get registers!");
								tokens[nr_token-1].value =(long long)reg_read(tokens[nr_token-1].str);
								//printf("--Read register %s: %lld\n", tokens[nr_token-1].str,tokens[nr_token-1].value);
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

bool check_inner(int p, int q){
	//check the inner condition of parentheses
	/*for(int i = p; i <= q; ++i){
			if(tokens[i].type == ')') return false;
	}*/
	//if(tokens[p].type != '(' && tokens[p].type != ')' ) return true;
	int brackets = 0;
	for(int i = p; i <= q; ++i){
			if(tokens[i].type == '(') brackets++;
			else if(tokens[i].type == ')') brackets--;
			if(brackets < 0) return false;
	}
	if(brackets != 0) return false;
	return true;
}

bool check_parentheses(int p, int q){
	printf("check () from %d to %d\n", p ,q);
	//judge if the pair of parentheses can be thrown away, not just matched
	if(tokens[p].type != '(' || tokens[q].type != ')') {
			//printf("No parentheses at all\n");
			return false;
	}
	//No parentheses at all
	int brackets = 0;
	for(int i = p; i <= q; ++i){
			if(tokens[i].type == '(') brackets++;
			else if(tokens[i].type == ')') brackets--;
			if(brackets < 0){
					panic("Please check your parentheses!\n");
					assert(0);
			}
	}
	if(brackets != 0) {
		  panic("Please checker your parentheses!\n");
			assert(0);
	}
	if(check_inner(p+1,q-1) == false) {
			//printf("Find pair but cannot throw\n");
			return false;
	}
	else {
			//printf("Find pair can throw\n");
			return true;
	}
}

static int find_main_op(int p, int q){
		printf("find main op from %d to %d\n", p ,q);
		int i = p;
		int op = 0, op_prior = 100;
		int prior[350]={};// the priority of the operators
		prior[')'] = prior['('] = prior[TK_DEREF] = 100;
		prior[TK_OR] = 1;
		prior[TK_AND] = 2;
		prior[TK_EQ] = prior[TK_NEQ] = 3;
		prior[TK_L] = prior[TK_G] = prior[TK_LE] = prior[TK_GE] = 4;
		prior['+'] = prior['-'] = 5;
		prior['*'] = prior['/'] = 6;
		while(i <= q){
			if(tokens[i].type == TK_NUMBER) {
					//printf("Meet numbers!\n");
			}
			else if(tokens[i].type == '('){
							//printf("Meet (!\n");
							//printf("Find )!\n");
							int brackets = 1;
							while(brackets > 0){
								i++;
								if(tokens[i].type == '(') brackets++;
								else if(tokens[i].type == ')') brackets--;
								if(brackets == 0) break;
							}
							if(i > q) {
									panic("WTF!");
									assert(0);
							}
		       }
					 else if(prior[tokens[i].type] <= op_prior){
									 //printf("Meet tokens!\n");
									 op = i;
									 op_prior = prior[tokens[i].type];
									 //find the latest operator with leatest priority
								 	 //Log("Main operator changes to %c\n", tokens[i].type);
					      }
			i++;
		}
		Log("**The main operator is %c at position %d**", tokens[op].type, op);
		return op;
}

long long eval(int p ,int q, bool* success){
	if(*success == false) return 0;
	if(p > q){
		panic("**Somethig wrong with the bounds!**\n");
		assert(0);
	}	
	else if(p == q){
		   		if(tokens[p].type != TK_NUMBER){
				    	panic("**Cannot read a single nondigit token!**\n");
							assert(0);
					}
					else {
							//printf("Get number %u\n", temp);
							return tokens[p].value;
					}
	}
			 else if(check_parentheses(p,q) == true){
					  	return eval (p+1, q-1,success);
						//The expression is surrounded by a pair of parentheses that can be thrown away
			      } 
			 			else {
							if(tokens[p].type == TK_DEREF){
								uint32_t vaddr;
								if(tokens[p+1].type == TK_NUMBER){
									vaddr = (uint32_t)tokens[p+1].value;
									tokens[p+1].value = (long long)vaddr_read(vaddr,32);
									return eval(p+1,q,success);
									}
								else {
									if(tokens[p+1].type == '(' && tokens[q].type == ')'){
										vaddr = (uint32_t)eval(p+1, q, success);
										return (long long) vaddr_read(vaddr,32);
										}
								}
							}
							int op = find_main_op(p,q); //find the position of the main operator
							long long val1 = eval(p,op-1,success);
							long long val2 = eval(op+1,q,success);
							//calculate the two parts of expressions recursively
							switch(tokens[op].type){
									case '+': return val1 + val2;
									case '-':
											/*if(val1 < val2){
													//printf("negative numbers!\n");
													*success = false;
													return 0;
											}	*/
											    return val1 - val2;
									case '*': return val1 * val2;
									case '/':
											if(val2 == 0){
												//printf("Divide by 0!\n");
												*success = false;
												return 0;
											}
										else return val1 / val2;
									case TK_LE: return val1 <= val2;
									case TK_GE: return val1 >= val2;
									case TK_L:  return val1 < val2;
									case TK_G:  return val1 > val2;
									case TK_EQ: return val1 == val2;
									case TK_NEQ:return val1 != val2;
									case TK_AND:return val1 && val2;
									case TK_OR: return val1 || val2;
									default: panic("Unkown calculating error!");
													 assert(0);
							}
            }
					
}

long long expr(char *e, bool *success) {
	//printf("The expr to be calculated is %s\n", e);
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

	for(int i = 0; i < nr_token; ++i){
		int type_now = tokens[i-1].type;
		if(tokens[i].type == '*' && (i == 0 ||(type_now != TK_NUMBER && type_now != ')'))) {
			tokens[i].type = TK_DEREF;
			Log("Recognize dereferrence!");
			if(type_now == TK_NUMBER || type_now == ')') Log("WTF!");
		}
	}

	long long result = eval(0,nr_token-1,success);
	if(*success == false) {
		//printf("Calculation failed!\n");
		return 0;
	}
	return result;
  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
