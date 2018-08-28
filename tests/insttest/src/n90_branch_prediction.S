#include <asm.h>
#include <regdef.h>
#include <inst_test.h>

LEAF(n90_branch_prediction)
    .set noreorder

	li t2, 0x1234
	li t3, 0x1234
	li t4, DADDR(0000)

###### 1
    sw t2, 0(t4)
	li t0, 0
	li t1, 1
	beq t0, t1, 1f # not taken
	nop
	b 2f
	nop
1:  sw t0, 0(t4)  # should not be executed
2:  lw t2, 0(t4)
	bne t2, t3, inst_error
	nop
    lw t2, 0(t4)
	bne t2, t3, inst_error
	nop

###### 2
    sw t2, 0(t4)
	li t0, 0
	li t1, 0
	beq t0, t1, 1f # taken, assume predicted to not taken
	nop
    sw t0, 0(t4)   # should not be executed
1:  lw t2, 0(t4)
	bne t2, t3, inst_error
	nop
    lw t2, 0(t4)
	bne t2, t3, inst_error
	nop

##################### 3, 4
	li t2, 0x1234
	li t3, 0x1234
	mthi t2
	mtlo t2

###### 3
	li t0, 3
	li t1, 4
	beq t0, t1, 1f # not taken
	nop
	b 2f
	nop
1:  mthi t0  # should not be executed
    mtlo t0
2:  mfhi t3
	bne t2, t3, inst_error
	nop
    mflo t3
	bne t2, t3, inst_error
	nop

###### 4

	li t0, 4
	li t1, 4
	beq t0, t1, 1f # taken, assume predicted to not taken
	nop
    mthi t0        # should not be executed
    mtlo t0        # should not be executed
1:  mfhi t3
	bne t2, t3, inst_error
	nop
    mflo t3
	bne t2, t3, inst_error
	nop

###score ++
    addiu s3, s3, 1
### return
    jr ra
    nop
END(n90_branch_prediction)
