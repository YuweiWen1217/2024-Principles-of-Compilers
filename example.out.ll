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
@RADIUS = global float 0x4016000000000000
@PI = global float 0x400921fb60000000
@EPS = global float 0x3eb0c6f7a0000000
@PI_HEX = global float 0x400921fb60000000
@HEX2 = global float 0x3fb4000000000000
@FACT = global float 0xc0e01d0000000000
@EVAL1 = global float 0x4057c21fc0000000
@EVAL2 = global float 0x4041475ce0000000
@EVAL3 = global float 0x4041475ce0000000
@CONV1 = global float 0x406d200000000000
@CONV2 = global float 0x40affe0000000000
@MAX = global i32 1000000000
@TWO = global i32 2
@THREE = global i32 3
@FIVE = global i32 5
define float @float_abs(float %r0)
{
L0:  ;
    %r1 = alloca float
    store float %r0, ptr %r1
    br label %L1
L1:  ;
    %r2 = load float, ptr %r1
    %r3 = fcmp olt float %r2,0x0
    br i1 %r3, label %L2, label %L3
L2:  ;
    %r4 = load float, ptr %r1
    %r5 = fsub float 0x0,%r4
    ret float %r5
    br label %L3
L3:  ;
    %r6 = load float, ptr %r1
    ret float %r6
}
define i32 @float_eq(float %r0,float %r1)
{
L0:  ;
    %r3 = alloca float
    %r2 = alloca float
    store float %r0, ptr %r2
    store float %r1, ptr %r3
    br label %L1
L1:  ;
    %r4 = load float, ptr %r2
    %r5 = load float, ptr %r3
    %r6 = fsub float %r4,%r5
    %r7 = call float @float_abs(float %r6)
    %r8 = fcmp olt float %r7,0x3eb0c6f7a0000000
    br i1 %r8, label %L2, label %L3
L2:  ;
    %r9 = fdiv float 0x4000000000000000,0x4000000000000000
    %r10 = fptosi float %r9 to i32
    ret i32 %r10
    br label %L4
L3:  ;
    %r11 = add i32 0,0
    ret i32 %r11
    br label %L4
L4:  ;
    ret i32 0
}
define void @error()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = add i32 101,0
    call void @putch(i32 %r0)
    %r1 = add i32 114,0
    call void @putch(i32 %r1)
    %r2 = add i32 114,0
    call void @putch(i32 %r2)
    %r3 = add i32 111,0
    call void @putch(i32 %r3)
    %r4 = add i32 114,0
    call void @putch(i32 %r4)
    %r5 = add i32 10,0
    call void @putch(i32 %r5)
    ret void
}
define float @circle_area(i32 %r0)
{
L0:  ;
    %r1 = alloca i32
    store i32 %r0, ptr %r1
    br label %L1
L1:  ;
    %r2 = load i32, ptr %r1
    %r3 = sitofp i32 %r2 to float
    %r4 = fmul float 0x400921fb60000000,%r3
    %r5 = load i32, ptr %r1
    %r6 = sitofp i32 %r5 to float
    %r7 = fmul float %r4,%r6
    %r8 = load i32, ptr %r1
    %r9 = load i32, ptr %r1
    %r10 = mul i32 %r8,%r9
    %r11 = sitofp i32 %r10 to float
    %r12 = load float, ptr @PI
    %r13 = fmul float %r11,%r12
    %r14 = fadd float %r7,%r13
    %r15 = add i32 2,0
    %r16 = sitofp i32 %r15 to float
    %r17 = fdiv float %r14,%r16
    ret float %r17
}
define void @ok()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = add i32 111,0
    call void @putch(i32 %r0)
    %r1 = add i32 107,0
    call void @putch(i32 %r1)
    %r2 = add i32 10,0
    call void @putch(i32 %r2)
    ret void
}
define void @assert(i32 %r0)
{
L0:  ;
    %r1 = alloca i32
    store i32 %r0, ptr %r1
    br label %L1
L1:  ;
    %r2 = load i32, ptr %r1
    %r3 = icmp eq i32 %r2,0
    br i1 %r3, label %L2, label %L3
L2:  ;
    call void @error()
    br label %L4
L3:  ;
    call void @ok()
    br label %L4
L4:  ;
    ret void
}
define i32 @main()
{
L0:  ;
    %r75 = alloca float
    %r70 = alloca float
    %r68 = alloca float
    %r63 = alloca i32
    %r42 = alloca [10 x float]
    %r40 = alloca i32
    %r38 = alloca i32
    br label %L1
L1:  ;
    %r0 = load float, ptr @HEX2
    %r1 = load float, ptr @FACT
    %r2 = call i32 @float_eq(float %r0,float %r1)
    %r3 = icmp eq i32 %r2,0
    %r4 = zext i1 %r3 to i32
    call void @assert(i32 %r4)
    %r5 = load float, ptr @EVAL1
    %r6 = load float, ptr @EVAL2
    %r7 = call i32 @float_eq(float %r5,float %r6)
    %r8 = icmp eq i32 %r7,0
    %r9 = zext i1 %r8 to i32
    call void @assert(i32 %r9)
    %r10 = load float, ptr @EVAL2
    %r11 = load float, ptr @EVAL3
    %r12 = call i32 @float_eq(float %r10,float %r11)
    call void @assert(i32 %r12)
    %r13 = load float, ptr @RADIUS
    %r14 = fptosi float %r13 to i32
    %r15 = call float @circle_area(i32 %r14)
    %r16 = load i32, ptr @FIVE
    %r17 = call float @circle_area(i32 %r16)
    %r18 = call i32 @float_eq(float %r15,float %r17)
    call void @assert(i32 %r18)
    %r19 = load float, ptr @CONV1
    %r20 = load float, ptr @CONV2
    %r21 = call i32 @float_eq(float %r19,float %r20)
    %r22 = icmp eq i32 %r21,0
    %r23 = zext i1 %r22 to i32
    call void @assert(i32 %r23)
    %r24 = fadd float 0x3ff8000000000000,0x0
    %r25 = fcmp one float %r24,0x0
    br i1 %r25, label %L2, label %L3
L2:  ;
    call void @ok()
    br label %L3
L3:  ;
    %r26 = fadd float 0x400a666660000000,0x0
    %r27 = fcmp oeq float %r26,0x0
    %r28 = zext i1 %r27 to i32
    %r29 = icmp eq i32 %r28,0
    br i1 %r29, label %L4, label %L5
L4:  ;
    call void @ok()
    br label %L5
L5:  ;
    %r30 = fadd float 0x0,0x0
    %r31 = fcmp one float %r30,0x0
    br i1 %r31, label %L8, label %L7
L6:  ;
    call void @error()
    br label %L7
L7:  ;
    %r34 = add i32 0,0
    %r35 = icmp ne i32 %r34,0
    br i1 %r35, label %L9, label %L11
L8:  ;
    %r32 = add i32 3,0
    %r33 = icmp ne i32 %r32,0
    br i1 %r33, label %L6, label %L7
L9:  ;
    call void @ok()
    br label %L10
L10:  ;
    %r39 = add i32 1,0
    store i32 %r39, ptr %r38
    %r41 = add i32 0,0
    store i32 %r41, ptr %r40
    %r43 = getelementptr [10 x float], ptr %r42, i32 0, i32 0
    %r44 = fadd float 0x3ff0000000000000,0x0
    store float %r44, ptr %r43
    %r45 = getelementptr [10 x float], ptr %r42, i32 0, i32 1
    %r46 = fadd float 0x4000000000000000,0x0
    store float %r46, ptr %r45
    %r47 = getelementptr [10 x float], ptr %r42, i32 0, i32 2
    %r48 = fadd float 0x0,0x0
    store float %r48, ptr %r47
    %r49 = getelementptr [10 x float], ptr %r42, i32 0, i32 3
    %r50 = fadd float 0x0,0x0
    store float %r50, ptr %r49
    %r51 = getelementptr [10 x float], ptr %r42, i32 0, i32 4
    %r52 = fadd float 0x0,0x0
    store float %r52, ptr %r51
    %r53 = getelementptr [10 x float], ptr %r42, i32 0, i32 5
    %r54 = fadd float 0x0,0x0
    store float %r54, ptr %r53
    %r55 = getelementptr [10 x float], ptr %r42, i32 0, i32 6
    %r56 = fadd float 0x0,0x0
    store float %r56, ptr %r55
    %r57 = getelementptr [10 x float], ptr %r42, i32 0, i32 7
    %r58 = fadd float 0x0,0x0
    store float %r58, ptr %r57
    %r59 = getelementptr [10 x float], ptr %r42, i32 0, i32 8
    %r60 = fadd float 0x0,0x0
    store float %r60, ptr %r59
    %r61 = getelementptr [10 x float], ptr %r42, i32 0, i32 9
    %r62 = fadd float 0x0,0x0
    store float %r62, ptr %r61
    %r64 = getelementptr [10 x float], ptr %r42, i32 0
    %r65 = call i32 @getfarray(ptr %r64)
    store i32 %r65, ptr %r63
    br label %L12
L11:  ;
    %r36 = fadd float 0x3fd3333340000000,0x0
    %r37 = fcmp one float %r36,0x0
    br i1 %r37, label %L9, label %L10
L12:  ;
    %r66 = load i32, ptr %r38
    %r67 = icmp slt i32 %r66,1000000000
    br i1 %r67, label %L13, label %L14
L13:  ;
    %r69 = call float @getfloat()
    store float %r69, ptr %r68
    %r71 = load float, ptr %r68
    %r72 = fmul float 0x400921fb60000000,%r71
    %r73 = load float, ptr %r68
    %r74 = fmul float %r72,%r73
    store float %r74, ptr %r70
    %r76 = load float, ptr %r68
    %r77 = fptosi float %r76 to i32
    %r78 = call float @circle_area(i32 %r77)
    store float %r78, ptr %r75
    %r79 = load i32, ptr %r40
    %r80 = getelementptr [10 x float], ptr %r42, i32 0, i32 %r79
    %r81 = load i32, ptr %r40
    %r82 = getelementptr [10 x float], ptr %r42, i32 0, i32 %r81
    %r83 = load float, ptr %r82
    %r84 = load float, ptr %r68
    %r85 = fadd float %r83,%r84
    store float %r85, ptr %r80
    %r86 = load float, ptr %r70
    call void @putfloat(float %r86)
    %r87 = add i32 32,0
    call void @putch(i32 %r87)
    %r88 = load float, ptr %r75
    %r89 = fptosi float %r88 to i32
    call void @putint(i32 %r89)
    %r90 = add i32 10,0
    call void @putch(i32 %r90)
    %r91 = load i32, ptr %r38
    %r92 = sitofp i32 %r91 to float
    %r93 = fadd float 0x4024000000000000,0x0
    %r94 = fsub float 0x0,%r93
    %r95 = fsub float 0x0,%r94
    %r96 = fmul float %r92,%r95
    %r97 = fptosi float %r96 to i32
    store i32 %r97, ptr %r38
    %r98 = load i32, ptr %r40
    %r99 = add i32 1,0
    %r100 = add i32 %r98,%r99
    store i32 %r100, ptr %r40
    br label %L12
L14:  ;
    %r101 = load i32, ptr %r63
    %r102 = getelementptr [10 x float], ptr %r42, i32 0
    call void @putfarray(i32 %r101,ptr %r102)
    %r103 = add i32 0,0
    ret i32 %r103
}
