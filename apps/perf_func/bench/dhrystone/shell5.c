#include <machine.h>
#include <time.h>

void shell5(void)
{
    unsigned long start_count = 0;
    unsigned long stop_count = 0;
    unsigned long total_count = 0;

    int i,err;

    err = 0;
    printf("dhrystone test begin.\n");
    reset_perf_counters();
    start_count = get_count();
    if(SIMU_FLAG){
        err = dhrystone(RUNNUMBERS);
    }else{
        for(i=0;i<LOOPTIMES;i++)
             err += dhrystone(RUNNUMBERS);
    }
    stop_count = get_count();
    total_count = stop_count - start_count;
    print_perf_counters();
	if(err == 0){
        printf("dhrystone PASS!\n");
	}else{
        printf("dhrystone ERROR!!!\n");
	}

	printf("dhrystone: Total Count = 0x%x\n", total_count);

    return;
}
