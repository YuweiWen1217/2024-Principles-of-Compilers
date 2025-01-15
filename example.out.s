	.text
	.globl main
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0"
main:
.main_0:
	jal			x0,.main_1
.main_1:
	li			%0,1
	add			%1,%0,x0
	jal			x0,.main_2
.main_2:
	li			%3,0
	bgt			%1,%3,.main_3
	jal			x0,.main_4
.main_3:
	li			%4,1
	sub			%2,%1,%4
	add			%1,%2,x0
	jal			x0,.main_2
.main_4:
	addi		a0,%1,0
	jalr		x0,ra,0
	.data
