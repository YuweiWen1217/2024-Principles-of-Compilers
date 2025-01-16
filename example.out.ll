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
    %r0 = add i32 1,0
    %r1 = icmp slt i32 %r0,8
    %r2 = zext i1 %r1 to i32
    %r3 = icmp ne i32 %r2,1
    br i1 %r3, label %L2, label %L3
L2:  ;
    %r4 = add i32 0,0
    ret i32 %r4
L3:  ;
    %r5 = add i32 1,0
    ret i32 %r5
}
