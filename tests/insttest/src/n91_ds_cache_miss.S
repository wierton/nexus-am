#include <asm.h>
#include <regdef.h>
#include <inst_test.h>

LEAF(n91_ds_cache_miss)
    .set noreorder

	li t0, 0
	li t1, 0
	li t2, 0x1234
.balignl 0x20,0
	nop; nop; nop; nop;    nop; nop; nop; nop;
	nop; nop; nop; nop;    nop; nop; nop; nop;
	nop; nop; nop; nop;    nop; nop; nop; nop;
	nop; nop; nop; nop;    nop; nop; nop; nop;
	nop; nop; nop; nop;    nop; nop; nop; nop;
	nop; nop; nop; nop;    nop; nop; nop; nop;
	nop; nop; nop; nop;    nop; nop; nop; nop;
	nop; nop; nop; nop;    nop; nop; nop;
	beq t0, t0, 1f
#   icache boundary
	li t1, 0x1234
1:  nop
	bne t1, t2, inst_error
	nop

###score ++
    addiu s3, s3, 1
### return
    jr ra
    nop
END(n91_ds_cache_miss)
