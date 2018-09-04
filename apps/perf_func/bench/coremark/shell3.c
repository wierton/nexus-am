#include <machine.h>
#include <time.h>

void shell3(void)
{
    unsigned long start_count = 0;
    unsigned long stop_count = 0;
    unsigned long total_count = 0;

    int err,i;

    err = 0;
    printf("coremark test begin.\n");
    reset_perf_counters();
    start_count = get_count();
    if(SIMU_FLAG){
	    err = core_mark(0,0,0x66,COREMARK_LOOP,7,1,2000);
    }else{
        for(i=0;i<LOOPTIMES;i++)
	        err += core_mark(0,0,0x66,COREMARK_LOOP,7,1,2000);
    }
    stop_count = get_count();
    total_count = stop_count - start_count;
    print_perf_counters();
    if(err == 0){
        printf("coremark PASS!\n");
	}else{
        printf("coremark ERROR!!!\n");
	}

	printf("coremark: Total Count = 0x%x\n", total_count);

    return;
}
