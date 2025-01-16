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
@s = global i32 0
define i32 @get_ans_se(i32 %r0,i32 %r1,i32 %r2)
{
L0:  ;
    br label %L1
L1:  ;
    %r7 = add i32 0,0
    %r10 = icmp eq i32 %r1,%r2
    br i1 %r10, label %L2, label %L3
L2:  ;
    %r11 = add i32 1,0
    br label %L3
L3:  ;
    %r22 = phi i32 [%r7,%L1],[%r11,%L2]
    %r13 = add i32 2,0
    %r14 = mul i32 %r0,%r13
    %r17 = add i32 %r14,%r22
    %r18 = load i32, ptr @s
    %r20 = add i32 %r18,%r17
    store i32 %r20, ptr @s
    ret i32 %r17
}
define i32 @get_ans(i32 %r0,i32 %r1,i32 %r2)
{
L0:  ;
    br label %L1
L1:  ;
    ret i32 %r1
}
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    %r1 = add i32 -2147483648,0
    %r18 = add i32 3,0
    %r20 = sub i32 -2147483647,1
    %r21 = call i32 @get_ans(i32 %r18,i32 %r1,i32 %r20)
    ret i32 %r21
}
