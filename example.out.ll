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
define i32 @uniquePaths(i32 %r0,i32 %r1)
{
L0:  ;
    %r4 = alloca [9 x i32]
    br label %L1
L1:  ;
    call void @llvm.memset.p0.i32(ptr %r4,i8 63,i32 36,i1 0)
    %r7 = add i32 0,0
    br label %L2
L2:  ;
    %r26 = phi i32 [%r7,%L1],[%r22,%L3]
    %r10 = icmp slt i32 %r26,%r0
    br i1 %r10, label %L3, label %L4
L3:  ;
    %r12 = add i32 3,0
    %r13 = mul i32 %r26,%r12
    %r15 = add i32 %r13,%r1
    %r16 = add i32 1,0
    %r17 = sub i32 %r15,%r16
    %r18 = getelementptr [9 x i32], ptr %r4, i32 0, i32 %r17
    %r19 = add i32 1,0
    store i32 %r19, ptr %r18
    %r21 = add i32 1,0
    %r22 = add i32 %r26,%r21
    br label %L2
L4:  ;
    %r23 = add i32 0,0
    %r24 = getelementptr [9 x i32], ptr %r4, i32 0, i32 %r23
    %r25 = load i32, ptr %r24
    ret i32 %r25
}
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    %r2 = add i32 3,0
    %r3 = add i32 3,0
    %r4 = call i32 @uniquePaths(i32 %r2,i32 %r3)
    ret i32 %r4
}
