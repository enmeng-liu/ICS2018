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

	uint32_t answer;
	char test[65540];
	FILE *fp = fopen("input", "r");
	int cnt = 0;
	assert(fp != NULL);
	while(fscanf(fp, "%u", &answer) == 1){
		fscanf(fp, "%s", test);
		//printf("Read: %u %s\n", answer, test);
		cnt ++;
	  bool succ = true;
		printf("The parameters passed: %s\n", test );
		uint32_t  result = expr(test, &succ);
		if(succ == false) {
			printf("exprssion calculating failed\n");
			continue;
		}
		if(result == answer){
			printf("case %d: right\n", cnt);
		}
		else {
		 printf("case %d: wrong\n", cnt);
		 printf("The result is %u\n", result);
		 printf("The answer is %u\n", answer);
		}
	}
	fclose(fp);
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);


  return 0;
}
