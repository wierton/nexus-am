#include <machine.h>

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

int range = 2 * nPerfCounters;
unsigned int *perf_counter_base = (unsigned int *)0xa0000000;

unsigned perf_counters[2 * nPerfCounters];

void reset_perf_counters(void) {
  int i = 0;
  for (i = 0; i < range; i++)
    perf_counter_base[i] = 0;
}

unsigned long _get_count() {
  return perf_counter_base[2 * cycle];
}

void print_perf_counter(char *str, int index) {
  printf("%s: ", str);
  unsigned int high = perf_counters[2 * index + 1];
  unsigned int low = perf_counters[2 * index];
  if (high != 0)
    printf("0x%x%x", high, low);
  else
    printf("0x%x", low);
  printf("\n");
}

void print_perf_counters(void) {
  int i = 0;
  for (i = 0; i < range; i++)
    perf_counters[i] = perf_counter_base[i];

  printf("\n\n\n**************************************************\n");

  print_perf_counter("cycle", cycle);
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
  printf("**************************************************\n\n\n\n");
}
