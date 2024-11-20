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
@MAX_DIM_X = global i32 8
@MAX_DIM_Y = global i32 8
@test_block = global [8x [8x float]] zeroinitializer
define void @write_mat(ptr %r0,i32 %r1,i32 %r2)
{
L0:  ;
    %r14 = alloca i32
    %r5 = alloca i32
    %r4 = alloca i32
    %r3 = alloca i32
    store i32 %r1, ptr %r3
    store i32 %r2, ptr %r4
    br label %L1
L1:  ;
    %r6 = add i32 0,0
    store i32 %r6, ptr %r5
    br label %L2
L2:  ;
    %r7 = load i32, ptr %r5
    %r8 = load i32, ptr %r3
    %r9 = icmp slt i32 %r7,%r8
    br i1 %r9, label %L3, label %L4
L3:  ;
    %r10 = load i32, ptr %r5
    %r11 = add i32 0,0
    %r12 = getelementptr [8 x float], ptr %r0, i32 %r10, i32 %r11
    %r13 = load float, ptr %r12
    call void @putfloat(float %r13)
    %r15 = add i32 1,0
    store i32 %r15, ptr %r14
    br label %L5
L4:  ;
    %r31 = add i32 10,0
    call void @putch(i32 %r31)
    ret void
L5:  ;
    %r16 = load i32, ptr %r14
    %r17 = load i32, ptr %r4
    %r18 = icmp slt i32 %r16,%r17
    br i1 %r18, label %L6, label %L7
L6:  ;
    %r19 = add i32 32,0
    call void @putch(i32 %r19)
    %r20 = load i32, ptr %r5
    %r21 = load i32, ptr %r14
    %r22 = getelementptr [8 x float], ptr %r0, i32 %r20, i32 %r21
    %r23 = load float, ptr %r22
    call void @putfloat(float %r23)
    %r24 = load i32, ptr %r14
    %r25 = add i32 1,0
    %r26 = add i32 %r24,%r25
    store i32 %r26, ptr %r14
    br label %L5
L7:  ;
    %r27 = add i32 10,0
    call void @putch(i32 %r27)
    %r28 = load i32, ptr %r5
    %r29 = add i32 1,0
    %r30 = add i32 %r28,%r29
    store i32 %r30, ptr %r5
    br label %L2
}
define i32 @main()
{
L0:  ;
    %r2 = alloca i32
    %r0 = alloca i32
    br label %L1
L1:  ;
    %r1 = call i32 @getint()
    store i32 %r1, ptr %r0
    %r3 = call i32 @getint()
    store i32 %r3, ptr %r2
    %r4 = add i32 0,0
    %r5 = add i32 0,0
    %r6 = getelementptr [8 x [8 x float]], ptr @test_block, i32 0, i32 %r4, i32 %r5
    %r7 = call float @getfloat()
    store float %r7, ptr %r6
    %r8 = add i32 1,0
    %r9 = add i32 0,0
    %r10 = getelementptr [8 x [8 x float]], ptr @test_block, i32 0, i32 %r8, i32 %r9
    %r11 = call float @getfloat()
    store float %r11, ptr %r10
    %r12 = add i32 0,0
    %r13 = add i32 0,0
    %r14 = getelementptr [8 x [8 x float]], ptr @test_block, i32 0, i32 %r12, i32 %r13
    %r15 = load float, ptr %r14
    call void @putfloat(float %r15)
    %r16 = add i32 32,0
    call void @putch(i32 %r16)
    %r17 = add i32 1,0
    %r18 = add i32 0,0
    %r19 = getelementptr [8 x [8 x float]], ptr @test_block, i32 0, i32 %r17, i32 %r18
    %r20 = load float, ptr %r19
    call void @putfloat(float %r20)
    %r21 = add i32 32,0
    call void @putch(i32 %r21)
    %r22 = add i32 1,0
    %r23 = add i32 1,0
    %r24 = getelementptr [8 x [8 x float]], ptr @test_block, i32 0, i32 %r22, i32 %r23
    %r25 = load float, ptr %r24
    call void @putfloat(float %r25)
    %r26 = add i32 10,0
    call void @putch(i32 %r26)
    %r27 = add i32 0,0
    ret i32 %r27
}
