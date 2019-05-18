#include <am.h>
#include <nemu.h>

extern char _heap_start;
extern char _heap_end;
int main();

_Area _heap = {
  .start = &_heap_start,
  .end = &_heap_end,
};

void _putc(char ch) {
  while ((inl(SERIAL_PORT + 8) >> 3) & 1);
  outl(SERIAL_PORT + 4, ch);
}

void _halt(int code) {
#define GPIO_ADDR 0x10000000
  outl(GPIO_ADDR, code);
  // should not reach here
  while (1);
}

void _trm_init() {
  int ret = main();
  _halt(ret);
}
