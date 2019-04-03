	.text
	ori	$s0,$0,Data1
	ori	$t0,$0,10
	ori $t1,$s0,32
	sll	$t2,$t0,3
	andi	$t5,$t1,255
	ori	$t3,$t2,1
	add	$t2,$t3,$t5
	sub	$t4,$t3,$t2
	sw	$t2,8($s0)
	halt
	.data
Data1:	.word	15
Data2:	.word	25