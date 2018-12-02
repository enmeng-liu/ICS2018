#include "common.h"
#include <amdev.h>
#include "klib.h"

size_t serial_write(const void *buf, size_t offset, size_t len) {
	//Log("call serial write!");
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
	//Log("events_read! len = %d", len);
	int keytemp = read_key() & 0xffff;
	char temp[128]; 
	if(keytemp != 0) {
		//Log("get key: %d", keytemp ^ 0x8000);
		if((keytemp & 0x8000) != 0){
			sprintf(temp, "kd %s\n", keyname[keytemp ^ 0x8000]);
			strncpy(buf, temp, len);
		}
		else {
			sprintf(temp, "ku %s\n", keyname[keytemp]);
			strncpy(buf, temp, len);
		}
	}
	else{
		uint32_t time_now = uptime();
		sprintf(temp, "t %d\n", time_now);
		strncpy(buf, temp, len);
	}

	//Log("get events: %s", temp);
	if(strlen(temp) < len) len = strlen(temp);
  return len;
}


static char dispinfo[128] __attribute__((used));


size_t dispinfo_read(void *buf, size_t offset, size_t len) {
	//Log("dipinfo_read: offset = %d, len = %d", offset, len);
	memcpy(buf, (void*)dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
	//Log("fb_write: offset = %d, len = %d",offset, len);
	//assert(0);
	int W = screen_width();
	/*for(int i = 0; i < len; i += 4){
		int x = (offset + i) % W;
		int y = (offset + i) / W;
		draw_rect((uint32_t*)(buf + i), x, y, 1, 1);
	}*/
	int x = (offset / 4) % W;
	int y = (offset / 4) / W;
	draw_rect((uint32_t*)buf, x, y, len/4, 1);
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
