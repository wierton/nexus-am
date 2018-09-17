#include <am.h>
#include <npc.h>
#include <arch.h>
#include <klib.h>

static _RegSet* (*H) (_Event, _RegSet*) = NULL;

uint8_t am_kstack[16 * 1024];

#if 0
void print_timer() {
  int compare = 0;
  MFC0(compare, CP0_COMPARE, 0);
  int count0 = 0;
  MFC0(count0, CP0_COUNT, 0);
  int count1 = 0;
  MFC0(count1, CP0_COUNT, 1);
  printk("\e[33m[AM]\e[0m: compare:%d, count0:%d, count1:%d\n", compare, count0, count1);
}
#endif

void update_timer(uint32_t step) {
  uint32_t compare = 0;
  MFC0(compare, CP0_COUNT, 0);
  compare += step;
  MTC0(CP0_COMPARE, compare, 0);
}

static void init_timer(int step) {
  int compare = 0;
  MFC0(compare, CP0_COUNT, 0);
  compare += step;
  MTC0(CP0_COMPARE, compare, 0);
}

static inline void flush_cache(void *begin, void *end) {
  for(void *p = begin; p < end; p += 4) {
	asm volatile("cache 0x10,0(%0)" ::"r"(p));
	asm volatile("cache 0x15,0(%0)" ::"r"(p));
  }
}

static inline void set_handler(unsigned offset, void *addr, int size) {
  memcpy(EBASE + offset, addr, size);
  flush_cache(EBASE + offset, EBASE + offset + size);
}

void *get_exception_entry() {
  static uint8_t __attribute__((unused, section(".ex_entry.1"))) data[0];
  return (void *)data;
}

uint32_t get_exception_entry_size() {
  static uint8_t __attribute__((unused, section(".ex_entry.3"))) data[0];
  return (void  *)data - get_exception_entry();
}

static inline void setup_bev() {
  cp0_status_t c0_status;
  asm volatile("mfc0 %0, $%1" :: "r"(c0_status), "i"(CP0_STATUS));
  c0_status.BEV = 1;
  asm volatile("mtc0 %0, $%1" :: "r"(c0_status), "i"(CP0_STATUS));
}

static inline void enable_interrupt() {
  cp0_status_t c0_status;
  asm volatile("mfc0 %0, $%1" :: "r"(c0_status), "i"(CP0_STATUS));
  c0_status.IM  = 0xFF;
  c0_status.ERL = 0;
  c0_status.IE  = 1;
  asm volatile("mtc0 %0, $%1" :: "r"(c0_status), "i"(CP0_STATUS));
}

int _asye_init(_RegSet* (*l)(_Event ev, _RegSet *regs)){
  H = l; // set asye handler

  setup_bev();

  void *entry = get_exception_entry();
  uint32_t size = get_exception_entry_size();

  set_handler(0x000, entry, size); // TLB
  set_handler(0x180, entry, size); // EXCEPTION
  set_handler(0x200, entry, size); // INTR
  set_handler(0x380, entry, size); // LOONGSON
  return 0;
}

_RegSet *_make(_Area kstack, void (*entry)(void *), void *args){
  _RegSet *regs = (_RegSet *)kstack.start;
  regs->sp = (uint32_t) kstack.end;
  regs->epc = (uint32_t) entry;

  static const char *envp[] = { "AM=true", NULL };

  uintptr_t *arg = args;
  regs->a0 = 0;
  regs->a1 = (uintptr_t)arg;
  regs->a2 = (uintptr_t)envp;
  for(; *arg; arg ++, regs->a0++);
  return regs;
}

void _yield(){
  init_timer(INTERVAL);
  // enable_interrupt();
  asm volatile("nop; li $a0, -1; syscall; nop");
}

int _intr_read() {
  int status = 0;
  MFC0(status, CP0_STATUS, 0);
  return status & 0x1;
}

void _intr_write(int enable) {
  int status = 0;
  MFC0(status, CP0_STATUS, 0);
  status = status ^ (!!enable + 0);
  MTC0(CP0_STATUS, status, 0); 
}

void irq_handle(struct _RegSet *regs){
  cp0_cause_t *cause = (void*)&(regs->cause);
  uint32_t exccode = cause->ExcCode;
  uint32_t ipcode = cause->IP;
  
  _Event ev;
  ev.event = _EVENT_NULL;
  //TODO: exception handling
  // Delayslot should be considered when handle exceptions !!!
  update_timer(INTERVAL); // update when exception happens
  switch(exccode){
    case EXC_INTR: {
      if(ipcode & IP_TIMER_MASK) {
          ev.event = _EVENT_IRQ_TIMER;
		  cause->IP = 0;
		  asm volatile("mtc0 %0, $13, 0;nop;nop"::"r"(regs->cause));
	  } else {
		  printk("invalid ipcode = %x\n", ipcode);
		  _halt(-1);
      }
      break;
    }
    case EXC_SYSCALL:
      regs->epc += 4;
	  if(regs->a0 == -1)
		ev.event = _EVENT_YIELD;
	  else
		ev.event = _EVENT_SYSCALL;
      break;
    case EXC_TRAP:
      ev.event = _EVENT_SYSCALL;
      break;
	case EXC_TLBM:
	case EXC_TLBL:
	case EXC_TLBS:
		ev.event = _EVENT_PAGEFAULT;
		break;
    case EXC_AdEL:
    case EXC_AdES:
    case EXC_BP:
    case EXC_RI:
    case EXC_OV:
    default:
	  printk("unhandled exccode = %x, epc:%08x, badvaddr:%08x\n", exccode, regs->epc, regs->badvaddr);
	  _halt(-1);
  }

  _RegSet *ret = regs;
  if(H) {
	  _RegSet *next = H(ev, regs);
	  if(next != NULL) ret = next;
  }

  // restore common registers
  asm volatile(
	".set noat;"
    "nop;"
    "lw $at, %0;"  "lw $v0, %1;"
    "lw $a0, %2;"  "lw $a1, %3;"  "lw $a2, %4;" "lw $a3, %5;"
    "lw $t0, %6;"  "lw $t1, %7;"  "lw $t2, %8;" "lw $t3, %9;"
    "lw $t4, %10;" "lw $t5, %11;" "lw $t6, %12;" "lw $t7, %13;"
    "lw $s0, %14;" "lw $s1, %15;" "lw $s2, %16;" "lw $s3, %17;"
    "lw $s4, %18;" "lw $s5, %19;" "lw $s6, %20;" "lw $s7, %21;"
    "lw $t8, %22;" "lw $t9, %23;"
    "lw $gp, %24;" "lw $fp, %25;" "lw $ra, %26;" "lw $sp, %27;"
    : : 
    "m"(ret->at), "m"(ret->v0),
    "m"(ret->a0), "m"(ret->a1), "m"(ret->a2), "m"(ret->a3),
    "m"(ret->t0), "m"(ret->t1), "m"(ret->t2), "m"(ret->t3),
    "m"(ret->t4), "m"(ret->t5), "m"(ret->t6), "m"(ret->t7),
    "m"(ret->s0), "m"(ret->s1), "m"(ret->s2), "m"(ret->s3),
    "m"(ret->s4), "m"(ret->s5), "m"(ret->s6), "m"(ret->s7),
    "m"(ret->t8), "m"(ret->t9),
    "m"(ret->gp), "m"(ret->fp), "m"(ret->ra), "m"(ret->sp)
    :"at",
     "v0",
     "a0","a1","a2","a3",
     "t0","t1","t2","t3",
     "t4","t5","t6","t7",
     "s0","s1","s2","s3","s4","s5","s6","s7",
     "t8","t9",
     "fp","ra","sp"
    );

  // restore cp0 registers
  asm volatile(
    "lw $k0, %0;"   // ld epc to k0
    "nop;"
    "nop;"
    "mtc0 $k0, $14;" // mtc0 epc
    "nop;"
    "nop;"
    "lw $k0, %1;"   // ld base t0 k0
    "mtc0 $k0, $7;"  // mtc0 base
    "nop;"
    "nop;"
	"mthi $0; mtlo $0; li $k0, 0; li $k1, 0;" // for diff
    "lw $v1, %2;" // used by assembler, must be load at last
    "eret;"
    : : 
    "m"(ret->epc),
    "m"(ret->base),
    "m"(ret->v1)
	);
}
