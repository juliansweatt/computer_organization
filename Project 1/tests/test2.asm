	.text
	sub	$s1,$0,$1
	sll	$s1,$s1,10
	nor	$s0,$s1,$0
	add	$s0,$t1,$1
	sll	$s0,$s0,31
	add	$s2,$t3,$s0
	sub	$t3,$s2,$s0
	sll	$t7,$t3,0
	nor	$s1,$t3,$s1
	sll	$s1,$s1,5
	.data
	.word	0
	