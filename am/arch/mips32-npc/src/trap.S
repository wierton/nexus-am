.extern irq_handle
.extern _halt
.extern am_kstack
.extern am_kstack_size
.set noat

.globl _trap_entry
.type _trapPentry function

#define CP0_INDEX        0
#define CP0_RANDOM       1
#define CP0_ENTRY_LO0    2
#define CP0_ENTRY_LO1    3
#define CP0_CONTEXT      4  // maintained by kernel
#define CP0_PAGEMASK     5
#define CP0_WIRED        6
#define CP0_BASE         7  // for extra debug and segment
#define CP0_BADVADDR     8
#define CP0_COUNT        9
#define CP0_ENTRY_HI     10
#define CP0_COMPARE      11
#define CP0_STATUS       12
#define CP0_CAUSE        13
#define CP0_EPC          14
#define CP0_PRID         15 // sel = 0
#define CP0_EBASE        15 // sel = 1
#define CP0_CONFIG       16

.section .ex_entry.2
	la $k0, _trap_entry
	jr $k0
	nop

.section .text
_trap_entry:
	mfc0	$k0, $14     # read c0_epc
	nop
	lui     $k1,0xe000
	and     $k1,$k0,$k1
	lui     $k0,0x8000
	addu    $k1,$k1,$k0
	sltiu   $k1,$k1,1
	bnez	$k1, use_sp  # judge if exception in kernel mode
	nop

use_kstack:
# pusha, save all user registers
    la		$k0, am_kstack
    la		$k1, am_kstack_size // set kern stack
	lw		$k1, 0($k1)
	addu	$k0, $k0, $k1
	b		save_gpr
	nop

use_sp:
	addiu	$k0, $sp, 0
	b		save_gpr
	nop

save_gpr:
	addiu	$k0, $k0,-0xa0
	sw 		$at, 0x4($k0)
	sw 		$v0, 0x8($k0)
	sw 		$v1, 0xc($k0)
	sw 		$a0, 0x10($k0)
	sw 		$a1, 0x14($k0)
	sw 		$a2, 0x18($k0)
	sw 		$a3, 0x1c($k0)
	sw 		$t0, 0x20($k0)
	sw 		$t1, 0x24($k0)
	sw 		$t2, 0x28($k0)
	sw 		$t3, 0x2c($k0)
	sw 		$t4, 0x30($k0)
	sw 		$t5, 0x34($k0)
	sw 		$t6, 0x38($k0)
	sw 		$t7, 0x3c($k0)
	sw 		$t8, 0x40($k0)
	sw 		$t9, 0x44($k0)
	sw 		$s0, 0x48($k0)
	sw 		$s1, 0x4c($k0)
	sw 		$s2, 0x50($k0)
	sw 		$s3, 0x54($k0)
	sw 		$s4, 0x58($k0)
	sw 		$s5, 0x5c($k0)
	sw 		$s6, 0x60($k0)
	sw 		$s7, 0x64($k0)
	sw 		$k0, 0x68($k0)
	sw 		$k1, 0x6c($k0)
	sw 		$gp, 0x70($k0)
	sw		$sp, 0x74($k0)
	sw 		$fp, 0x78($k0)
	sw 		$ra, 0x7c($k0)

	addu	$sp, $k0, $0

	nop
	nop
	mfc0 $k0, $CP0_EPC;     nop; sw $k0, 0x80($sp); nop;
	mfc0 $k0, $CP0_CAUSE;   nop; sw $k0, 0x84($sp); nop;
	mfc0 $k0, $CP0_STATUS;  nop; sw $k0, 0x88($sp); nop;
	mfc0 $k0, $CP0_BADVADDR;nop; sw	$k0, 0x8c($sp); nop;
	mfc0 $k0, $CP0_BASE;    nop; sw	$k0, 0x90($sp); nop;

	mfhi $k0; sw $k0, 0x94($sp); nop
	mflo $k0; sw $k0, 0x98($sp); nop
# give trapframe pointer to a0
	addiu   $a0, $sp, 0x4
# jmp to irq_handle
	jal		irq_handle
	nop

# eret by irq_handle, should not reach here
	addiu	$a0, $0, 1
	jal		_halt
