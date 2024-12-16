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
    %r5 = add i32 3,0
    %r7 = add i32 4,0
    %r9 = add i32 5,0
    %r11 = add i32 6,0
    %r12 = add i32 2,3
    %r13 = icmp slt i32 %r12,6
    br i1 %r13, label %L5, label %L3
L2:  ;
    %r17 = icmp ne i32 %r9,0
    br i1 %r17, label %L6, label %L9
L3:  ;
    %r49 = add i32 1,0
    ret i32 %r49
L5:  ;
    %r15 = icmp ne i32 %r7,0
    br i1 %r15, label %L2, label %L3
L6:  ;
    %r21 = icmp eq i32 %r5,2
    br i1 %r21, label %L13, label %L11
L7:  ;
    %r48 = add i32 2,0
    ret i32 %r48
L9:  ;
    %r18 = add i32 0,0
    %r19 = icmp ne i32 %r18,0
    br i1 %r19, label %L6, label %L7
L10:  ;
    %r24 = add i32 3,0
    ret i32 %r24
L11:  ;
    %r25 = srem i32 6,3
    %r26 = icmp ne i32 %r25,0
    br i1 %r26, label %L17, label %L15
L13:  ;
    %r22 = add i32 4,5
    %r23 = icmp sgt i32 %r22,2
    br i1 %r23, label %L10, label %L11
L14:  ;
    %r29 = add i32 4,0
    ret i32 %r29
L15:  ;
    %r30 = add i32 2,1
    %r31 = icmp sge i32 %r30,2
    br i1 %r31, label %L18, label %L19
L17:  ;
    %r28 = icmp ne i32 %r9,0
    br i1 %r28, label %L14, label %L15
L18:  ;
    %r32 = sub i32 5,6
    %r33 = icmp sge i32 %r32,0
    br i1 %r33, label %L21, label %L24
L19:  ;
    %r47 = add i32 5,0
    ret i32 %r47
L21:  ;
    %r36 = add i32 6,0
    ret i32 %r36
L22:  ;
    %r38 = icmp ne i32 %r5,6
    br i1 %r38, label %L25, label %L26
L24:  ;
    %r35 = icmp sgt i32 %r7,4
    br i1 %r35, label %L21, label %L22
L25:  ;
    %r39 = add i32 2,20
    %r40 = icmp sgt i32 %r39,10
    br i1 %r40, label %L28, label %L29
L26:  ;
    %r46 = add i32 7,0
    ret i32 %r46
L28:  ;
    %r42 = icmp eq i32 %r11,0
    br i1 %r42, label %L31, label %L32
L29:  ;
    %r45 = add i32 8,0
    ret i32 %r45
L31:  ;
    %r43 = add i32 9,0
    ret i32 %r43
L32:  ;
    %r44 = add i32 10,0
    ret i32 %r44
}
