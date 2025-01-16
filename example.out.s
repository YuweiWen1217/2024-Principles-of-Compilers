	.text
	.globl main
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0"
uniquePaths:
.uniquePaths_0:
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-64
	add			fp,a1,x0
	add			s1,a0,x0
	jal			x0,.uniquePaths_1
.uniquePaths_1:
	addi		a0,sp,0
	li			a1,63
	li			a2,36
	call		memset
	li			t0,0
	add			t0,t0,x0
	jal			x0,.uniquePaths_2
.uniquePaths_2:
	blt			t0,s1,.uniquePaths_3
	jal			x0,.uniquePaths_4
.uniquePaths_3:
	li			t1,3
	mul			t1,t0,t1
	add			t1,t1,fp
	li			t2,1
	sub			t1,t1,t2
	addi		t2,x0,0
	li			t3,1
	mul			t1,t1,t3
	add			t2,t2,t1
	addi		t2,t2,0
	slli		t2,t2,2
	addi		t1,sp,0
	add			t1,t1,t2
	li			t2,1
	sw			t2,0(t1)
	li			t1,1
	add			t0,t0,t1
	add			t0,t0,x0
	jal			x0,.uniquePaths_2
.uniquePaths_4:
	li			t0,0
	addi		t1,x0,0
	li			t2,1
	mul			t0,t0,t2
	add			t1,t1,t0
	addi		t1,t1,0
	slli		t1,t1,2
	addi		t0,sp,0
	add			t0,t0,t1
	li			t1,1
	sw			t1,0(t0)
	li			t0,0
	addi		t1,x0,0
	li			t2,1
	mul			t0,t0,t2
	add			t1,t1,t0
	addi		t1,t1,0
	slli		t1,t1,2
	addi		t0,sp,0
	add			t0,t0,t1
	lw			t0,0(t0)
	addi		a0,t0,0
	addi		sp,sp,64
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	jalr		x0,ra,0
main:
.main_0:
	sd			ra,-8(sp)
	addi		sp,sp,-16
	jal			x0,.main_1
.main_1:
	li			t0,3
	li			t1,3
	addi		a0,t0,0
	addi		a1,t1,0
	call		uniquePaths
	addi		t0,a0,0
	addi		a0,t0,0
	addi		sp,sp,16
	ld			ra,-8(sp)
	jalr		x0,ra,0
	.data
