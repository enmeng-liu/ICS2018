#include "proc.h"
#include "common.h"

#define DEFAULT_ENTRY 0x8048000

typedef size_t ssize_t;
typedef size_t off_t;
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();
int fs_open(const char *pathname, int flags, int mode);
size_t fs_filesz(int fd);
ssize_t fs_read(int fd, void* buf, size_t len);
ssize_t fs_close(int fd);
ssize_t fs_write(int fd, const void* buf, size_t len);
off_t fs_lseek(int fd, off_t offset, int whence);

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
	//ramdisk_read((void*)DEFAULT_ENTRY, 0, get_ramdisk_size());
	void* pa = new_page(1);
	Log("pcb->as.ptr=%p",pcb->as.ptr);
	_map(&(pcb->as), (void*)DEFAULT_ENTRY, pa, 1);
	Log("map va=0x%p to pa=0x%x", (void*)DEFAULT_ENTRY, pa);
  int fd = fs_open(filename, 0, 0);
  fs_read(fd, (void*)DEFAULT_ENTRY, fs_filesz(fd));
	fs_close(fd);
  return DEFAULT_ENTRY;
}

extern void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
	Log("call context_kload!");
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
	Log("pcb->as.ptr=%p", pcb->as.ptr);
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
