
.align16
;; compute a * b on 16 bit
;; input:
;;		r0 <- a
;;		r1 <- b
;; output:
;;		r2 <- result
;; erase r0, r1, r2, r3
;; Work for both signed and unsigned
mul16:
	xor r2 r2 r2
	__mul16_loop:
		and r3 r0 1
		snif r3 neq 1
			add r2 r2 r1
		lsr r0 r0 1
		lsl r1 r1 1
		snif r0 eq 0
			jump __mul16_loop
	
	return
	
.align16
;; Compute the a // b
;; input:
;;		r0 <- a
;;		r1 <- b
;; output:
;;		r2 <- result
;; erase r0, r1, r2, r3, r4
div:
	xor r2 r2 r2
	letl r3 1			;; hold the current pow of two
	snif r0 ge r1		;; if r0 < r1 we must stop
		return
	;; align r1 and r0 together	
	__div_loop1:
		lsl r1 r1 1
		lsl r3 r3 1
		letl r2 0
		leth r2 0x80
		and r2 r3 r2
		snif r2 eq 0
			jump __div_preloop
		snif r1 gt r0
			jump __div_loop1
	lsr r1 r1 1
	lsr r3 r3 1
	;; now divide!
	__div_preloop:
	xor r2 r2 0
	__div_loop2:
		snif r0 ge r1	;; if r0 < r1 we must stop
			jump __div_updt
		sub r0 r0 r1	;; r0 -= r1
		or r2 r2 r3		;; put a one 
		__div_updt:
		lsr r3 r3 1		;; pass to next digit
		lsr r1 r1 1
		snif r3 eq 0		
			jump __div_loop2
	return	
	
.align16
;; Compute the a // b
;; input:
;;		r0 <- a
;;		r1 <- b
;; output:
;;		r2 <- result
;; erase r0, r1, r2, r3, r4
divs:
	xor r4 r4 r4
	snif r0 slt 0
		jump __divs_r1s
	xor r4 r4 1
	xor r0 r0 -1
	add r0 r0 1
	__divs_r1s:
	snif r1 slt 0
		jump __divs_begin
	xor r4 r4 1
	xor r1 r1 -1
	add r1 r1 1
	__divs_begin:

	xor r2 r2 r2
	letl r3 1			;; hold the current pow of two
	snif r0 ge r1		;; if r0 < r1 we must stop
		jump __divs_return
	;; align r1 and r0 together	
	__divs_loop1:
		lsl r1 r1 1
		lsl r3 r3 1
		letl r2 0
		leth r2 0x80
		and r2 r3 r2
		snif r2 eq 0
			jump __divs_preloop
		snif r1 gt r0
			jump __divs_loop1
	lsr r1 r1 1
	lsr r3 r3 1
	;; now divide!
	__divs_preloop:
	xor r2 r2 0
	__divs_loop2:
		snif r0 ge r1	;; if r0 < r1 we must stop
			jump __divs_updt
		sub r0 r0 r1	;; r0 -= r1
		or r2 r2 r3		;; put a one 
		__divs_updt:
		lsr r3 r3 1		;; pass to next digit
		lsr r1 r1 1
		snif r3 eq 0		
			jump __divs_loop2
	__divs_return:
	snif r4 eq 1
		return
	xor r2 r2 -1
	add r2 r2 1
	return	
	
.align16
;; Compute the 32-bit product between two unsigned 16-bits numbers
;; input:
;;		r0 <- a
;;		r1 <- b
;; output
;;		r2 <- low of a * b
;;		r3 <- high of a * b
;; erases register r0, r1, r2, r3, r4, r5, r6
mult32us:
	xor r2 r2 r2
	xor r3 r3 r3
	letl r4 0x10
	copy r5 r1
	__mult32us_loop:			;; while a != 0
		and r6 r0 1
		snif r6 eq 1
			jump __mult32us_noadd
		letl r6 -1				;; to detect it here we use the fact that the presence of a carry 
		sub r6 r6 r2			;;while computing a + b <=> a + b > 0xFFFF <=> a > 0xFFFF - b (with 0xFFFF-b in [0, 0xFFFF])	
		snif r1 le r6			;; if there is a carry 
			add r3 r3 1
		add r2 r2 r1			;; r2 += low of the sum
		lsr r6 r5 r4			
		add r3 r3 r6			;; r3 += part that overflows
		__mult32us_noadd:
		lsr r0 r0 1				;; a >>= 1
		lsl r1 r1 1				;; b <<= 1
		sub r4 r4 1
		snif r0 eq 0
			jump __mult32us_loop
	return

.align16
;; Compute the 32-bit product between two signed 16-bits numbers
;; input:
;;		r0 <- a
;;		r1 <- b
;; output
;;		r2 <- low of a * b
;;		r3 <- high of a * b 
;; erases register r0, r1, r2, r3, r4, r5, r6, r7
mult32s:
	;; firstly, make all the input unsigned
	;; r7 will hold if we must invert the end result
	;; (we can also see that in fact r4 could hold the information of r7:  r4 will always contain a number between 0 and 16 therefore the high semi-word will always be equal to 0 and  r7 is 0 on 1: it could fit on the high semi-word of r4
	;; I prefer the version with inversion rather than a version without it: invertng a value is quick and not so long to write
	xor r4 r4 r4
	snif r0 slt 0
		jump __mult32s_r1s
	xor r4 r4 1
	xor r0 r0 -1
	add r0 r0 1
	__mult32s_r1s:
	snif r1 slt 0
		jump __mult32s_begin
	xor r4 r4 1
	xor r1 r1 -1
	add r1 r1 1
	__mult32s_begin:
	.push r4
	;; normal 32bit product
	xor r2 r2 r2
	xor r3 r3 r3
	letl r4 0x10
	copy r5 r1
	__mult32s_loop:
		and r6 r0 1
		snif r6 eq 1
			jump __mult32s_noadd
		letl r6 -1
		sub r6 r6 r2
		snif r1 le r6
			add r3 r3 1
		add r2 r2 r1
		lsr r6 r5 r4
		add r3 r3 r6
		__mult32s_noadd:
		lsr r0 r0 1
		lsl r1 r1 1
		sub r4 r4 1
		snif r0 eq 0
			jump __mult32s_loop

	;; if we must invert, then invert, otherwise stop
	.pop r4
	snif r4 eq 1
		return
	xor r2 r2 -1
	xor r3 r3 -1
	snif r2 neq -1			;; do not forget the carry 
		add r3 r3 1
	add r2 r2 1
	return


.align16
;; Compute the 32-bit product between two signed 16-bits float numbers
;; input:
;;		r0 <- a
;;		r1 <- b
;; output
;;		r0 <- a * b
;; erases register r0, r1, r2, r3, r4, r5, r6, r7
multfloat:
	;; firstly, make all the input unsigned
	;; r7 will hold if we must invert the end result
	;; (we can also see that in fact r4 could hold the information of r7:  r4 will always contain a number between 0 and 16 therefore the high semi-word will always be equal to 0 and  r7 is 0 on 1: it could fit on the high semi-word of r4
	;; I prefer the version with inversion rather than a version without it: invertng a value is quick and not so long to write
	xor r4 r4 r4
	snif r0 slt 0
		jump __multfloat_r1s
	xor r4 r4 1
	xor r0 r0 -1
	add r0 r0 1
	__multfloat_r1s:
	snif r1 slt 0
		jump __multfloat_begin
	xor r4 r4 1
	xor r1 r1 -1
	add r1 r1 1
	__multfloat_begin:
	.push r4
	;; normal 32bit product
	xor r2 r2 r2
	xor r3 r3 r3
	letl r4 0x10
	copy r5 r1
	__multfloat_loop:
		and r6 r0 1
		snif r6 eq 1
			jump __multfloat_noadd
		letl r6 -1
		sub r6 r6 r2
		snif r1 le r6
			add r3 r3 1
		add r2 r2 r1
		lsr r6 r5 r4
		add r3 r3 r6
		__multfloat_noadd:
		lsr r0 r0 1
		lsl r1 r1 1
		sub r4 r4 1
		snif r0 eq 0
			jump __multfloat_loop

	;; if we must invert, then invert, otherwise stop
	.pop r4
	snif r4 eq 1
		jump __multfloat_end
	xor r2 r2 -1
	xor r3 r3 -1
	snif r2 neq -1			;; do not forget the carry 
		add r3 r3 1
	add r2 r2 1

	__multfloat_end:
	lsr r2 r2 8 ;;r0 = 0x__highr2
	lsl r3 r3 8 ;;r3 = 0x__lowr3__
	or r0 r2 r3 ;;r0 = 0xlowr3highr2
	return


