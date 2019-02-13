	.text
MAIN:	sll	$t0,$t1,10
	la	$t2,bookmark
	la	$s0,A3
	la	$t5,A1
	la	$s2,A2
	la	$t1,MAIN
bookmark:	sll	$t5,$s0,0
	add	$t1,$t2,$0
	add	$t3,$s0,$1
	addi	$s2,$t3,21
	addi	$s3,$0,2147483647
	addi	$s3,$0,0
	addi	$s3,$0,-2147483648
	addi	$t0,$t3,-500
	addi	$t0,$t3,-25600
	nor	$t2,$t3,$s0
	nor	$s0,$0,$s0
	ori	$s5,$t3,100
	ori	$s5,$t3,2147483647
	ori	$s5,$t3,0
	ori	$s5,$t3,-100
	ori	$s3,$0,-2147483648
	lui	$t7,2147483647
	lui	$s7,137
	lui	$s6,4
	lui	$s0,0
	lui	$s2,-10
	lui	$0,-2147483648
	sw	$s3,2147483647($s5)
	sw	$s7,5($t2)
	sw	$t3,0($t0)
	sw	$t6,-51($t4)
	sw	$s6,-2147483648($0)
	lw	$s1,2147483647($t1)
	lw	$s2,2468($t2)
	lw	$s3,4($t3)
	lw	$s4,0($0)
	lw	$s5,-5($t5)
	lw	$s6,-872($t6)
	lw	$s7,-2147483648($t0)
	bne	$s1,$s1,bookmark
	bne	$t1,$s1,MAIN
	bne	$0,$0,A1
	bne	$s7,$t7,END
jumpcmd:	j	MAIN
jloop:	j	jloop
	j	jumpcmd
	j	END
	j	A1
	j	A2
	j	A3
	j	bookmark
END:	add	$t0,$0,$0
	.data
A1:	.word	1
A2:	.space	500
A3:	.word	1
