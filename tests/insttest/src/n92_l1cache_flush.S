#include <asm.h>
#include <regdef.h>
#include <inst_test.h>

#define CACHE_OP(op, id) (((op) << 2) | ((id) & 3))

# a0: st, a1: ed, a2: target
copy_words:
	sltu    v0,a0,a1
	beqz    v0,2f
1:	nop
	lw      v1,0(a0)
	sw      v1,0(a2)
	addiu   a0,a0,4
	addiu   a2,a2,4
	bltu    a0,a1, 1b
	nop
2:	jr      ra
	nop
	nop

# a0: st, a1: ed
flush_cache:
	sltu    v0,a0,a1
	beqz    v0,2f
	nop
1:	cache   0x10,0(a0)
	cache   0x15,0(a0)
	addiu   a0,a0,4
	sltu    v0,a0,a1
	bnez    v0,1b
	nop
2:	jr      ra
	nop
	nop

.balign 16, 0
override_start:
	li t0, 0x1234
	jr ra
	nop
.balign 16, 0
override_end:

LEAF(n92_cache_flush)
    .set noreorder

	move t4, ra # store ra
	la ra, func_end

	# execute it to cache the func_start in icache
.balign 16, 0
func_start:
	li t0, 1
	jr ra
	nop
.balign 16, 0
func_end:
	
	la a0, override_start
	la a1, override_end
	la a2, func_start
	jal copy_words
	nop

	la a0, func_start
	la a1, func_end
	jal flush_cache
	nop

	la ra, 1f
	jal func_start
	nop
1:	li t1, 0x1234
	bne t0, t1, inst_error
	nop
	move ra, t4 # resume ra

###score ++
    addiu s3, s3, 1
### return
    jr ra
    nop
END(n92_cache_flush)
