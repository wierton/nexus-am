#include <machine.h>
#include <time.h>

void shell1(void)
{
    unsigned long start_count = 0;
    unsigned long stop_count = 0;
    unsigned long total_count = 0;

    int i,n,err;

    err=0;

    printf("bitcount test begin.\n");

    reset_perf_counters();
    start_count = get_count();
    if(SIMU_FLAG){
        n = bitcnts(1, 100);
        err = n!=811;
    }else{
        for(i=0;i<LOOPTIMES;i++){
            n = bitcnts(1, 100);
            err += n!=811;
        }
    }
    stop_count = get_count();
    total_count = stop_count - start_count;

    print_perf_counters();

	if (err==0) {
        printf("bitcount PASS!");
    }
	else {
        printf("bitcount ERROR!!!");
    }
	printf("Bits: %ld\n", n);
	printf("bitcount: Total Count = 0x%x\n", total_count);

    return;
}
