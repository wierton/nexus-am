#include "trap.h"


int main() {
#ifdef __ISA_MIPS32__
  {
	int in = 0x12345678, out = 0;
	asm volatile("mthi %1; mfhi %0;": "=r"(out) : "r"(in));
	nemu_assert(out == in);
  }

  {
	int in = 0x12345678, out = 0;
	asm volatile("mtlo %1; mflo %0;": "=r"(out) : "r"(in));
	nemu_assert(out == in);
  }
#endif
  return 0;
}
