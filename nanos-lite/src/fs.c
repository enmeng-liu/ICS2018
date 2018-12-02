#include "fs.h"
#include "klib.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef size_t ssize_t;
typedef size_t off_t;

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t events_read(void* buf, size_t offset, size_t len);


typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  off_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
	{"/dev/fb", 0, 0, 0, invalid_read, fb_write},
	{"/proc/dispinfo", 128, 0, 0, dispinfo_read, invalid_write},
	{"/dev/events", 1024, 0, 0, events_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
	int W = screen_width();
	int H = screen_height();
 	file_table[3].size = W * H * 4;
}


extern int fs_open(const char *pathname, int flags, int mode){
	Log("open file: %s", pathname);
	for(int i = 0; i < NR_FILES; ++i){
		if(strcmp(pathname, file_table[i].name) == 0){
      file_table[i].open_offset = 0;
			return i;	//use index as file descriptor
		}
	}
	panic("No such file!");
}


extern size_t fs_filesz(int fd){
  return file_table[fd].size;
}

extern ssize_t fs_read(int fd, void *buf, size_t len){
  //assert(file_table[fd].open_offset + len <= file_table[fd].size);  
	Log("fs_read: fd = %d, name = %s, offset = %d, len = %d", fd, file_table[fd].name, file_table[fd].open_offset, len);
	if(file_table[fd].open_offset + len > file_table[fd].size){
		len = file_table[fd].size - file_table[fd].open_offset;
	}

	if(file_table[fd].read == NULL){
  	ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
	}
	else file_table[fd].read(buf, file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
	Log("fs_read: new offset = %d", file_table[fd].open_offset);
  return len;
}

extern ssize_t fs_close(int fd){
	Log("fs_close: fd = %d, name = %s, offset = %d", fd, file_table[fd].name, file_table[fd].open_offset);
  return 0;
}

extern ssize_t fs_write(int fd, const void* buf, size_t len){
	Log("fs_write: fd = %d, offset = %d", fd, file_table[fd].open_offset);
	/*if(fd == 1 || fd == 2){
		char* buff = (char*)buf;
		for(int i = 0; i < len; ++i) _putc(buff[i]);
		return len;
	}*/


	if(file_table[fd].write == NULL){
		if(file_table[fd].open_offset + len > file_table[fd].size)
			len = file_table[fd].size - file_table[fd].open_offset;
 		ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
	}
	else file_table[fd].write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);

	file_table[fd].open_offset += len;
	Log("fs_write: new offset = %d", file_table[fd].open_offset);
  return len;
}

extern off_t fs_lseek(int fd, off_t offset, int whence){
	Log("lseek whence: fd = %d, name = %s, offset = %d, whence = %d", fd, file_table[fd].name, offset, whence);
	Log("fs_lseek: old offset = %d", file_table[fd].open_offset);
	if(strcmp(file_table[fd].name, "/proc/dispinfo") == 0) assert(0);
	switch(whence){
		case SEEK_SET: file_table[fd].open_offset = offset; break;
		case SEEK_CUR: file_table[fd].open_offset += offset; break;
		case SEEK_END: file_table[fd].open_offset = file_table[fd].size + offset; break;
		default: return -1;
	}
	Log("fs_lseek: new offset = %d", file_table[fd].open_offset);
	if(file_table[fd].open_offset > file_table[fd].size)
		file_table[fd].open_offset = file_table[fd].size;
	return file_table[fd].open_offset;
}
