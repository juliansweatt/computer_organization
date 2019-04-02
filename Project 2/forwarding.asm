	.text
	ori	$s0,$0,Data1
	ori	$t0,$0,10
	ori $t1,$t0,32
	sll	$t2,$t1,3
	andi	$t5,$t2,255
	ori	$t3,$t5,1
	sw	$t3,12($s0)
	ori	$s3,$0,Data2
	halt
	.data
Data1:	.word	15
Data2:	.word	25