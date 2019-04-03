	.text
	ori	$s0,$0,Data1
	sw	$t5,32($s0)
	ori	$t1,$0,9
	ori	$t5,$t1,50
	sw	$t5,0($s0)
	sll	$t2,$t5,3
	andi	$t3,$t2,255
	add	$t4,$t3,$0
	ori	$t0,$t4,10
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