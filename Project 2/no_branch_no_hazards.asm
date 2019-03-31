	.text
	ori	$s0,$0,Data1
	noop
	noop
	lw	$t0,0($s0)
	noop
	noop
	lw	$t1,4($s0)
	noop
	noop
	add	$t3,$t0,$t1
	noop
	noop
	sub	$t4,$t0,$t1
	noop
	noop
	sll	$t2,$t0,8
	noop
	noop
	andi	$t5,$t2,1
	noop
	noop
	or	$t5,$0,1
	noop
	noop
	sw	$t2,8($s0)
	halt
	.data
Data1:	.word	15
Data2:	.word	25