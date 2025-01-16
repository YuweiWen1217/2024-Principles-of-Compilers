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
    %r1 = add i32 1,0
    br label %L2
L2:  ;
    %r8 = phi i32 [%r1,%L1],[%r6,%L3]
    %r3 = icmp slt i32 %r8,4
    br i1 %r3, label %L3, label %L4
L3:  ;
    %r5 = add i32 1,0
    %r6 = add i32 %r8,%r5
    br label %L2
L4:  ;
    ret i32 %r8
}
