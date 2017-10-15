    call clearscr
    .let r4 1
    .let r0 0x0000
    .let r1 10
    .let r2 95
    .set r3 HELLO
    call putstr         ; putstr code is in lib.s
    refresh
    .let r1 10
    .let r2 85
    .set r3 COURSE
    call putstr         ; putstr code is in lib.s
    refresh

    jump 0

HELLO: 
    .string "Hello"
COURSE:
    .string "COMPILATION 2017!"

#include lib.s

