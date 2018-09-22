#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];
static int buf_len = 0;

static int choose(int x){
	if(buf_len > 60000) return 0;
  int num = (rand())%x;
	return num;	
}

static inline void gen_space(){
	if(buf_len > 60000) return;
	int num = (rand())%5000;
	for (int i = 1; i <= num; ++i){
		buf_len += sprintf(buf, " ");
	}
}

static inline void gen_num(){
	int num = rand();
	if(num == 0) num++;
  buf_len += sprintf(buf, "%d", num);
}

static inline void gen(char c){
	buf_len += sprintf(buf, "%c", c);
}

static inline void gen_op{
	int num = choose(4);
	switch(num){
		case 0: gen('+'); break;
		case 1: gen('-'); break;
		case 2: gen('*'); break;
		default: gen('/'); break;
	} 
}

static inline void gen_rand_expr() {
  buf[0] = '\0';
	switch (choose(3)){
		case 0: gen_num(); break;
		case 1: gen('('); gen_space();  gen_rand_expr(); gen_space(); gen(')');break;
		default: gen_rand_expr(); gen_space(); gen_rand_op(); gen space(); gen_rand_expr(); break;
	}
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
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
	// input the number of loops by command line
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen(".code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc .code.c -o .expr");
		//system() is used the execute the command line
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
