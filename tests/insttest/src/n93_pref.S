#include <asm.h>
#include <regdef.h>
#include <inst_test.h>


LEAF(n93_pref)
    .set noreorder
# should not raise exceptions
start:
    pref    0,0(zero)
    pref    1,0(zero)
    pref    2,0(zero)
    pref    3,0(zero)
    pref    4,0(zero)
    pref    5,0(zero)
    pref    6,0(zero)
    pref    7,0(zero)
    pref    8,0(zero)
    pref    9,0(zero)
    pref    10,0(zero)
    pref    11,0(zero)
    pref    12,0(zero)
    pref    13,0(zero)
    pref    14,0(zero)
    pref    15,0(zero)
    pref    16,0(zero)
    pref    17,0(zero)
    pref    18,0(zero)
    pref    19,0(zero)
    pref    20,0(zero)
    pref    21,0(zero)
    pref    22,0(zero)
    pref    23,0(zero)
    pref    24,0(zero)
    pref    25,0(zero)
    pref    26,0(zero)
    pref    27,0(zero)
    pref    28,0(zero)
    pref    29,0(zero)
    pref    30,0(zero)
    pref    31,0(zero)
    la a0, start
    pref    0,0(a0)
    pref    0,1(a0)
    pref    0,2(a0)
    pref    0,3(a0)
###score ++
    addiu s3, s3, 1
### return
    jr ra
    nop
END(n93_pref)
