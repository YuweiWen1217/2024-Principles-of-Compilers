declare i32 @getint()
declare i32 @getch()
declare float @getfloat()
declare i32 @getarray(ptr)
declare i32 @getfarray(ptr)
declare void @putint(i32)
declare void @putch(i32)
declare void @putfloat(float)
declare void @putarray(i32,ptr)
declare void @putfarray(i32,ptr)
declare void @_sysy_starttime(i32)
declare void @_sysy_stoptime(i32)
declare void @llvm.memset.p0.i32(ptr,i8,i32,i1)
declare i32 @llvm.umax.i32(i32,i32)
declare i32 @llvm.umin.i32(i32,i32)
declare i32 @llvm.smax.i32(i32,i32)
declare i32 @llvm.smin.i32(i32,i32)
@a = global float 0x3ff19999a0000000
@b = global i32 2
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = load float, ptr @a
    %r1 = add i32 1024,0
    %r2 = sitofp i32 %r1 to float
    %r3 = fadd float %r0,%r2
    %r4 = fcmp oeq float %r3,0x0
    %r5 = zext i1 %r4 to i32
    %r6 = icmp eq i32 %r5,0
    %r7 = zext i1 %r6 to i32
    %r8 = load float, ptr @a
    %r9 = fcmp oeq float %r8,0x0
    %r10 = zext i1 %r9 to i32
    %r11 = add i32 %r7,%r10
    %r12 = add i32 2,0
    %r13 = sub i32 %r11,%r12
    store i32 %r13, ptr @b
    ret i32 0
}
