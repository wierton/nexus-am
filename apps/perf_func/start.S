#/*	$Id: start.S,v 1.1.1.1 2006/09/14 01:59:08 root Exp $ */

/*
 * Copyright (c) 2001 Opsycon AB  (www.opsycon.se)
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Opsycon AB, Sweden.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef _KERNEL
#define _KERNEL
#endif

#include <asm/asm.h>
#include <asm/regdef.h>
#include <cpu.h>
#include <machine.h>
#include <ns16550.h>
#include <asm/context.h>

	.set	noreorder
	.globl	_start
	.globl	start
	.globl	__main
_start:
start:
    # disable_trace_cmp_s
    # disable_num_monitor_s

    lui t2,0x0040    
	mtc0	t2, c0_status
	mtc0	zero, c0_cause
	la	sp, _stack
	la	gp, _gp
    
    la t9, run_test    #####          
    jr t9              ###kseg1 -> kseg0            
    nop                ##### 


.org 0x100
test_finish:
    li t0, 0x0
    li t1, GPIO_TRAP
    sb t0, 0x0(t1)
halt:
    b halt
    nop

/*
 *  exception handle
 */
.org 0x380
1:  
    addiu t0, t0, 1
    b 1b
    nop

run_test:
#if CMP_FUNC==1
	bal	shell1
	nop
#elif CMP_FUNC==2
	bal	shell2
	nop
#elif CMP_FUNC==3
	bal	shell3
	nop
#elif CMP_FUNC==4
	bal	shell4
	nop
#elif CMP_FUNC==5
	bal	shell5
	nop
#elif CMP_FUNC==6
	bal	shell6
	nop
#elif CMP_FUNC==7
	bal	shell7
	nop
#elif CMP_FUNC==8
	bal	shell8
	nop
#elif CMP_FUNC==9
	bal	shell9
	nop
#elif CMP_FUNC==10
	bal	shell10
	nop
#else
    bal shell1
    nop

    bal shell2
    nop

    bal shell3
    nop

    bal shell4
    nop

    bal shell5
    nop

    bal shell6
    nop

    bal shell7
    nop

    bal shell8
    nop

    bal shell9
    nop

    bal shell10
    nop
#endif
go_finish:
    la t1, test_finish
	jr t1
    nop
