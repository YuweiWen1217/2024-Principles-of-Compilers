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
    %r1 = add i32 5,0
    store i32 %r1, ptr %r0
    %r3 = add i32 4,0
    store i32 %r3, ptr %r2
    %r4 = load i32, ptr %r0
    %r5 = icmp sgt i32 %r4,2
    br i1 %r5, label %L2, label %L3
L2:  ;
    %r6 = load i32, ptr %r2
    store i32 %r6, ptr %r0
    br label %L4
L3:  ;
    %r7 = add i32 0,0
    ret i32 %r7
L4:  ;
    %r14 = load i32, ptr %r0
    %r15 = icmp sge i32 %r14,4
    br i1 %r15, label %L8, label %L9
L8:  ;
    %r16 = add i32 2,0
    store i32 %r16, ptr %r2
    br label %L10
L9:  ;
    %r17 = add i32 3,0
    store i32 %r17, ptr %r0
    br label %L10
L10:  ;
    %r18 = add i32 0,0
    ret i32 %r18
}
