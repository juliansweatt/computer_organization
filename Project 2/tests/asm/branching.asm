	.text
	ori	$s0,$0,Data1
	ori	$s1,$0,Data2
	ori	$s2,$0,1
	sll	$s2,$s2,1
	bne	$s0,$s2,-4
	bne	$s0,$s0,8
	bne	$s0,$s1,4
	add	$t4,$s0,$s1
	halt
	.data
Data1:	.word	4
Data2:	.word	25