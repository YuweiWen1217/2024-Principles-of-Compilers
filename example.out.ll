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
    br label %L1
L1:  ;
    %r17 = phi i32 
    %r16 = phi i32 
    %r1 = add i32 0,0
    store i32 %r1, ptr %r0
    %r3 = add i32 100000,0
    store i32 %r3, ptr %r2
    br label %L2
L2:  ;
    %r4 = load i32, ptr %r0
    %r5 = load i32, ptr %r2
    %r6 = icmp slt i32 %r4,%r5
    br i1 %r6, label %L3, label %L4
L3:  ;
    %r7 = load i32, ptr %r0
    %r8 = icmp sgt i32 %r7,9961
    br i1 %r8, label %L5, label %L6
L4:  ;
    ret i32 0
L5:  ;
    br label %L7
L6:  ;
    %r18 = phi i32 
    %r13 = load i32, ptr %r0
    %r14 = add i32 1,0
    %r15 = add i32 %r13,%r14
    store i32 %r15, ptr %r0
    br label %L2
L7:  ;
    %r9 = load i32, ptr %r0
    %r10 = load i32, ptr %r2
    %r11 = icmp slt i32 %r9,%r10
    br i1 %r11, label %L8, label %L9
L8:  ;
    %r12 = add i32 0,0
    ret i32 %r12
L9:  ;
    br label %L6
}
