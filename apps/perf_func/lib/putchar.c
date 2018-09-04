#include <machine.h>

int putchar(int c)
{
tgt_putchar(c);
return 0;
}

inline char __attribute__((__always_inline__)) get_stat(){
  volatile char *stat = SERIAL_PORT + STAT;
  return *stat;
}

inline void out_byte(char ch) {
  volatile char *csend = SERIAL_PORT + Tx;
  while((get_stat() >> 3) & 0x1);
  *csend = ch;
}

inline char in_byte() {
  volatile char *crecv = SERIAL_PORT + Rx;
  if(!(get_stat() & 0x1)) return '\0';
  else return *crecv;
}

void tgt_putchar(char ch) {
  out_byte(ch);
}
