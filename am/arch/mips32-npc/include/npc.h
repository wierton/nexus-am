#ifndef __NPC_H__
#define __NPC_H__

#define EBASE ((void *)0xbfc00000)

/* ====================vga================== */
#define VMEM_ADDR ((volatile void *)0xb0400000)
#define SCR_WIDTH 400
#define SCR_HEIGHT 300
#define SCR_SIZE (SCR_WIDTH * SCR_HEIGHT)

/* ====================serial================== */
#define SERIAL_PORT ((volatile char *)0xbfe50000)
#define Rx 0x0
#define Tx 0x04
#define STAT 0x08
#define CTRL 0x0c

/* ====================keyboard================== */
#define SCANCODE 0x10
#define SCANCODE_STAT 0x14

/* ====================gpio================== */
#define GPIO_TRAP ((volatile char *)0xb0000000)

/* ====================schedule================== */
#define HZ 50000000
#define INTERVAL 300000

/* ====================timer================== */
// #define REAL_TIMER_BASE ((volatile char *)0x41c00000)
// #define INT_TIMER_BASE ((volatile char *)0x41c10000)
// #define PERF_COUNTER_BASE ((volatile char *)0xc0000000)

typedef struct {
	uint32_t IE   : 1;
	uint32_t EXL  : 1;
	uint32_t ERL  : 1;
	uint32_t R0   : 1;

	uint32_t UM   : 1;
	uint32_t UX   : 1;
	uint32_t SX   : 1;
	uint32_t KX   : 1;

	uint32_t IM   : 8;

	uint32_t Impl : 2;
	uint32_t _0   : 1;
	uint32_t NMI  : 1;
	uint32_t SR   : 1;
	uint32_t TS   : 1;

	uint32_t BEV  : 1;
	uint32_t PX   : 1;

	uint32_t MX   : 1;
	uint32_t RE   : 1;
	uint32_t FR   : 1;
	uint32_t RP   : 1;
	uint32_t CU   : 4;
} cp0_status_t;

typedef struct {
	uint32_t _5 : 2;
	uint32_t ExcCode : 5;
	uint32_t _4 : 1;
	uint32_t IP : 8;

	uint32_t _3 : 6;
	uint32_t WP : 1;
	uint32_t IV : 1;

	uint32_t _2 : 4;
	uint32_t CE : 2;
	uint32_t _1 : 1;
	uint32_t BD : 1;
} cp0_cause_t;


typedef struct {
  uint32_t revision        : 8;
  uint32_t processor_id    : 8;
  uint32_t company_id      : 8;
  uint32_t company_options : 8;
} cp0_prid_t;


typedef struct {
  uint32_t K0   : 3; // kseg0 coherency algorithms
  uint32_t _0   : 4; // must be zero
  uint32_t MT   : 3; // MMU type
                     // 0 for none
					 // 1 for standard TLB
					 // 2 xxx, 3 xxx
  uint32_t AR   : 3; // 0 for revision 1
  uint32_t AT   : 2; // 0 for mips32,
                     // 1 for mips64 with access only to 32-bit seg
                     // 2 for mips64 with all access to 32-bit seg
					 // 3 reserved
  uint32_t BE   : 1; // 0 for little endian, 1 for big endian
  uint32_t Impl : 15;
  uint32_t M    : 1; // donate that config1 impled at sel 1
} cp0_config_t;

// 1'b0, 6'd15, 3'd1, 3'd5, 3'd0, 3'd2, 3'd5, 3'd0, 7'd0
typedef struct {
  uint32_t FP : 1; // FPU present bit
  uint32_t EP : 1; // EJTAG present bit
  uint32_t CA : 1; // code compression present bit
  uint32_t WR : 1; // watch registers present bit

  uint32_t PC : 1; // performance counter present bit
  uint32_t MD : 1; // not used on mips32 processor
  uint32_t C2 : 1; // coprocessor present bit
  
  uint32_t DA : 3; // dcache associativity
                   // 0 for direct mapped
				   // 2^(DA) ways
				   // ---------------------------
  uint32_t DL : 3; // dcache line size: 
                   // 0 for no icache, 7 reserved
				   // othwise: 2^(DL + 1) bytes
				   // ---------------------------
  uint32_t DS : 3; // dcache sets per way:
                   // 2^(IS + 8)
				   // ---------------------------
  uint32_t IA : 3; // icache associativity
                   // 0 for direct mapped
				   // 2^(IA) ways
				   // ---------------------------
  uint32_t IL : 3; // icache line size: 
                   // 0 for no icache, 7 reserved
				   // othwise: 2^(IL + 1) bytes
				   // ---------------------------
  uint32_t IS : 3; // icache sets per way:
                   // 2^(IS + 8)
				   // ---------------------------
  uint32_t MMU_size : 6; // 0 to 63 indicates 1 to 64 TLB entries
  uint32_t M  : 1; // indicate config 2 is present
} cp0_config1_t;

typedef struct {
  uint32_t _0   : 13;
  uint32_t mask : 16;
  uint32_t _1   :  3;
} cp0_pagemask_t;

// only 4KB page is supported
typedef struct {
  uint32_t asid : 8;
  uint32_t _0   : 5;
  uint32_t vpn  : 19;
} cp0_entry_hi_t;

typedef struct {
  uint32_t g   : 1;
  uint32_t v   : 1;
  uint32_t d   : 1;
  uint32_t c   : 3;
  uint32_t pfn : 24;
  uint32_t _0  : 2;
} cp0_entry_lo_t;

typedef struct {
  uint32_t _0   : 13;
  uint32_t mask : 16;
  uint32_t _1   : 3;
} cp0_page_mask_t;

typedef uint32_t cp0_wired_t;

typedef struct {
  uint32_t idx : 31;
  uint32_t p   : 1;
} cp0_index_t;

#define CP0_INDEX        0
#define CP0_RANDOM       1
#define CP0_ENTRY_LO0    2
#define CP0_ENTRY_LO1    3
#define CP0_CONTEXT      4  // maintained by kernel
#define CP0_PAGEMASK     5
#define CP0_WIRED        6
#define CP0_RESERVED     7  // for extra debug and segment
#define CP0_BASE         7  // for extra debug and segment
#define CP0_BADVADDR     8
#define CP0_COUNT        9
#define CP0_ENTRY_HI     10
#define CP0_COMPARE      11
#define CP0_STATUS       12
#define CP0_CAUSE        13
#define CP0_EPC          14
#define CP0_PRID         15 // sel = 0
#define CP0_EBASE        15 // sel = 1
#define CP0_CONFIG       16

#define CP0_PRID_SEL     0 // sel = 0
#define CP0_EBASE_SEL    1 // sel = 1

#define CP0_TAG_LO       28
#define CP0_TAG_HI       29

#define IP_TIMER_MASK 0x80

#define EXC_INTR    0
#define EXC_TLBM    1
#define EXC_TLBL    2
#define EXC_TLBS    3
#define EXC_AdEL    4
#define EXC_AdES    5
#define EXC_IBE     6
#define EXC_DBE     7
#define EXC_SYSCALL 8
#define EXC_BP      9
#define EXC_RI      10
#define EXC_CPU     11
#define EXC_OV      12
#define EXC_TRAP    13

#define MFC0(dst, src, sel) \
asm volatile("mfc0 %0, $"_STR(src)", %1; nop\n\t":"=r"(dst):"i"(sel))

#define MTC0(dst, src, sel) \
asm volatile("mtc0 %0, $"_STR(dst)", %1; nop\n\t"::"g"(src),"i"(sel))

#define _STR(x) _VAL(x)
#define _VAL(x) #x


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

inline char __attribute__((__always_inline__)) get_kb_stat(){
  volatile char *stat = SERIAL_PORT + SCANCODE_STAT;
  return *stat;
}

inline int in_scancode() {
  volatile int *crecv = (void *)SERIAL_PORT + SCANCODE;
  if(!(get_kb_stat() & 0x1)) return 0;
  else return *crecv;
}

#endif
