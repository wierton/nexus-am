#define NOP  addu zero, zero, zero
#define LI(reg, imm) \
    li reg, imm

#define disable_trace_cmp \
    sw zero, -8($0); \
    sw zero, -0x7004($0); \
    sw zero, -0x7004($0)
#define enable_trace_cmp \
    li s2,   0x1;   \
    sw s2,   -8($0); \
    sw zero, -0x7004($0); \
    sw zero, -0x7004($0)

#define HEAP_ADDR    0x11000000 // (16 M beyond MEM)
#define HEAP_ADDR_HI 0x1100

#define CatImpl(a, b) a ## b
#define Cat(a, b) CatImpl(a, b)
#define DADDR(lo) Cat(HEAP_ADDR_HI, lo)

