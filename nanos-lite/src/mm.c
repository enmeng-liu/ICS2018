#include "memory.h"
#include "proc.h"

static void *pf = NULL;

PCB *current;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t new_brk) {
	Log("new_brk=%p", new_brk);
	Log("max_brk=%p", current->max_brk);
	current->cur_brk = new_brk;
	if(new_brk > current->max_brk) {
		current->max_brk = new_brk;
		int m_brk = current->max_brk;
		Log("now max_brk=%p",current->max_brk);
		int size_left = new_brk - m_brk;
		int page_left = PGSIZE - m_brk%PGSIZE;
		size_left -= page_left;
		void* va = (void*)(m_brk/PGSIZE + 1);
		while(size_left > 0) {
			void* pa = new_page(1);
			_map(&(current->as), va, pa, 1);
			va += PGSIZE;
			size_left -= PGSIZE;
		}
	}
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
