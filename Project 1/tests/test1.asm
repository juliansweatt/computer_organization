	.text
Main:	add $t1,$t2,$t3
	addi	$t0,$t1,10
test: add $t1,$t2,$t3
	add $t1,$t2,$t3
	add $t4,$t5,$t6
	add $t7,$s0,$s1
	add $s2,$s3,$s4
	nor $s5,$s6,$s7
	nor $0,$0,$s7
	.data
L1:	.word	1