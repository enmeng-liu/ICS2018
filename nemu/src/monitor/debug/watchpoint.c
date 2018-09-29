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
		wp_pool[i].new_value = 0;
		wp_pool[i].change_flag = false;
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
	bool succ = true;
	target_wp -> old_value = expr(exp, &succ);
	if(succ == false){
		panic("Something wrong with calculation!");
		assert(0);
		return;
	}
	target_wp -> new_value = target_wp -> old_value;
	target_wp -> change_flag = false;
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
	Log("Successfully free a wp NO.%d: %s",free_->NO, free_->expr);
	return;
}


void info_wp(){
	/*print all the information of wps*/
	WP *temp = head;
	if(head == NULL){
		printf("No wp to info!\n");
		return;
	}
	while(temp != NULL){
		printf("Watchpoint No.%d: %s =  %lld\n", temp->NO, temp->expr, temp->old_value);
		temp = temp->next;
	}
}


bool check_wp(){
	/*check whether the wps has changed*/
	bool flag = false;
	WP* temp = head;
	if(head == NULL){
		return false;
	}
	while(temp != NULL){
		bool succ = true;
		temp -> new_value = expr(temp -> expr, &succ);
		if(succ == false) {
			panic("wp calculating error!");
			assert(0);
			return false;
		}
		if(temp->new_value != temp->old_value){
			flag = true;
			temp -> change_flag = true;
			Log("Recognize wp change NO.%d", temp->NO);
		}
		temp = temp -> next;
	}
	Log("Wp checked!");
	return flag;
}

void print_wp(){
	/*print all the changed wp*/
	WP* temp = head;
	if(head == NULL) {
		panic("wp printing error!");
		assert(0);
		return;
	}
	while(temp != NULL){
		if(temp->change_flag == true){
			printf("Changed watchpoint NO.%d: %s\n", temp->NO, temp->expr);
			printf("Old value: %lld\n", temp->old_value);
			printf("New value: %lld\n", temp->new_value);
		}
		temp = temp->next;
	}
	Log("wp printed!");
	return;
}

void update_wp(){
	/*update the value of all wps*/
	if(head == NULL){
		panic("wp updating error!");
		assert(0);
		return;
	}
	WP* temp = head;
	while(temp != NULL){
		temp->old_value = temp->new_value;
		temp->change_flag = false;
		temp = temp->next;
	}
	Log("wp updated!");
	return;
}


