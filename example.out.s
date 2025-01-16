	.text
	.globl main
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0"
main:
.main_0:
	addi		sp,sp,-16
	jal			x0,.main_1
.main_1:
	li			t0,1
	li			t1,8
	slt			t0,t0,t1
	li			t1,1
	bne			t0,t1,.main_2
	jal			x0,.main_3
.main_2:
	li			t0,0
	addi		a0,t0,0
	addi		sp,sp,16
	jalr		x0,ra,0
.main_3:
	li			t0,1
	addi		a0,t0,0
	addi		sp,sp,16
	jalr		x0,ra,0
	.data
