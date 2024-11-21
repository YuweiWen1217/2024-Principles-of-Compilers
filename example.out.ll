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
@k = global i32 zeroinitializer
define float @params_f40(float %r0,float %r1)
{
L0:  ;
    %r4 = alloca [2 x float]
    %r3 = alloca float
    %r2 = alloca float
    store float %r0, ptr %r2
    store float %r1, ptr %r3
    br label %L1
L1:  ;
    %r5 = getelementptr [2 x float], ptr %r4, i32 0, i32 0
    %r6 = load float, ptr %r2
    store float %r6, ptr %r5
    %r7 = getelementptr [2 x float], ptr %r4, i32 0, i32 1
    %r8 = load float, ptr %r3
    store float %r8, ptr %r7
    %r9 = add i32 1,0
    %r10 = getelementptr [2 x float], ptr %r4, i32 0, i32 %r9
    %r11 = load float, ptr %r10
    call void @putfloat(float %r11)
    %r12 = add i32 0,0
    %r13 = sitofp i32 %r12 to float
    ret float %r13
}
define i32 @main()
{
L0:  ;
    %r0 = alloca float
    br label %L1
L1:  ;
    %r1 = fadd float 0x3ffd8283a0000000,0x0
    %r2 = fadd float 0x3ff527e9e0000000,0x0
    %r3 = call float @params_f40(float %r1,float %r2)
    store float %r3, ptr %r0
    %r4 = add i32 0,0
    ret i32 %r4
}
