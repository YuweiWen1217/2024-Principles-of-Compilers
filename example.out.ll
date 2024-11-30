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
    %r9 = alloca i32
    %r0 = alloca i32
    br label %L1
L1:  ;
    %r1 = add i32 5,0
    store i32 %r1, ptr %r0
    %r2 = load i32, ptr %r0
    %r3 = icmp sgt i32 %r2,5
    br i1 %r3, label %L2, label %L3
L2:  ;
    %r4 = add i32 5,0
    ret i32 %r4
L3:  ;
    %r8 = add i32 0,0
    ret i32 %r8
}
