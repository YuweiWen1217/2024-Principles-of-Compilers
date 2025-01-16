	.text
	.globl main
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0"
main:
.main_0:
	addi		sp,sp,-16
	jal			x0,.main_1
.main_1:
	li			t0,1065353216
	fmv.w.x		ft0,t0
	fcvt.w.s		t0,ft0,rtz
	addi		a0,t0,0
	addi		sp,sp,16
	jalr		x0,ra,0
	.data
