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
@a = global i32 zeroinitializer
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = add i32 10,0
    store i32 %r0, ptr @a
    %r1 = load i32, ptr @a
    %r2 = icmp sgt i32 %r1,0
    br i1 %r2, label %L2, label %L3
L2:  ;
    %r3 = add i32 1,0
    ret i32 %r3
L3:  ;
    %r4 = add i32 0,0
    ret i32 %r4
}
