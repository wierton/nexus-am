#include <time.h>
#include <machine.h>

unsigned long get_count(void);

void shell10(void)
{
    unsigned long start_count = 0;
    unsigned long stop_count = 0;
    unsigned long total_count = 0;

    int err,i;

    err = 0;
    printf("string search test begin.\n");
    reset_perf_counters();
    start_count = get_count();
    if(SIMU_FLAG){
        err = search_small();
    }else{
        for(i=0; i<LOOPTIMES; i++)
            err += search_small();
    }
    stop_count = get_count();
    total_count = stop_count - start_count;
    print_perf_counters();

	if(err == 0){
        printf("string search PASS!\n");
	}else{
        printf("string search ERROR!!!\n");
	}

    printf("string search: Total Count = 0x%x\n", total_count);

    return;
}
