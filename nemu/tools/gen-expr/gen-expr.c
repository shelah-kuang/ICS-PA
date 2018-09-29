#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];
int cnt=1;
/*void restart(){
	if(cnt>=65536){
		memset(buf,'\0',sizeof(buf));
		cnt=0;
		gen_rand_expr();
		}
	}*/
uint32_t choose(uint32_t n){
	return rand()%n;
}
void gen_notype(){
	switch(choose(20)){
		case 1:buf[cnt]=' ',cnt++;break;
		case 2:buf[cnt]=' ',cnt++;buf[cnt]=' ';cnt++;break;
		default:break;
		}
	}
void gen(char s){
	gen_notype();
	buf[cnt]=s;
	cnt++;
	gen_notype();
	//restart();
}
void gen_rand_op(){
   switch(choose(4)){
	   case 0: buf[cnt]='+';break;
	   case 1: buf[cnt]='-';break;
	   case 2: buf[cnt]='*';break;
       default: buf[cnt]='/';break;
	   }
	cnt++;
	gen_notype();
	//restart();
}
void gen_num(){
	gen_notype();
	uint32_t numb=rand()%100000;
	sprintf(buf+cnt,"%d",numb);
	while(numb!=0){
		cnt++;
		numb/=10;
		}
	gen_notype();
	//restart();
}	
static inline void gen_rand_expr() {
  buf[0] = '\0';
  switch(choose(3)){
	  case 0: gen_num();break;
	  case 1: gen('(');gen_rand_expr();gen('}');break;
	  default:gen_rand_expr();gen_rand_op();gen_rand_expr();break;
	  }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = gen_rand_expr(); "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  } 
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen(".code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc .code.c -o .expr");
    if (ret != 0) continue;

    fp = popen("./.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  } 
  return 0;
}
