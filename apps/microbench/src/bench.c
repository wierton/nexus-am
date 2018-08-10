#include <am.h>
#include <benchmark.h>
#include <limits.h>

Benchmark *current;
Setting *setting;

// #define PERF

static char *start;

#define ARR_SIZE(a) (sizeof((a)) / sizeof((a)[0]))

// The benchmark list

#define ENTRY(_name, _sname, _s1, _s2, _desc) \
  { .prepare = bench_##_name##_prepare, \
    .run = bench_##_name##_run, \
    .validate = bench_##_name##_validate, \
    .name = _sname, \
    .desc = _desc, \
    .settings = {_s1, _s2}, },

Benchmark benchmarks[] = {
  BENCHMARK_LIST(ENTRY)
};

void reset_cycle();
uint64_t my_cycle();
unsigned long my_uptime();
void print_perf_counters(void);

// Running a benchmark
static void bench_prepare(Result *res) {
  res->msec = my_uptime();
}

static void bench_done(Result *res) {
  res->msec = my_uptime() - res->msec;
}

static const char *bench_check(Benchmark *bench) {
  unsigned long freesp = (unsigned long)_heap.end - (unsigned long)_heap.start;
  if (freesp < setting->mlim) {
    return "(insufficient memory)";
  }
  return NULL;
}

void run_once(Benchmark *b, Result *res) {
  bench_reset();       // reset malloc state
  current->prepare();  // call bechmark's prepare function
  bench_prepare(res); // clean everything, start timer
  current->run();      // run it
  bench_done(res);    // collect results
  res->pass = current->validate();
}

unsigned long score(Benchmark *b, unsigned long tsc, unsigned long msec) {
  if (msec == 0) return 0;
  return (REF_SCORE / 1000) * setting->ref / msec;
}

int main() {
  reset_cycle();
  _ioe_init();
  /*
  volatile int a;
  for (int i = 0; i <= 50000000; i++) {
    a += 1;
  }
#ifdef PERF
  print_perf_counters();
#endif
  return 0;
  */
  unsigned long bench_score = 0;
  int pass = 1;

  for (int i = 0; i < ARR_SIZE(benchmarks); i ++) {
    Benchmark *bench = &benchmarks[i];
    current = bench;
    setting = &bench->settings[SETTING];
    const char *msg = bench_check(bench);
    printk("[%s] %s: ", bench->name, bench->desc);
    if (msg != NULL) {
      printk("Ignored %s\n", msg);
    } else {
      unsigned long msec = ULONG_MAX;
      int succ = 1;
      for (int i = 0; i < REPEAT; i ++) {
        Result res;
        run_once(bench, &res);
        printk(res.pass ? "*" : "X");
        succ &= res.pass;
        if (res.msec < msec) msec = res.msec;
      }

      if (succ) printk(" Passed.");
      else printk(" Failed.");

      pass &= succ;
      unsigned long cur = score(bench, 0, msec);

      printk("\n");
      if (SETTING != 0) {
        printk("  min time: %d ms [%d]\n", (unsigned int)msec, (unsigned int)cur);
      }

      bench_score += cur;
    }
  }

  bench_score /= sizeof(benchmarks) / sizeof(benchmarks[0]);

  printk("==================================================\n");
  printk("MicroBench %s", pass ? "PASS" : "FAIL");
  if (SETTING != 0) {
    printk("        %d Marks\n", (unsigned int)bench_score);
    printk("                   vs. %d Marks (%s)\n", REF_SCORE, REF_CPU);
  } else {
    printk("\n");
  }
#ifdef PERF
  print_perf_counters();
#endif
  _halt(0);
  return 0;
}

// Library
void* bench_alloc(size_t size) {
  if ((uintptr_t)start % 16 != 0) {
    start = start + 16 - ((uintptr_t)start % 16);
  }
  char *old = start;
  start += size;
  assert((uintptr_t)_heap.start <= (uintptr_t)start && (uintptr_t)start < (uintptr_t)_heap.end);
  for (char *p = old; p != start; p ++) *p = '\0';
  assert((uintptr_t)start - (uintptr_t)_heap.start <= setting->mlim);
  return old;
}

void bench_free(void *ptr) {
}

void bench_reset() {
  start = (char*)_heap.start;
}

static int32_t seed = 1;

void bench_srand(int32_t _seed) {
  seed = _seed & 0x7fff;
}

int32_t bench_rand() {
  seed = (seed * (int32_t)214013L + (int32_t)2531011L);
  return (seed >> 16) & 0x7fff;
}

// FNV hash
uint32_t checksum(void *start, void *end) {
  const int32_t x = 16777619;
  int32_t hash = 2166136261u;
  for (uint8_t *p = (uint8_t*)start; p + 4 < (uint8_t*)end; p += 4) {
    int32_t h1 = hash;
    for (int i = 0; i < 4; i ++) {
      h1 = (h1 ^ p[i]) * x;
    }
    hash = h1;
  }
  hash += hash << 13;
  hash ^= hash >> 7;
  hash += hash << 3;
  hash ^= hash >> 17;
  hash += hash << 5;
  return hash;
}

// ******** timer and performance counter code ********
// performance counters
enum {
  instr,
  frontend_stall_cycles,
  backend_stall_cycles,
  frontend_backend_stall_cycles,
  // frontend performance
  // branch prediction
  branch,
  branch_misprediction,
  branch_btb_miss,
  br,
  br_misprediction,
  br_btb_miss,
  j,
  j_misprediction,
  j_btb_miss,
  call,
  call_misprediction,
  call_btb_miss,
  ret,
  ret_misprediction,
  ret_btb_miss,
  // icache performance
  icache_access,
  icache_miss,
  icache_miss_cycles,
  icache_miss_backend_busy_cycles,
  // backend performance
  backend_starving,
  operand_not_ready,
  rd_busy,
  backend_mdu_busy,
  backend_lsu_busy,
  // dcache performance
  dcache_access,
  dcache_miss,
  lsu_wait_dcache,
  dcache_load,
  dcache_load_miss,
  dcache_store,
  dcache_store_miss,
  cycle,
  nPerfCounters
};

uint64_t *perf_counter_base = (uint64_t *)0xc0000000;

uint64_t perf_counters[nPerfCounters];

void print64(uint64_t a) {
  uint32_t high = a >> 32;
  uint32_t low = a;
  printf("0x");
  if (high != 0U)
    printf("%x", high);
  printf("%x", low);
}


void print_perf_counter(char *str, int index) {
  printf("%s: ", str);
  print64(perf_counters[index]);
  printf("\n");
}

void print_perf_counters(void) {
  for (int i = 0; i < nPerfCounters; i++)
    perf_counters[i] = perf_counter_base[i];

  printf("cycle: ");
  print64(my_cycle());
  printf("\n");

  print_perf_counter("instr", instr);
  print_perf_counter("frontend_stall_cycles", frontend_stall_cycles);
  print_perf_counter("backend_stall_cycles", backend_stall_cycles);
  print_perf_counter("frontend_backend_stall_cycles", frontend_backend_stall_cycles);
  // frontend performance
  // branch prediction
  print_perf_counter("branch", branch);
  print_perf_counter("branch_misprediction", branch_misprediction);
  print_perf_counter("branch_btb_miss", branch_btb_miss);
  print_perf_counter("br", br);
  print_perf_counter("br_misprediction", br_misprediction);
  print_perf_counter("br_btb_miss", br_btb_miss);
  print_perf_counter("j", j);
  print_perf_counter("j_misprediction", j_misprediction);
  print_perf_counter("j_btb_miss", j_btb_miss);
  print_perf_counter("call", call);
  print_perf_counter("call_misprediction", call_misprediction);
  print_perf_counter("call_btb_miss", call_btb_miss);
  print_perf_counter("ret", ret);
  print_perf_counter("ret_misprediction", ret_misprediction);
  print_perf_counter("ret_btb_miss", ret_btb_miss);
  // icache performance
  print_perf_counter("icache_access", icache_access);
  print_perf_counter("icache_miss", icache_miss);
  print_perf_counter("icache_miss_cycles", icache_miss_cycles);
  print_perf_counter("icache_miss_backend_busy_cycles", icache_miss_backend_busy_cycles);
  // backend performance
  print_perf_counter("backend_starving", backend_starving);
  print_perf_counter("operand_not_ready", operand_not_ready);
  print_perf_counter("rd_busy", rd_busy);
  print_perf_counter("backend_mdu_busy", backend_mdu_busy);
  print_perf_counter("backend_lsu_busy", backend_lsu_busy);
  // dcache performance
  print_perf_counter("dcache_access", dcache_access);
  print_perf_counter("dcache_miss", dcache_miss);
  print_perf_counter("lsu_wait_dcache", lsu_wait_dcache);
  print_perf_counter("dcache_load", dcache_load);
  print_perf_counter("dcache_load_miss", dcache_load_miss);
  print_perf_counter("dcache_store", dcache_store);
  print_perf_counter("dcache_store_miss", dcache_store_miss);
}

// ******** make reading cp0 register much easier ********
#define NOP() \
  asm volatile("nop")

#define _STR(x) #x

// use NOP to avoid pipeline harzards on cp0 registers
#define MFC0(dst, src, sel) \
  asm volatile("mfc0 %0, $"_STR(src)", %1\n\t":"=r"(dst):"i"(sel)); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP()

#define MTC0(dst, src, sel) \
  asm volatile("mtc0 %0, $"_STR(dst)", %1\n\t"::"r"(src),"i"(sel)); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP(); \
  NOP()

#define HZ 80000000

// timer and performance counters
#define cycle  9
#define cycle_sel  0

unsigned long my_uptime() {
  // for 32bit machines: sizeof(unsigned long) is 32bit!!!
  //50MHZ
  // time (ms) = HIGH * 1000 * (2^32) / HZ + LOW * 1000 / HZ
  // ** be careful of overflow **
#ifdef PERF
  uint32_t counter_reg1 = 0, counter_reg0 = 0, tmp = 0;
  do {
    MFC0(counter_reg1, cycle, 1);
    MFC0(counter_reg0, cycle, 0);
    MFC0(tmp, cycle, 1);
  } while(counter_reg1 != tmp);
  unsigned long npc_time = counter_reg1 * 1000 * ((1ul << 31) / HZ) * 2 + counter_reg0 / (HZ / 1000);
  return npc_time;
#else
  return 0UL;
#endif
}

uint64_t my_cycle() {
#ifdef PERF
  //1. Read the upper 32-bit timer/counter register (TCR1).
  //2. Read the lower 32-bit timer/counter register (TCR0).
  //3. Read the upper 32-bit timer/counter register (TCR1) again. If the value is different from
  //the 32-bit upper value read previously, go back to previous step (reading TCR0).
  //Otherwise 64-bit timer counter value is correct.
  uint32_t counter_reg1 = 0, counter_reg0 = 0, tmp = 0;
  do {
    MFC0(counter_reg1, cycle, 1);
    MFC0(counter_reg0, cycle, 0);
    MFC0(tmp, cycle, 1);
  } while(counter_reg1 != tmp);
  return (uint64_t)counter_reg1 << 32 | (uint64_t)counter_reg0;
#else
  return 0UL;
#endif
}

void reset_cycle() {
  MTC0(cycle, 0, 1);
  MTC0(cycle, 0, 0);
}
