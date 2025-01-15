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
@b = global [2 x i32] [i32 1,i32 2]
define i32 @main()
{
L0:  ;
    %r0 = alloca [10 x [10 x i32]]
    br label %L1
L1:  ;
    call void @llvm.memset.p0.i32(ptr %r0,i8 0,i32 400,i1 0)
    %r1 = getelementptr [10 x [10 x i32]], ptr %r0, i32 0, i32 0, i32 0
    %r2 = add i32 1,0
    store i32 %r2, ptr %r1
    %r4 = add i32 0,0
    %r5 = getelementptr [2 x i32], ptr @b, i32 0, i32 %r4
    %r6 = load i32, ptr %r5
    %r7 = add i32 1,0
    %r8 = add i32 1,0
    %r9 = getelementptr [10 x [10 x i32]], ptr %r0, i32 0, i32 %r7, i32 %r8
    %r10 = add i32 2,0
    store i32 %r10, ptr %r9
    %r11 = add i32 2,0
    %r12 = add i32 2,0
    %r13 = getelementptr [10 x [10 x i32]], ptr %r0, i32 0, i32 %r11, i32 %r12
    %r14 = add i32 1,0
    %r15 = getelementptr [2 x i32], ptr @b, i32 0, i32 %r14
    %r16 = load i32, ptr %r15
    store i32 %r16, ptr %r13
    ret i32 %r6
}
