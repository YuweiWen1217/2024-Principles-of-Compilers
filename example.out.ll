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
define i32 @main()
{
L0:  ;
    %r2 = alloca i32
    %r0 = alloca i32
    br label %L1
L1:  ;
    %r1 = add i32 0,0
    store i32 %r1, ptr %r0
    %r3 = add i32 0,0
    store i32 %r3, ptr %r2
    %r4 = add i32 56,0
    store i32 %r4, ptr %r0
    %r5 = add i32 4,0
    store i32 %r5, ptr %r2
    %r6 = load i32, ptr %r0
    %r7 = add i32 4,0
    %r8 = sub i32 0,%r7
    %r9 = sub i32 %r6,%r8
    %r10 = load i32, ptr %r2
    %r11 = add i32 %r9,%r10
    store i32 %r11, ptr %r0
    %r12 = load i32, ptr %r0
    %r13 = icmp eq i32 %r12,0
    %r14 = zext i1 %r13 to i32
    %r15 = icmp eq i32 %r14,0
    %r16 = zext i1 %r15 to i32
    %r17 = icmp eq i32 %r16,0
    %r18 = zext i1 %r17 to i32
    %r19 = sub i32 0,%r18
    %r20 = icmp ne i32 %r19,0
    br i1 %r20, label %L2, label %L3
L2:  ;
    %r21 = add i32 1,0
    %r22 = sub i32 0,%r21
    %r23 = sub i32 0,%r22
    %r24 = sub i32 0,%r23
    store i32 %r24, ptr %r0
    br label %L4
L3:  ;
    %r25 = load i32, ptr %r2
    %r26 = add i32 0,%r25
    store i32 %r26, ptr %r0
    br label %L4
L4:  ;
    %r27 = load i32, ptr %r0
    call void @putint(i32 %r27)
    %r28 = add i32 0,0
    ret i32 %r28
}
