#include <stdlib.h>
#include <stdio.h>
#include "nemu.h"
#include "debug.h"
#include "assert.h"
#include "monitor/expr.h"


int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

	printf("yarimasune\n");
	uint32_t answer;
	char *test = NULL;
	FILE *fp = fopen("input", "r");
	int cnt = 0;
	assert(fp != NULL);
  //fscanf(fp, "%u %s\n", &answer, test);
	//fscanf(fp, "%u", &answer);
  //fscanf(fp, "%[^\n]%*c", test);
	char *whole = NULL;
	fgets(whole, 70000, fp);
	char *answerchar = NULL;
 	int answersize = sprintf(answerchar, "%s", whole);
	printf("%d\n", answersize);
	answer = (unsigned) atol(answerchar);
	strncpy(test, whole+answersize, 65539);
	printf("%u %s\n", answer, test);
	while(fscanf(fp, "%u %s\n", &answer, test) == 2){
		printf("Read something\n");
		cnt ++;
	  bool succ = true;
		int result = expr(test, &succ);
		if(succ == false) {
			panic("exprssion calcalating failed\n");
			assert(0);
		}
		if(result == answer){
			printf("case %d: right\n", cnt);
		}
		else {
		 printf("case %d: wrong\n", cnt);
		}
	}
	fclose(fp);
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);


  return 0;
}
