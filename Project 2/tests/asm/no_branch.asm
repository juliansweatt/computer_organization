	.text
	ori	$s0,$0,Data2
	ori	$s1,$0,4
	sub	$t1,$s0,$s1
	sll	$s1,$s1,1
	add	$t2,$s0,$s1
	lw	$t5,-8($t2)
	andi	$t5,$s1,255
	sw	$t5,8($s0)
	ori	$t4,$0,32
	sll	$t3,$t4,1
	andi	$t5,$t4,64
	add	$t1,$t3,$t5
	sw	$t5,16($s0)
	halt
	.data
Data1:	.word	77
Data2:	.word	45
Data3:	.word	6