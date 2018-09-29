#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
	char expr[200];
	long long old_value;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */


} WP;

WP* new_wp();
void add_wp(WP* target_wp, char *exp);
WP* search_wp(int wp_NO);
void free_wp(int wp_NO);

#endif
