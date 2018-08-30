#include <asm.h>
#include <regdef.h>
#include <cpu_cde.h>

#define TEST_NUM 91

#define EXT_8TO16(t2, t3) \

##s0, number
##s1, number adress 
##s2, exception use
##s3, score
##s4, exception pc
	.set   noreorder
	.globl _start
_start:
	j main
	nop

/*
 *  exception handle
 */
.org 0x20
exception_vector_entry:
    mfhi k0
    mflo k1
    li  k0, DADDR(0000)
    lw  k1, 0x0(k0)
    li  k0, 0x01 # syscall
    beq k1, k0, syscall_ex
    nop
    li  k0, 0x02 # break
    beq k1, k0, break_ex
    nop
    li  k0, 0x03 # overflow
    beq k1, k0, overflow_ex
    nop
    li  k0, 0x04 # adel(load)
    beq k1, k0, adel_load_ex
    nop
    li  k0, 0x05 # ades
    beq k1, k0, ades_ex
    nop
    li  k0, 0x06 # adel(inst fetch)
    beq k1, k0, adel_if_ex
    nop
    li  k0, 0x07 # reserved inst
    beq k1, k0, reserved_inst_ex
    nop
    li  k0, 0x08 # int
    beq k1, k0, int_ex
    nop

syscall_ex:
    addu  s2, zero, zero
    mfc0 k0, c0_epc
    bne  k0, s4, ex_finish
    nop
    mfc0 k0, c0_cause
    andi k0, k0, 0x7c # 6..2
    li   k1, 0x20 # 010_0000
    bne  k0, k1, ex_finish
    nop
    mfc0 k0, c0_status
    andi k0, k0, 0x02
    li   k1, 0x02
    bne  k0, k1, ex_finish
    nop
    lui  s2, 0x1
    b ex_finish
    nop

break_ex:
    addu  s2, zero, zero
    mfc0 k0, c0_epc
    bne  k0, s4, ex_finish
    nop
    mfc0 k0, c0_cause
    andi k0, k0, 0x7c # 6..2
    li   k1, 0x24 # 010_0100
    bne  k0, k1, ex_finish
    nop
    mfc0 k0, c0_status
    andi k0, k0, 0x02 # exl
    li   k1, 0x02 # exl = 1
    bne  k0, k1, ex_finish
    nop
    lui  s2, 0x2
    b ex_finish
    nop

overflow_ex:
    addu s2, zero, zero 
    mfc0 k0, c0_epc
    bne  k0, s4, ex_finish
    mfc0 k0, c0_cause
    andi k0, k0, 0x7c # 6..2 exc code
    li   k1, 0x30 # 011_0000
    bne  k0, k1, ex_finish
    nop
    mfc0 k0, c0_status
    andi k0, k0, 0x02 # exl
    li k1, 0x02 # exl = 1
    bne k0, k1, ex_finish
    nop
    lui s2, 0x3
    b ex_finish
    nop

adel_load_ex:
    addu s2, zero, zero 
    mfc0 k0, c0_epc
    bne  k0, s4, ex_finish
    nop
    mfc0 k0, c0_cause
    andi k0, k0, 0x7c # 6..2 exc code
    li   k1, 0x10 # 001_0000
    bne  k0, k1, ex_finish
    nop
    mfc0 k0, c0_status
    andi k0, k0, 0x02 # exl
    li k1, 0x02 # exl = 1
    bne k0, k1, ex_finish
    nop
    lui s2, 0x4
    b ex_finish
    nop

ades_ex:
    addu s2, zero, zero 
    mfc0 k0, c0_epc
    bne  k0, s4, ex_finish
    nop
    mfc0 k0, c0_cause
    andi k0, k0, 0x7c # 6..2 exc code
    li   k1, 0x14 # 001_0100
    bne  k0, k1, ex_finish
    nop
    mfc0 k0, c0_status
    andi k0, k0, 0x02 # exl
    li k1, 0x02 # exl = 1
    bne k0, k1, ex_finish
    nop
    lui s2, 0x5
    b ex_finish
    nop

adel_if_ex:
    addu s2, zero, zero 
    mfc0 k0, c0_epc
    bne  k0, s4, ex_finish
    nop
    mtc0 s5, c0_epc
    mfc0 k0, c0_cause
    andi k0, k0, 0x7c # 6..2 exc code
    li   k1, 0x10 # 001_0000
    bne  k0, k1, ex_finish
    nop
    mfc0 k0, c0_status
    andi k0, k0, 0x02 # exl
    li k1, 0x02 # exl = 1
    bne k0, k1, ex_finish
    nop
    lui s2, 0x6
    b ex_finish
    nop

reserved_inst_ex:
    addu s2, zero, zero 
    mfc0 k0, c0_epc
    bne  k0, s4, ex_finish
    nop
    mfc0 k0, c0_cause
    andi k0, k0, 0x7c # 6..2 exc code
    li   k1, 0x28 # 010_1000
    bne  k0, k1, ex_finish
    nop
    mfc0 k0, c0_status
    andi k0, k0, 0x02 # exl
    li k1, 0x02 # exl = 1
    bne k0, k1, ex_finish
    nop
    lui s2, 0x7
    b ex_finish
    nop

int_ex:
    addu s2, zero, zero 
    mfc0 k0, c0_epc
    bne  k0, s4, ex_finish
    nop
    addiu k0, k0, 8
    mtc0  k0, c0_epc   //epc+8
    // disable_trace_cmp
    mfc0 k0, c0_cause  //don't compare cause.bd
    // enable_trace_cmp
    andi k0, k0, 0x7c # 6..2 exc code
    li   k1, 0x00 # 000_0000
    bne  k0, k1, ex_finish
    nop
    mfc0 k0, c0_status
    andi k0, k0, 0x02 # exl
    li k1, 0x02 # exl = 1
    bne k0, k1, ex_finish
    nop
    li   k0, 0xffffffff
    li   k1, 0x00000000
    mtc0 k0, c0_compare //clear compare
    mtc0 k1, c0_cause
    lui s2, 0x8
    b ex_ret
    nop

ex_finish:
    mfc0 k0,c0_cause
    lui k1,0x8000
    and k0,k0,k1
    mfc0 k1,c0_epc
    addiu k1,k1,0x4
    beq k0,zero, 1f
    nop
    addiu k1,k1,0x4
1:  
    mtc0  k1,c0_epc
    nop
    bne s2, zero, ex_ret
    nop
    lui s2, 0xffff
ex_ret:
    .set mips32
    eret
    .set mips1
    nop
	b    inst_error

main:
	.set noreorder

    lui s0, 0          ## initial run number
	li  s3, 0

inst_test:
    jal n1_lui_test    #lui
    nop
    jal n2_addu_test   #addu
    nop
    jal n3_addiu_test  #addiu
    nop
    jal n4_beq_test    #beq
    nop
    jal n5_bne_test    #bne
    nop
    jal n6_lw_test     #lw
    nop
    jal n7_or_test     #or
    nop
    jal n8_slt_test    #slt
    nop
    jal n9_slti_test   #slti
    nop
    jal n10_sltiu_test #sltiu
    nop
    jal n11_sll_test   #sll
    nop
    jal n12_sw_test    #sw
    nop
    jal n13_j_test       #j
    nop
    jal n14_jal_test     #jal
    nop
    jal n15_jr_test      #jr
    nop
    jal n16_beq_ds_test  #beq delay slot
    nop
    jal n17_bne_ds_test  #bne delay slot
    nop
    jal n18_j_ds_test    #j delay slot
    nop
    jal n19_jal_ds_test  #jal delay slot
    nop
    jal n20_jr_ds_test   #jr delay slot
    nop
    jal n21_add_test   #add
    nop
    jal n22_addi_test   #addi
    nop
    jal n23_sub_test   #sub
    nop
    jal n24_subu_test   #subu
    nop
    jal n25_sltu_test   #sltu
    nop
    jal n26_and_test   #and
    nop
    jal n27_andi_test   #andi
    nop
    jal n28_nor_test   #nor
    nop
    jal n29_ori_test   #ori
    nop
    jal n30_xor_test   #xor
    nop
    jal n31_xori_test   #xori 
    nop
    jal n32_sllv_test   #sllv
    nop
    jal n33_sra_test   #sra
    nop
    jal n34_srav_test   #srav
    nop
    jal n35_srl_test   #srl
    nop
    jal n36_srlv_test   #srlv
    nop
    jal n37_bgez_test   #bgez
    nop
    jal n38_bgtz_test   #bgtz
    nop
    jal n39_blez_test   #blez
    nop
    jal n40_bltz_test   #bltz
    nop
    jal n41_bltzal_test   #bltzal
    nop
    jal n42_bgezal_test   #bgezal
    nop
    jal n43_jalr_test   #jalr
    nop
    jal n44_div_test   #div
    nop
    jal n45_divu_test   #divu
    nop
    jal n46_mult_test   #mult
    nop
    jal n47_multu_test   #multu
    nop
    jal n48_mfhi_test   #mfhi
    nop
    jal n49_mflo_test   #mflo
    nop
    jal n50_mthi_test   #mthi
    nop
    jal n51_mtlo_test   #mtlo
    nop
    jal n52_bgez_ds_test   #bgez delay slot
    nop
    jal n53_bgtz_ds_test   #bgtz delay slot
    nop
    jal n54_blez_ds_test   #blez delay slot
    nop
    jal n55_bltz_ds_test   #bltz delay slot
    nop
    jal n56_bltzal_ds_test   #bltzal delay slot
    nop
    jal n57_bgezal_ds_test   #bgezal delay slot
    nop
    jal n58_jalr_ds_test   #jalr delay slot
    nop
    jal n59_lb_test        #lb
    nop
    jal n60_lbu_test       #lbu
    nop
    jal n61_lh_test        #lh
    nop
    jal n62_lhu_test       #lhu
    nop
    jal n63_sb_test        #sb
    nop
    jal n64_sh_test        #sh
    nop
    jal n65_syscall_ex_test       #syscall
    nop
    jal n66_break_ex_test       #break ex
    nop
    jal n67_add_ov_ex_test      #add ov ex
    nop
    jal n68_addi_ov_ex_test     #addi ov ex
    nop
    jal n69_sub_ov_ex_test      #sub ov ex
    nop
    jal n70_lw_adel_ex_test     #lw adel ex
    nop
    jal n71_lh_adel_ex_test     #lh adel ex
    nop
    jal n72_lhu_adel_ex_test    #lhu adel ex
    nop
    jal n73_sw_ades_ex_test     #sw ades ex
    nop
    jal n74_sh_ades_ex_test     #sh ades ex
    nop
    jal n75_ft_adel_ex_test     #ft adel ex
    nop
    jal n76_ri_ex_test          #ri ex
    nop
    jal n77_soft_int_ex_test    #soft int ex
    nop
    jal n78_beq_ds_ex_test      #beq ds ex
    nop
    jal n79_bne_ds_ex_test      #bne ds ex
    nop
    jal n80_bgez_ds_ex_test     #bgez ds ex
    nop
    jal n81_bgtz_ds_ex_test     #bgtz ds ex
    nop
    jal n82_blez_ds_ex_test     #blez ds ex
    nop
    jal n83_bltz_ds_ex_test     #bltz ds ex
    nop
    jal n84_bltzal_ds_ex_test   #bltzal ds ex
    nop
    jal n85_bgezal_ds_ex_test   #bgezal ds ex
    nop
    jal n86_j_ds_ex_test        #j ds ex
    nop
    jal n87_jal_ds_ex_test      #jal ds ex
    nop
    jal n88_jr_ds_ex_test       #jr ds ex
    nop
    jal n89_jalr_ds_ex_test     #jalr ds ex
    nop
    jal n90_branch_prediction   #branch prediction
    nop
    jal n91_ds_cache_miss       #ds_cache_miss
    nop

test_end:
    LI  (s0, TEST_NUM)
    bne s0, s3, inst_error
    nop

	li k0, 0
	lui v0,0x4000
	sb  k0,0(v0)
1:  b 1b
	nop

inst_error:
	li k0, 1
	lui v0,0x4000
	sb  k0,0(v0)
1:  b 1b
	nop
