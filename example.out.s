	.text
	.globl main
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0"
main:
.main_0:
	sd			s3,-40(sp)
	sd			s2,-32(sp)
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-48
	jal			x0,.main_1
.main_1:
	li			fp,5
	li			s1,5
	li			s2,1
	li			t0,2
	li			t1,0
	sub			s3,t1,t0
	li			t0,1
	mul			t0,s3,t0
	li			t1,2
	div			t0,t0,t1
	sub			t1,fp,s1
	add			t0,t0,t1
	li			t1,3
	add			t1,s2,t1
	li			t2,0
	sub			t1,t2,t1
	li			t2,2
	rem			t1,t1,t2
	sub			t0,t0,t1
	addi		a0,t0,0
	call		putint
	li			t0,2
	rem			t0,s3,t0
	li			t1,67
	add			t0,t0,t1
	sub			t1,fp,s1
	li			t2,0
	sub			t1,t2,t1
	add			t0,t0,t1
	li			t1,2
	add			t1,s2,t1
	li			t2,2
	rem			t1,t1,t2
	li			t2,0
	sub			t1,t2,t1
	sub			t0,t0,t1
	li			t1,3
	add			t0,t0,t1
	addi		a0,t0,0
	call		putint
	li			t0,0
	addi		a0,t0,0
	addi		sp,sp,48
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	ld			s2,-32(sp)
	ld			s3,-40(sp)
	jalr		x0,ra,0
	.data
