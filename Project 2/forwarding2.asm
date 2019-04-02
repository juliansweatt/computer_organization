	.text
	ori	$s0,$0,Data1
	ori	$t1,$0,9
	ori	$t5,$0,50
	sw	$t5,32($s0)
	sw	$0,0($s0)
	sll	$t2,$s1,3
	andi	$t3,$t2,255
	add	$t4,$t3,$0
	halt
	.data
Data1:	.word	15
Data2:	.word	25