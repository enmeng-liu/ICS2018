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

	long long answer;
	char test[65540];
	FILE *fp = fopen("./tools/gen-expr/input", "r");
	int cnt = 0;
	assert(fp != NULL);
	while(fscanf(fp, "%lld", &answer) == 1){
		fscanf(fp, "%s", test);
		cnt ++;
	  bool succ = true;
		long long  result = expr(test, &succ);
		if(succ == false) {
			printf("Illegal expression!\n");
			continue;
		}
		if(result == answer){
			printf("case %d: right\n", cnt);
		}
		else {
		 printf("case %d: wrong\n", cnt);
		 printf("The result is %lld\n", result);
		 printf("The answer is %lld\n", answer);
		}
	}
	fclose(fp);
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);


  return 0;
}
