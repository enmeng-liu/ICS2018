#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
		wp_pool[i].old_value = 0;
    wp_pool[i].next = &wp_pool[i + 1];
		for(int j = 0; j < strlen(wp_pool[i].expr); ++j) {
			wp_pool[i].expr[j] = '\0';
		}
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

	
WP* new_wp(){
	/*return a free watchpoint from *free_*/
	if(free_ == NULL){
		panic("No enough watchpoint structure!");
		assert(0);
		return NULL;
	}
	WP *new_watchpoint = free_;
	free_ = free_ -> next;
	Log("Successfully get a free wp!");
	return new_watchpoint;
}

void add_wp(WP *target_wp, char *exp){
	/*add a new wp to head*/
	if(head == NULL){
		target_wp->NO = 1;
	}
	else{
		target_wp -> NO = head -> NO + 1;
	}
	strncpy(target_wp->expr, exp, strlen(exp));
	target_wp -> old_value = 0;
	target_wp -> next = head;
	head = target_wp;
	Log("Successfully add a new wp: %s", head->expr);
	return;
}

WP* search_wp(int wp_NO){
	/*return the previous pointer to the wp to be deleted*/
	WP* temp = head;
	while(temp != NULL && temp->next != NULL && temp->next->NO != wp_NO){
		temp = temp->next;
	}
	if(temp == NULL){
		panic("Search failure for the watchpoint!");
		assert(0);
		return NULL;
	}
	return temp;
}

void free_wp(int wp_NO){
	/*free a wp from head*/
	if(head == NULL){
		panic("No watchpoint to delete!");
		assert(0);
		return;
	}
	if(head->NO == wp_NO){
		WP* temp = head;
		head = head->next;
		temp->next = free_;
		free_ = temp;
	}
	else{
		WP *pretarget = search_wp(wp_NO);
		WP *target = pretarget -> next;
		pretarget->next = target->next;
		target->next = free_;
		free_ = target;
	}
	Log("Successfully free a wp NO%d: %s",free_->NO, free_->expr);
	return;
}

