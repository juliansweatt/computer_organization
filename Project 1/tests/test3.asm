	.text
MAIN:	addi	$s0,$0,50
	addi	$s1,$s0,-5
	ori	$t0,$s1,1
	ori	$t2,$s1,0
	ori	$t1,$t0,-1
	ori	$s7,$t1,64
	lui	$s0,100
	lui	$s1,200
	lui	$s2,300
	lui	$s3,400
	lui	$s4,500
	lui	$s5,600
	lui	$s6,700
	lui	$s7,800
	lui	$t0,0
	lui	$t0,-1000
	lui	$t1,-2000
	lui	$t2,-3000
	lui	$t3,-4000
	lui	$t4,-5000
	lui	$t5,-6000
	lui	$t6,-7000
	lui	$t7,-8000
	la	$t7,testInt
	sw	$t6,0($t7)
	addi	$t6,$0,1
	sw	$t6,10($t7)
	la	$s7,ARRL
	sw	$t6,-10($s7)
	la	$t5,testBool
	addi	$t6,$0,1
	sw	$t6,0($t5)
	lw	$t3,0($t5)
	lw	$t3,-5($t5)
	lw	$t3,5($t5)
	la	$s0,testInt
	lw	$s6,0($s0)
	addi	$s0,$0,50
	addi	$s1,$0,55
LOOP:	addi	$s0,$s0,1
	bne	$s0,$s1,LOOP
	bne	$s0,$s1,END
	addi	$s0,$0,8
	addi	$s1,$0,9
LOOP2:	addi	$s0,$s0,1
	beq	$s0,$s1,LOOP2
	beq	$s0,$s0,END
END:	addi	$t0,$0,0
	.data
testInt:	.word	1
ARRL:	.word	50
ARRS:	.word	10
testBool:	.space	1
