#include "nemu.h"
#include<stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,DEC

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
  {"\\+", '+'},         // plus
  {"==", TK_EQ},         // equal
  {"\\-", '-'},         //minus
  {"\\*", '*'},         //multi
  {"\\/", '/'},         //div
  {"[0-9]+", DEC},       //decimal number
  {"\\(", '('},          
  {"\\)", ')'}
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

  for (i  = 0; i < NR_REGEX; i ++) {
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

         /* TODO:  Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

         switch (rules[i].token_type) {
			case TK_NOTYPE: break;
          default:
		        assert(substr_len<=32); 
		        tokens[nr_token].type=rules[i].token_type;
				strncpy(tokens[nr_token].str,substr_start,substr_len);
				nr_token++;
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
bool check_parentheses(int p,int q){
	if(tokens[p].type=='('&&tokens[q].type==')'){
		int l=0,r=0;
		for(int i=p+1;i<q;i++){
			if(tokens[i].type=='(') l++;
			if(tokens[i].type==')') r++;
			if(r>l){
				assert(0);
				return false;
		 		} 
		 	}
		if(r==l) return true;
		else return false;
		} 
	else 
	   return false;
	}
int getprecedence(int a){
	if(tokens[a].type=='+'||tokens[a].type=='-') return 1;
	if(tokens[a].type=='*'||tokens[a].type=='/') return 2;
	else return 3;
	}
int find_main_op(int p,int q){
	int op=p+1;
	int opr=p;
	while (op<q){ 
		if(tokens[op].type==DEC) {op++;continue;}
 		else if(tokens[op].type=='('){
			while(tokens[op].type!=')') op++;
			}
         else if(getprecedence(op)<=getprecedence(opr)){
			opr=op;
			op++;
			} 
 		}
	return opr;
	}
uint32_t eval(int p,int q){
 	if(p>q){ 
        printf("tokens path error");
		return -1;} 
 	else if ( p==q){
		if(tokens[p].type==DEC){
 			return atoi(tokens[p].str);}
 		else{ 
            printf("error:no number input");
			return -1;
		} 
	}
    else if (check_parentheses(p,q)==true){
		return eval(p+1,q-1);
 		} 
	else{
		int op=find_main_op(p,q);
		uint32_t val1=eval(p,op-1);
		uint32_t val2=eval(op+1,q);
		switch(tokens[op].type){
			case '+' : return val1+val2;
			case '-' : return val1-val2;
			case '*' : return val1*val2;
			case '/' : return val1/val2;
			default: assert(0);
  		 	}
  		} 
		return 0;
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  } 

  /* TODO: Insert codes to evaluate the expression. */
  return eval(0,nr_token-1);

  //return 0;
}
