	.text
	.globl main
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0"
rand:
.rand_0:
	addi		sp,sp,-16
	jal			x0,.rand_1
.rand_1:
	lui			t0,%hi(seed)
	lw			t0,%lo(seed)(t0)
	li			t1,19980130
	mul			t0,t0,t1
	li			t1,23333
	add			t0,t0,t1
	li			t1,100000007
	rem			t0,t0,t1
	lui			t1,%hi(seed)
	sw			t0,%lo(seed)(t1)
	lui			t0,%hi(seed)
	lw			t0,%lo(seed)(t0)
	li			t1,0
	blt			t0,t1,.rand_2
	jal			x0,.rand_3
.rand_2:
	lui			t0,%hi(seed)
	lw			t0,%lo(seed)(t0)
	li			t1,100000007
	add			t0,t0,t1
	lui			t1,%hi(seed)
	sw			t0,%lo(seed)(t1)
	jal			x0,.rand_3
.rand_3:
	lui			t0,%hi(seed)
	lw			t0,%lo(seed)(t0)
	addi		a0,t0,0
	addi		sp,sp,16
	jalr		x0,ra,0
p:
.p_0:
	addi		sp,sp,-16
	fmv.s		ft0,fa0
	jal			x0,.p_1
.p_1:
	li			t0,1077936128
	fmv.w.x		ft1,t0
	fmul.s		ft1,ft1,ft0
	li			t0,1082130432
	fmv.w.x		ft2,t0
	fmul.s		ft2,ft2,ft0
	fmul.s		ft2,ft2,ft0
	fmul.s		ft0,ft2,ft0
	fsub.s		ft0,ft1,ft0
	fmv.s		fa0,ft0
	addi		sp,sp,16
	jalr		x0,ra,0
my_fabs:
.my_fabs_0:
	addi		sp,sp,-16
	fmv.s		ft0,fa0
	jal			x0,.my_fabs_1
.my_fabs_1:
	li			t0,0
	fmv.w.x		ft1,t0
	fle.s		t0,ft0,ft1
	beq			t0,x0,.my_fabs_2
	jal			x0,.my_fabs_3
.my_fabs_2:
	fmv.s		fa0,ft0
	addi		sp,sp,16
	jalr		x0,ra,0
.my_fabs_3:
	li			t0,0
	fmv.w.x		ft1,t0
	fsub.s		ft0,ft1,ft0
	fmv.s		fa0,ft0
	addi		sp,sp,16
	jalr		x0,ra,0
my_sqrt:
.my_sqrt_0:
	addi		sp,sp,-16
	fmv.s		ft0,fa0
	jal			x0,.my_sqrt_1
.my_sqrt_1:
	li			t0,8
	fcvt.s.w		ft1,t0
	fdiv.s		ft1,ft0,ft1
	li			t0,1056964608
	fmv.w.x		ft2,t0
	fadd.s		ft1,ft1,ft2
	li			t0,1073741824
	fmv.w.x		ft2,t0
	fmul.s		ft2,ft2,ft0
	li			t0,1082130432
	fmv.w.x		ft3,t0
	fadd.s		ft3,ft3,ft0
	fdiv.s		ft2,ft2,ft3
	fadd.s		ft1,ft1,ft2
	li			t0,10
	add			t0,t0,x0
	fmv.s		ft1,ft1
	jal			x0,.my_sqrt_2
.my_sqrt_2:
	li			t1,0
	bne			t0,t1,.my_sqrt_3
	jal			x0,.my_sqrt_4
.my_sqrt_3:
	fdiv.s		ft2,ft0,ft1
	fadd.s		ft1,ft1,ft2
	li			t1,2
	fcvt.s.w		ft2,t1
	fdiv.s		ft1,ft1,ft2
	li			t1,1
	sub			t0,t0,t1
	add			t0,t0,x0
	fmv.s		ft1,ft1
	jal			x0,.my_sqrt_2
.my_sqrt_4:
	fmv.s		fa0,ft1
	addi		sp,sp,16
	jalr		x0,ra,0
main:
.main_0:
	sd			ra,-8(sp)
	addi		sp,sp,-16
	jal			x0,.main_1
.main_1:
	call		write_pgm
	li			t0,0
	addi		a0,t0,0
	addi		sp,sp,16
	ld			ra,-8(sp)
	jalr		x0,ra,0
my_sin_impl:
.my_sin_impl_0:
	fsd			fs0,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-16
	fmv.s		fs0,fa0
	jal			x0,.my_sin_impl_1
.my_sin_impl_1:
	fmv.s		fa0,fs0
	call		my_fabs
	fmv.s		ft0,fa0
	li			t0,897988541
	fmv.w.x		ft1,t0
	fle.s		t0,ft0,ft1
	bne			t0,x0,.my_sin_impl_2
	jal			x0,.my_sin_impl_3
.my_sin_impl_2:
	fmv.s		fa0,fs0
	addi		sp,sp,16
	ld			ra,-8(sp)
	fld			fs0,-16(sp)
	jalr		x0,ra,0
.my_sin_impl_3:
	li			t0,1077936128
	fmv.w.x		ft0,t0
	fdiv.s		ft0,fs0,ft0
	fmv.s		fa0,ft0
	call		my_sin_impl
	fmv.s		ft0,fa0
	fmv.s		fa0,ft0
	call		p
	fmv.s		ft0,fa0
	fmv.s		fa0,ft0
	addi		sp,sp,16
	ld			ra,-8(sp)
	fld			fs0,-16(sp)
	jalr		x0,ra,0
my_sin:
.my_sin_0:
	sd			ra,-8(sp)
	addi		sp,sp,-16
	fmv.s		ft0,fa0
	jal			x0,.my_sin_1
.my_sin_1:
	li			t0,1086918619
	fmv.w.x		ft1,t0
	fle.s		t0,ft0,ft1
	beq			t0,x0,.my_sin_2
	jal			x0,.my_sin_4
.my_sin_2:
	lui			t0,%hi(TWO_PI)
	flw			ft1,%lo(TWO_PI)(t0)
	fdiv.s		ft1,ft0,ft1
	fcvt.w.s		t0,ft1,rtz
	fcvt.s.w		ft1,t0
	lui			t0,%hi(TWO_PI)
	flw			ft2,%lo(TWO_PI)(t0)
	fmul.s		ft1,ft1,ft2
	fsub.s		ft0,ft0,ft1
	fmv.s		ft1,ft0
	jal			x0,.my_sin_3
.my_sin_3:
	li			t0,1078530011
	fmv.w.x		ft0,t0
	fle.s		t0,ft1,ft0
	beq			t0,x0,.my_sin_5
	fmv.s		ft0,ft1
	jal			x0,.my_sin_6
.my_sin_4:
	li			t0,-1060565029
	fmv.w.x		ft1,t0
	flt.s		t0,ft0,ft1
	bne			t0,x0,.my_sin_2
	fmv.s		ft1,ft0
	jal			x0,.my_sin_3
.my_sin_5:
	lui			t0,%hi(TWO_PI)
	flw			ft0,%lo(TWO_PI)(t0)
	fsub.s		ft0,ft1,ft0
	fmv.s		ft0,ft0
	jal			x0,.my_sin_6
.my_sin_6:
	li			t0,-1068953637
	fmv.w.x		ft1,t0
	flt.s		t0,ft0,ft1
	bne			t0,x0,.my_sin_7
	fmv.s		ft1,ft0
	jal			x0,.my_sin_8
.my_sin_7:
	lui			t0,%hi(TWO_PI)
	flw			ft1,%lo(TWO_PI)(t0)
	fadd.s		ft0,ft0,ft1
	fmv.s		ft1,ft0
	jal			x0,.my_sin_8
.my_sin_8:
	fmv.s		fa0,ft1
	call		my_sin_impl
	fmv.s		ft0,fa0
	fmv.s		fa0,ft0
	addi		sp,sp,16
	ld			ra,-8(sp)
	jalr		x0,ra,0
my_cos:
.my_cos_0:
	sd			ra,-8(sp)
	addi		sp,sp,-16
	fmv.s		ft0,fa0
	jal			x0,.my_cos_1
.my_cos_1:
	li			t0,1070141403
	fmv.w.x		ft1,t0
	fadd.s		ft0,ft0,ft1
	fmv.s		fa0,ft0
	call		my_sin
	fmv.s		ft0,fa0
	fmv.s		fa0,ft0
	addi		sp,sp,16
	ld			ra,-8(sp)
	jalr		x0,ra,0
circle_sdf:
.circle_sdf_0:
	fsd			fs0,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-16
	fmv.s		fs0,fa4
	fmv.s		ft0,fa3
	fmv.s		ft1,fa2
	fmv.s		ft2,fa1
	fmv.s		ft3,fa0
	jal			x0,.circle_sdf_1
.circle_sdf_1:
	fsub.s		ft1,ft3,ft1
	fsub.s		ft0,ft2,ft0
	fmul.s		ft1,ft1,ft1
	fmul.s		ft0,ft0,ft0
	fadd.s		ft0,ft1,ft0
	fmv.s		fa0,ft0
	call		my_sqrt
	fmv.s		ft0,fa0
	fsub.s		ft0,ft0,fs0
	fmv.s		fa0,ft0
	addi		sp,sp,16
	ld			ra,-8(sp)
	fld			fs0,-16(sp)
	jalr		x0,ra,0
scene:
.scene_0:
	fsd			fs2,-40(sp)
	fsd			fs1,-32(sp)
	fsd			fs0,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-48
	add			fp,a0,x0
	fmv.s		fs0,fa1
	fmv.s		fs1,fa0
	jal			x0,.scene_1
.scene_1:
	li			t0,1053609165
	fmv.w.x		ft0,t0
	li			t0,1053609165
	fmv.w.x		ft1,t0
	li			t0,1036831949
	fmv.w.x		ft2,t0
	fmv.s		fa0,fs1
	fmv.s		fa1,fs0
	fmv.s		fa2,ft0
	fmv.s		fa3,ft1
	fmv.s		fa4,ft2
	call		circle_sdf
	fmv.s		fs2,fa0
	li			t0,1058642330
	fmv.w.x		ft0,t0
	li			t0,1058642330
	fmv.w.x		ft1,t0
	li			t0,1028443341
	fmv.w.x		ft2,t0
	fmv.s		fa0,fs1
	fmv.s		fa1,fs0
	fmv.s		fa2,ft0
	fmv.s		fa3,ft1
	fmv.s		fa4,ft2
	call		circle_sdf
	fmv.s		ft0,fa0
	flt.s		t0,fs2,ft0
	bne			t0,x0,.scene_2
	jal			x0,.scene_3
.scene_2:
	li			t0,0
	addi		t1,x0,0
	li			t2,1
	mul			t0,t0,t2
	add			t1,t1,t0
	addi		t1,t1,0
	slli		t1,t1,2
	add			t0,fp,t1
	fsw			fs2,0(t0)
	li			t0,1
	addi		t1,x0,0
	li			t2,1
	mul			t0,t0,t2
	add			t1,t1,t0
	addi		t1,t1,0
	slli		t1,t1,2
	add			t0,fp,t1
	li			t1,1077936128
	fmv.w.x		ft0,t1
	fsw			ft0,0(t0)
	jal			x0,.scene_4
.scene_3:
	li			t0,0
	addi		t1,x0,0
	li			t2,1
	mul			t0,t0,t2
	add			t1,t1,t0
	addi		t1,t1,0
	slli		t1,t1,2
	add			t0,fp,t1
	fsw			ft0,0(t0)
	li			t0,1
	addi		t1,x0,0
	li			t2,1
	mul			t0,t0,t2
	add			t1,t1,t0
	addi		t1,t1,0
	slli		t1,t1,2
	add			t0,fp,t1
	li			t1,0
	fmv.w.x		ft0,t1
	fsw			ft0,0(t0)
	jal			x0,.scene_4
.scene_4:
	addi		sp,sp,48
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	fld			fs0,-24(sp)
	fld			fs1,-32(sp)
	fld			fs2,-40(sp)
	jalr		x0,ra,0
trace:
.trace_0:
	fsd			fs4,-56(sp)
	fsd			fs3,-48(sp)
	fsd			fs2,-40(sp)
	fsd			fs1,-32(sp)
	fsd			fs0,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-64
	fmv.s		fs0,fa3
	fmv.s		fs1,fa2
	fmv.s		fs2,fa1
	fmv.s		fs3,fa0
	jal			x0,.trace_1
.trace_1:
	li			t0,0
	fmv.w.x		ft0,t0
	li			t0,0
	add			fp,t0,x0
	fmv.s		fs4,ft0
	jal			x0,.trace_2
.trace_2:
	li			t0,10
	blt			fp,t0,.trace_5
	jal			x0,.trace_4
.trace_3:
	addi		a0,sp,0
	li			a1,63
	li			a2,8
	call		memset
	fmul.s		ft0,fs1,fs4
	fadd.s		ft0,fs3,ft0
	fmul.s		ft1,fs0,fs4
	fadd.s		ft1,fs2,ft1
	addi		t0,x0,0
	addi		t0,t0,0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	fmv.s		fa0,ft0
	fmv.s		fa1,ft1
	addi		a0,t0,0
	call		scene
	li			t0,0
	addi		t1,x0,0
	li			t2,1
	mul			t0,t0,t2
	add			t1,t1,t0
	addi		t1,t1,0
	slli		t1,t1,2
	addi		t0,sp,0
	add			t0,t0,t1
	flw			ft0,0(t0)
	li			t0,897988541
	fmv.w.x		ft1,t0
	flt.s		t0,ft0,ft1
	bne			t0,x0,.trace_6
	jal			x0,.trace_7
.trace_4:
	li			t0,0
	fmv.w.x		ft0,t0
	fmv.s		fa0,ft0
	addi		sp,sp,64
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	fld			fs0,-24(sp)
	fld			fs1,-32(sp)
	fld			fs2,-40(sp)
	fld			fs3,-48(sp)
	fld			fs4,-56(sp)
	jalr		x0,ra,0
.trace_5:
	li			t0,1073741824
	fmv.w.x		ft0,t0
	flt.s		t0,fs4,ft0
	bne			t0,x0,.trace_3
	jal			x0,.trace_4
.trace_6:
	li			t0,1
	addi		t1,x0,0
	li			t2,1
	mul			t0,t0,t2
	add			t1,t1,t0
	addi		t1,t1,0
	slli		t1,t1,2
	addi		t0,sp,0
	add			t0,t0,t1
	flw			ft0,0(t0)
	fmv.s		fa0,ft0
	addi		sp,sp,64
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	fld			fs0,-24(sp)
	fld			fs1,-32(sp)
	fld			fs2,-40(sp)
	fld			fs3,-48(sp)
	fld			fs4,-56(sp)
	jalr		x0,ra,0
.trace_7:
	li			t0,0
	addi		t1,x0,0
	li			t2,1
	mul			t0,t0,t2
	add			t1,t1,t0
	addi		t1,t1,0
	slli		t1,t1,2
	addi		t0,sp,0
	add			t0,t0,t1
	flw			ft0,0(t0)
	fadd.s		ft0,fs4,ft0
	li			t0,1
	add			t0,fp,t0
	add			fp,t0,x0
	fmv.s		fs4,ft0
	jal			x0,.trace_2
sample:
.sample_0:
	fsd			fs4,-56(sp)
	fsd			fs3,-48(sp)
	fsd			fs2,-40(sp)
	fsd			fs1,-32(sp)
	fsd			fs0,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-64
	fmv.s		fs0,fa1
	fmv.s		fs1,fa0
	jal			x0,.sample_1
.sample_1:
	li			t0,0
	fmv.w.x		ft0,t0
	li			t0,0
	add			fp,t0,x0
	fmv.s		fs2,ft0
	jal			x0,.sample_2
.sample_2:
	li			t0,24
	blt			fp,t0,.sample_3
	jal			x0,.sample_4
.sample_3:
	call		rand
	addi		t0,a0,0
	fcvt.s.w		ft0,t0
	fcvt.s.w		ft1,fp
	lui			t0,%hi(RAND_MAX)
	lw			t0,%lo(RAND_MAX)(t0)
	fcvt.s.w		ft2,t0
	fdiv.s		ft0,ft0,ft2
	fadd.s		ft0,ft1,ft0
	li			t0,1086918619
	fmv.w.x		ft1,t0
	fmul.s		ft0,ft1,ft0
	lui			t0,%hi(N)
	lw			t0,%lo(N)(t0)
	fcvt.s.w		ft1,t0
	fdiv.s		fs3,ft0,ft1
	fmv.s		fa0,fs3
	call		my_cos
	fmv.s		fs4,fa0
	fmv.s		fa0,fs3
	call		my_sin
	fmv.s		ft0,fa0
	fmv.s		fa0,fs1
	fmv.s		fa1,fs0
	fmv.s		fa2,fs4
	fmv.s		fa3,ft0
	call		trace
	fmv.s		ft0,fa0
	fadd.s		ft0,fs2,ft0
	li			t0,1
	add			t0,fp,t0
	add			fp,t0,x0
	fmv.s		fs2,ft0
	jal			x0,.sample_2
.sample_4:
	lui			t0,%hi(N)
	lw			t0,%lo(N)(t0)
	fcvt.s.w		ft0,t0
	fdiv.s		ft0,fs2,ft0
	fmv.s		fa0,ft0
	addi		sp,sp,64
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	fld			fs0,-24(sp)
	fld			fs1,-32(sp)
	fld			fs2,-40(sp)
	fld			fs3,-48(sp)
	fld			fs4,-56(sp)
	jalr		x0,ra,0
write_pgm:
.write_pgm_0:
	sd			ra,-8(sp)
	addi		sp,sp,-16
	jal			x0,.write_pgm_1
.write_pgm_1:
	li			t0,80
	addi		a0,t0,0
	call		putch
	li			t0,50
	addi		a0,t0,0
	call		putch
	li			t0,10
	addi		a0,t0,0
	call		putch
	lui			t0,%hi(W)
	lw			t0,%lo(W)(t0)
	addi		a0,t0,0
	call		putint
	li			t0,32
	addi		a0,t0,0
	call		putch
	lui			t0,%hi(H)
	lw			t0,%lo(H)(t0)
	addi		a0,t0,0
	call		putint
	li			t0,32
	addi		a0,t0,0
	call		putch
	li			t0,255
	addi		a0,t0,0
	call		putint
	li			t0,10
	addi		a0,t0,0
	call		putch
	li			t0,0
	li			t1,0
	fcvt.s.w		ft0,t1
	fcvt.s.w		ft1,t0
	lui			t0,%hi(W)
	lw			t0,%lo(W)(t0)
	fcvt.s.w		ft2,t0
	fdiv.s		ft0,ft0,ft2
	lui			t0,%hi(H)
	lw			t0,%lo(H)(t0)
	fcvt.s.w		ft2,t0
	fdiv.s		ft1,ft1,ft2
	fmv.s		fa0,ft0
	fmv.s		fa1,ft1
	call		sample
	fmv.s		ft0,fa0
	li			t0,1132396544
	fmv.w.x		ft1,t0
	fmul.s		ft0,ft0,ft1
	fcvt.w.s		t0,ft0,rtz
	li			t1,255
	bgt			t0,t1,.write_pgm_2
	add			t0,t0,x0
	jal			x0,.write_pgm_3
.write_pgm_2:
	li			t0,255
	add			t0,t0,x0
	jal			x0,.write_pgm_3
.write_pgm_3:
	addi		a0,t0,0
	call		putint
	li			t0,32
	addi		a0,t0,0
	call		putch
	li			t0,10
	addi		a0,t0,0
	call		putch
	addi		sp,sp,16
	ld			ra,-8(sp)
	jalr		x0,ra,0
	.data
W:
	.word	192
H:
	.word	192
N:
	.word	24
PI:
	.word	1078530011
TWO_PI:
	.word	1086918619
MAX_STEP:
	.word	10
MAX_DISTANCE:
	.word	1073741824
EPSILON:
	.word	897988541
RAND_MAX:
	.word	100000006
seed:
	.word	0
