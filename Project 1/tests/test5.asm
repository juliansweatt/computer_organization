	.text
MAIN:	la	$s0,TEST_INT
	addi	$t0,$0,0
	addi	$s1,$0,1
INCREMENT:	add	$t0,$t0,$s1
	bne	$t0,$s1,INCREMENT
	sw	$t0,0($s0)
	sll	$t0,$t0,4
	lui	$t1,256
	nor	$t0,$t0,$t1
	ori	$t0,$t0,1
	lw	$t2,0($s0)
	bne	$t0,$t2,BREAK
	j	MAIN
BREAK:	j	END
END:	la	$s1,ARR
	sw	$t0,0($s1)
	.data
TEST_INT:	.word	0
TEST_INT2:	.word	0
ARR:	.space	8
