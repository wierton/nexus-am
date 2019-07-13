#include "trap.h"

#define KB 1024
#define MB 1024 * 1024

void fast_srand(unsigned int *g_seed, int seed) {
  *g_seed = seed;
}

int fast_rand(unsigned int *g_seed) {
  *g_seed = (214013 * *g_seed + 2531011);
  return *g_seed;
}

static volatile uint32_t arr[4 * MB] = {0};
static volatile char flag[4 * MB] = {0};

#define cache_flush(seed)                               \
  uint32_t mask = 0xfffffff1;                           \
  printf("Step 1\n");                                   \
  fast_srand(&g_seed, seed);                            \
  for (int j = 0; j < 1; j++) {                         \
    for (s = 0; s < sizeof(sizes) / sizeof(int); s++) { \
      lengthMod = sizes[s] - 1;                         \
      for (unsigned int i = 0; i < steps; i++) {        \
        int idx = ((fast_rand(&g_seed) & mask) |        \
                   (mhartid << 1)) &                    \
                  lengthMod;                            \
        int result = fast_rand(&g_seed);                \
        if (!flag[idx]) {                               \
          arr[idx] = result;                            \
          flag[idx] = 1;                                \
        }                                               \
      }                                                 \
    }                                                   \
  }                                                     \
                                                        \
  printf("Step 2\n");                                   \
  fast_srand(&g_seed, seed);                            \
  for (int j = 0; j < 1; j++) {                         \
    for (s = 0; s < sizeof(sizes) / sizeof(int); s++) { \
      lengthMod = sizes[s] - 1;                         \
      for (unsigned int i = 0; i < steps; i++) {        \
        int idx = ((fast_rand(&g_seed) & mask) |        \
                   (mhartid << 1)) &                    \
                  lengthMod;                            \
        fast_rand(&g_seed);                             \
        flag[idx] = 0;                                  \
      }                                                 \
    }                                                   \
  }                                                     \
                                                        \
  printf("Step 3\n");                                   \
  fast_srand(&g_seed, seed);                            \
  for (int j = 0; j < 1; j++) {                         \
    for (s = 0; s < sizeof(sizes) / sizeof(int); s++) { \
      lengthMod = sizes[s] - 1;                         \
      for (unsigned int i = 0; i < steps; i++) {        \
        int idx = ((fast_rand(&g_seed) & mask) |        \
                   (mhartid << 1)) &                    \
                  lengthMod;                            \
        int result = fast_rand(&g_seed);                \
        if (!flag[idx]) {                               \
          nemu_assert(arr[idx] == result);              \
          flag[idx] = 1;                                \
        }                                               \
      }                                                 \
    }                                                   \
  }                                                     \
  printf("Step 4\n");                                   \
  fast_srand(&g_seed, seed);                            \
  for (int j = 0; j < 1; j++) {                         \
    for (s = 0; s < sizeof(sizes) / sizeof(int); s++) { \
      lengthMod = sizes[s] - 1;                         \
      for (unsigned int i = 0; i < steps; i++) {        \
        int idx = ((fast_rand(&g_seed) & mask) |        \
                   (mhartid << 1)) &                    \
                  lengthMod;                            \
        fast_rand(&g_seed);                             \
        flag[idx] = 0;                                  \
      }                                                 \
    }                                                   \
  }

int main() {
  unsigned int g_seed;
  unsigned int steps = 1024;
  int lengthMod;
  int mhartid = 0;
  int sizes[] = {4 * MB};
  // assume at most 8cores, cache line size = 64byte, that
  // is 16 uint32 different core access different part of
  // cache line
  int s = 0;
  for (int k = 0; k < 10; k++) {
    printf("Round %d started\n", k);
    cache_flush(214013 * k + 2531011);
    printf("Round %d passed\n", k);
  }
  return 0;
}
