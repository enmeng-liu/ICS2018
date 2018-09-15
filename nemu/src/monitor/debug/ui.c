#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
	{ "si", "Execute by single step", cmd_si},
	{ "info", "Show related information, 'r' for registers and 'w' for watchpoints", cmd_info},
	{ "x", "Scan the memory from the address expressed by a certain expression", cmd_x},

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *argv){
		uint64_t num;
		char *arg = strtok(NULL, " ");
		if(arg == NULL){
				num = 1;
		}//the default case
	  else {
			  num = atol(arg);
		}
		cpu_exec(num);
		return 0;
}

static int cmd_info(char *args){
		char *arg = strtok(NULL, " ");
		if(*arg == 'r'){
				printf("$eax = 0x%08x\n", cpu.eax);
				printf("$ecx = 0x%08x\n", cpu.ecx);
				printf("$edx = 0x%08x\n", cpu.edx);
				printf("$ebx = 0x%08x\n", cpu.ebx);
				printf("$esp = 0x%08x\n", cpu.esp);
				printf("$ebp = 0x%08x\n", cpu.ebp);
				printf("$esi = 0x%08x\n", cpu.esi);
				printf("$edi = 0x%08x\n", cpu.edi);
				printf("$eip = 0x%08x\n", cpu.eip);
		}
		else if(*arg == 'w'){
				printf("This command hasn't been implemented yet!\n");
		}
				else{
			  printf("Unkown command\n");
		}
		return 0;
}

static int cmd_x(char *args){
		char *arg = strtok(NULL, " ");
		int num = atoi(arg);
		char *expr = strtok(NULL, " ");
		char *hex="0x";
		if(strncmp(expr, hex ,2) == 0){
				uint32_t addr = strtol(expr, NULL, 16);
				for(int i =0; i < num; ++i){
						if(i%4 == 0){
								if(i !=0){
										printf("\n");
								}
							  printf("0x%x: ", addr);
						}//line change
						printf("%02x ", pmem[addr]);
						addr ++;
				}
				printf("\n");
		}
		else {
				 printf("This command hasn't been implemented!\n");
		}
		return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
