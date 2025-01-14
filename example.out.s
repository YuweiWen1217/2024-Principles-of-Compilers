	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p0"
	.file	"example.out.ll"
	.globl	main                            # -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %L0
	li	a0, 1
	li	a0, 0
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	a,@object                       # @a
	.section	.sdata,"aw",@progbits
	.globl	a
	.p2align	2
a:
	.word	0x3f8ccccd                      # float 1.10000002
	.size	a, 4

	.type	b,@object                       # @b
	.globl	b
	.p2align	2
b:
	.word	2                               # 0x2
	.size	b, 4

	.section	".note.GNU-stack","",@progbits
