#include <am.h>
#include <npc.h>
#include <arch.h>
#include <klib.h>

static _RegSet* (*H) (_Event, _RegSet*) = NULL;

uint8_t am_kstack[16 * 1024];
uint32_t am_kstack_size = sizeof(am_kstack);

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
  enable_interrupt();
  asm volatile("nop; mthi $0; mtlo $0; li $a0, -1; syscall; nop");
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
    "lw $v0, %[v0];" "lw $at, %[at];"
    "lw $a0, %[a0];" "lw $a1, %[a1];" "lw $a2, %[a2];" "lw $a3, %[a3];"
    "lw $t0, %[t0];" "lw $t1, %[t1];" "lw $t2, %[t2];" "lw $t3, %[t3];"
    "lw $t4, %[t4];" "lw $t5, %[t5];" "lw $t6, %[t6];" "lw $t7, %[t7];"
    "lw $s0, %[s0];" "lw $s1, %[s1];" "lw $s2, %[s2];" "lw $s3, %[s3];"
    "lw $s4, %[s4];" "lw $s5, %[s5];" "lw $s6, %[s6];" "lw $s7, %[s7];"
    "lw $t8, %[t8];" "lw $t9, %[t9];"
    "lw $gp, %[gp];" "lw $fp, %[fp];" "lw $ra, %[ra];" "lw $sp, %[sp];"
    : : 
    [v0]"m"(ret->v0), [at]"m"(ret->at),
    [a0]"m"(ret->a0), [a1]"m"(ret->a1), [a2]"m"(ret->a2), [a3]"m"(ret->a3),
    [t0]"m"(ret->t0), [t1]"m"(ret->t1), [t2]"m"(ret->t2), [t3]"m"(ret->t3),
    [t4]"m"(ret->t4), [t5]"m"(ret->t5), [t6]"m"(ret->t6), [t7]"m"(ret->t7),
    [s0]"m"(ret->s0), [s1]"m"(ret->s1), [s2]"m"(ret->s2), [s3]"m"(ret->s3),
    [s4]"m"(ret->s4), [s5]"m"(ret->s5), [s6]"m"(ret->s6), [s7]"m"(ret->s7),
    [t8]"m"(ret->t8), [t9]"m"(ret->t9),
    [gp]"m"(ret->gp), [fp]"m"(ret->fp), [ra]"m"(ret->ra), [sp]"m"(ret->sp)
    :"v0","at",
     "a0","a1","a2","a3",
     "t0","t1","t2","t3",
     "t4","t5","t6","t7",
     "s0","s1","s2","s3","s4","s5","s6","s7",
     "t8","t9",
     "fp","ra","sp"
    );

  asm volatile(
    "lw $k0, %[epc];  nop; mtc0 $k0, $%[epc_no];  nop;"
    "lw $k0, %[base]; nop; mtc0 $k0, $%[base_no]; nop;"
	"lw $k0, %[hi]; mthi $k0; nop;"
	"lw $k0, %[lo]; mtlo $k0; nop;"
    "lw $v1, %[v1];"
	"eret;"
	: :
    [epc]"m"(ret->epc),
    [base]"m"(ret->base),
    [hi]"m"(ret->hi), [lo]"m"(ret->lo),
    [v1]"m"(ret->v1),
	[epc_no]"i"(CP0_EPC),
	[base_no]"i"(CP0_BASE)
	);
}
