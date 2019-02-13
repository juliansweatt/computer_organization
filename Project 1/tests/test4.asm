	.text
START:	j	J5
J1:	j	J3
J2:	j	J4
J3:	j	END
J4:	j	J1
J5:	j	J2
END:	addi	$s0,$s0,0
	.data
	.space 0
