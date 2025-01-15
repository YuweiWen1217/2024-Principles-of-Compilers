	.text
	.globl main
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0"
main:
.main_0:
	jal			x0,.main_1
.main_1:
	addi		a0,sp,0
	call		llvm.memset.p0.i32
	addi		%0,x0,0
	addi		%0,%0,0
	slli		%0,%0,2
	addi		%1,sp,0
	add			%2,%1,%0
	li			%3,1
	sw			%3,0(%2)
	li			%4,0
	addi		%5,x0,0
	li			%7,1
	mul			%6,%4,%7
	add			%5,%5,%6
	addi		%5,%5,0
	slli		%5,%5,2
	lui			%9,%hi(b)
	addi		%8,%9,%lo(b)
	add			%10,%8,%5
	lw			%11,0(%10)
	li			%12,1
	li			%13,1
	addi		%14,x0,0
	li			%16,10
	mul			%15,%12,%16
	add			%14,%14,%15
	li			%18,1
	mul			%17,%13,%18
	add			%14,%14,%17
	addi		%14,%14,0
	slli		%14,%14,2
	addi		%19,sp,0
	add			%20,%19,%14
	li			%21,2
	sw			%21,0(%20)
	li			%22,2
	li			%23,2
	addi		%24,x0,0
	li			%26,10
	mul			%25,%22,%26
	add			%24,%24,%25
	li			%28,1
	mul			%27,%23,%28
	add			%24,%24,%27
	addi		%24,%24,0
	slli		%24,%24,2
	addi		%29,sp,0
	add			%30,%29,%24
	li			%31,1
	addi		%32,x0,0
	li			%34,1
	mul			%33,%31,%34
	add			%32,%32,%33
	addi		%32,%32,0
	slli		%32,%32,2
	lui			%36,%hi(b)
	addi		%35,%36,%lo(b)
	add			%37,%35,%32
	lw			%38,0(%37)
	sw			%38,0(%30)
	li			%39,4
	lui			%40,%hi(d)
	sw			%39,%lo(d)(%40)
	lui			%41,%hi(d)
	lw			%42,%lo(d)(%41)
	add			%43,%11,%42
	addi		a0,%43,0
	jalr		x0,ra,0
	.data
b:
	.word	1
	.word	2
d:
	.word	1
