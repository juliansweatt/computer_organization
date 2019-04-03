	.text
	ori	$s0,$0,Data1
	lw	$t0,0($s0)
	lw	$t1,4($s0)
	add	$t3,$t0,$t1
	sub	$t4,$t0,$t1
	sll	$t2,$t0,8
	andi	$t5,$t2,1
	ori	$t5,$0,1
	sw	$t2,8($s0)
	halt
	.data
Data1:	.word	15
Data2:	.word	25