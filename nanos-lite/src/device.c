#include "common.h"
#include <amdev.h>
#include "klib.h"

size_t serial_write(const void *buf, size_t offset, size_t len) {
	Log("call serial write!");
	char* buff = (char*) buf;
	for(int i = 0; i < len; ++i){
		_putc(buff[i]);
	}
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  return 0;
}


static char dispinfo[128] __attribute__((used));


size_t dispinfo_read(void *buf, size_t offset, size_t len) {
	Log("dipinfo_read: offset = %d, len = %d", offset, len);
	memcpy(buf, (void*)dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
	Log("fb_write: offset = %d, len = %d",offset, len);
	//assert(0);
	int W = screen_width();
	//int H = screen_height();
	for(int i = 0; i < len; ++i){
		int x = (offset + i) % W;
		int y = (offset + i) / W;
		draw_rect((uint32_t*)(buf + i), x, y, 1, 1);
	}
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();
	int W = screen_width();
	int H = screen_height();
	int j = sprintf(dispinfo, "WIDTH:%d\n", W);
	sprintf(dispinfo + j, "HEIGHT:%d\n", H);
	//Log("dispinfo = %s", dispinfo);
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
