.align16
putstr:
	;put a string on the screen at coordinates (r1, r2)
	; the string adress is in register r3
	;if r4 is not 1 then refresh between each letter
	copy r6 r3
	__putstr_loop:
		rmem r3 [r6]
		copy r10 r1
		copy r11 r2
		copy r12 r3
		copy r13 r6
		copy r14 r15
		call putchar
		snif r4 eq 0
			refresh
		copy r1 r10
		copy r2 r11
		copy r3 r12
		copy r6 r13
		copy r15 r14
		add r6 r6 1
		add r1 r1 8
		snif r3 eq 0
			jump __putstr_loop
	return
	
	
.align16
putchar:
	;put a char on the screen at coordinates (r1, r2)
	;;get the pointer to the screen
	add r2 r2 1
	lsl r4 r2 7
	lsl r2 r2 5
	add r4 r4 r2
	letl r2 0
	sub r2 r2 r4
	add r4 r2 r1 ; r4 <- screen pointer

	;; load the font
	.set r5 font
	add r3 r3 r3
	add r3 r3 r3
	add r5 r5 r3
	

	;;get the adress to the bottom of the font (1280 = 160 * 8)
	.let r6 1280	
	sub r4 r4 r6

	letl r1 4
	;;loop through the font
	__putchar_loopy:
		rmem r3 [r5]
		letl r2 8 
		add r4 r4 8
		__putchar_loopx:
			and r6 r3 1
			snif r6 neq 1
				wmem r0 [r4]
			sub r4 r4 1
			lsr r3 r3 1
			sub r2 r2 1
			snif r2 eq 0
				jump __putchar_loopx
		.let r2 160
		add r4 r4 r2
		letl r2 8
		add r4 r4 8
		__putchar_loopx2:
			and r6 r3 1
			snif r6 neq 1
				wmem r0 [r4]
			sub r4 r4 1
			lsr r3 r3 1
			sub r2 r2 1
			snif r2 eq 0
				jump __putchar_loopx2
		.let r2 160
		add r4 r4 r2
		add r5 r5 1
		sub r1 r1 1
		snif r1 eq 0
			jump __putchar_loopy
	return

.align16
line:
	; draw a line

	letl r5 0x5f
	leth r5 0xff
	;sub r2 r2 1 <- commented for a fix
	add r4 r4 1 ;<- also in the fix
	
	lsl r6 r2 7
	sub r5 r5 r6
	lsl r6 r2 5
	sub r5 r5 r6
	add r5 r5 r1
	add r2 r2 1
	.push r5

	; all the case are to reduce the problem to one octant
	snif r3 ge r1   ;si x2 >= x1
		jump __line_ifr3ger1
	sub r1 r3 r1
	letl r5 1
	jump __line_ifr3ger1end
	__line_ifr3ger1:
		sub r1 r1 r3
		letl r5 -1
	__line_ifr3ger1end:
	
	snif r4 ge r2   ;si y2 >= y1
		jump __line_ifr4ger2
	sub r2 r4 r2
	letl r6 0x60
	leth r6 0xff
	jump __line_ifr4ger2end
	__line_ifr4ger2:
		sub r2 r2 r4
		letl r6 160
		leth r6 0x00
	__line_ifr4ger2end:


	snif r1 le r2  ;dx >= dy
		jump __line_ifdxgedyend
		;now inverts the dx and invert the i_
		xor r1 r1 r2
		xor r2 r1 r2
		xor r1 r1 r2
		xor r6 r6 r5
		xor r5 r6 r5
		xor r6 r6 r5
	__line_ifdxgedyend:
	xor r3 r3 r3   ;set up e
	sub r3 r3 r1
	add r2 r2 r2	;dy = 2dy
	add r1 r1 r1    ;dx = 2dx
	add r3 r3 r2	;e = 2dy - dx


	copy r8 r2
	.pop r2
	; main loop
	letl r4 -2
	__line_loop:
		wmem r0 [r2]
		snif r3 slt 0    ;e < 0
			jump __line_branch
		jump __line_branch_default
		__line_branch:
			sub r3 r3 r1
			add r2 r2 r6
		__line_branch_default:
			add r3 r3 r8
			add r2 r2 r5
		add r4 r4 2
		snif r4 eq r1
			jump __line_loop

	return


.align16
plotpx:
	;; (0, 0) is in bottom left corner
	add r2 r2 1
	lsl r3 r2 7
	lsl r2 r2 5
	add r3 r3 r2
	letl r2 0
	sub r2 r2 r3
	add r3 r2 r1
	wmem r0 [r3]
	return

.align16
clearscr:
	;; here we don't mind where is (0, 0): is is enough to walkthrough the whole screen
	letl r1 0
	leth r1 -80
	;leth r1 0
	;lsl r1 r1 12
	____loop_clrscr:
		wmem r0 [r1]
		add r1 r1 1
		snif r1 eq 0
			jump ____loop_clrscr
	return

.align16
fill:
	; first we make sur to plot from the bottom left to the bottom right
	snif r1 le r3
		jump __fill_r1>r3
	;r1<=r3
	copy r5 r1
	sub r1 r3 r1
	copy r3 r5
	jump __fill_r1r3end

	__fill_r1>r3:
		sub r1 r1 r3
	__fill_r1r3end:

	snif r2 le r4
		jump __fill_r2>r4
	;r2<=r4
	copy r5 r2
	sub r2 r4 r2
	copy r4 r5
	jump __fill_r2r4end

	__fill_r2>r4:
		sub r2 r2 r4
	__fill_r2r4end:

	
	add r4 r4 1
	lsl r5 r4 7
	lsl r4 r4 5
	add r5 r5 r4
	letl r4 0
	sub r4 r4 r5
	add r5 r4 r3

	.let r4 161
	add r4 r4 r1
	copy r3 r1

	__fill_loopy:
		copy r1 r3
		__fill_loopx:
			wmem r0 [r5]
			add r5 r5 1
			sub r1 r1 1
			snif r1 slt 0
				jump __fill_loopx
		sub r5 r5 r4
		sub r2 r2 1
		snif r2 slt 0
			jump __fill_loopy
	return




#include fonts_data.s
