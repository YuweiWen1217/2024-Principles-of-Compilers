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
@ints = global [10000x i32] zeroinitializer
@intt = global i32 zeroinitializer
@chas = global [10000x i32] zeroinitializer
@chat = global i32 zeroinitializer
@i = global i32 0
@ii = global i32 1
@c = global i32 zeroinitializer
@get = global [10000x i32] zeroinitializer
@get2 = global [10000x i32] zeroinitializer
define i32 @isdigit(i32 %r0)
{
L0:  ;
    br label %L1
L1:  ;
    %r3 = icmp sge i32 %r0,48
    br i1 %r3, label %L4, label %L3
L2:  ;
    %r6 = add i32 1,0
    ret i32 %r6
L3:  ;
    %r7 = add i32 0,0
    ret i32 %r7
L4:  ;
    %r5 = icmp sle i32 %r0,57
    br i1 %r5, label %L2, label %L3
}
define i32 @getstr(ptr %r0)
{
L0:  ;
    br label %L1
L1:  ;
    %r2 = call i32 @getch()
    %r4 = add i32 0,0
    br label %L2
L2:  ;
    %r18 = phi i32 [%r2,%L1],[%r15,%L3]
    %r17 = phi i32 [%r4,%L1],[%r14,%L3]
    %r6 = icmp ne i32 %r18,13
    br i1 %r6, label %L5, label %L4
L3:  ;
    %r10 = getelementptr i32, ptr %r0, i32 %r17
    store i32 %r18, ptr %r10
    %r13 = add i32 1,0
    %r14 = add i32 %r17,%r13
    %r15 = call i32 @getch()
    br label %L2
L4:  ;
    ret i32 %r17
L5:  ;
    %r8 = icmp ne i32 %r18,10
    br i1 %r8, label %L3, label %L4
}
define i32 @power(i32 %r0,i32 %r1)
{
L0:  ;
    br label %L1
L1:  ;
    %r5 = add i32 1,0
    br label %L2
L2:  ;
    %r16 = phi i32 [%r1,%L1],[%r13,%L3]
    %r15 = phi i32 [%r5,%L1],[%r10,%L3]
    %r7 = icmp ne i32 %r16,0
    br i1 %r7, label %L3, label %L4
L3:  ;
    %r10 = mul i32 %r15,%r0
    %r12 = add i32 1,0
    %r13 = sub i32 %r16,%r12
    br label %L2
L4:  ;
    ret i32 %r15
}
define void @intpush(i32 %r0)
{
L0:  ;
    br label %L1
L1:  ;
    %r2 = load i32, ptr @intt
    %r3 = add i32 1,0
    %r4 = add i32 %r2,%r3
    store i32 %r4, ptr @intt
    %r5 = load i32, ptr @intt
    %r6 = getelementptr [10000 x i32], ptr @ints, i32 0, i32 %r5
    store i32 %r0, ptr %r6
    ret void
}
define void @chapush(i32 %r0)
{
L0:  ;
    br label %L1
L1:  ;
    %r2 = load i32, ptr @chat
    %r3 = add i32 1,0
    %r4 = add i32 %r2,%r3
    store i32 %r4, ptr @chat
    %r5 = load i32, ptr @chat
    %r6 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r5
    store i32 %r0, ptr %r6
    ret void
}
define i32 @intpop()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = load i32, ptr @intt
    %r1 = add i32 1,0
    %r2 = sub i32 %r0,%r1
    store i32 %r2, ptr @intt
    %r3 = load i32, ptr @intt
    %r4 = add i32 1,0
    %r5 = add i32 %r3,%r4
    %r6 = getelementptr [10000 x i32], ptr @ints, i32 0, i32 %r5
    %r7 = load i32, ptr %r6
    ret i32 %r7
}
define i32 @chapop()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = load i32, ptr @chat
    %r1 = add i32 1,0
    %r2 = sub i32 %r0,%r1
    store i32 %r2, ptr @chat
    %r3 = load i32, ptr @chat
    %r4 = add i32 1,0
    %r5 = add i32 %r3,%r4
    %r6 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r5
    %r7 = load i32, ptr %r6
    ret i32 %r7
}
define void @intadd(i32 %r0)
{
L0:  ;
    br label %L1
L1:  ;
    %r2 = load i32, ptr @intt
    %r3 = getelementptr [10000 x i32], ptr @ints, i32 0, i32 %r2
    %r4 = load i32, ptr @intt
    %r5 = getelementptr [10000 x i32], ptr @ints, i32 0, i32 %r4
    %r6 = load i32, ptr %r5
    %r7 = add i32 10,0
    %r8 = mul i32 %r6,%r7
    store i32 %r8, ptr %r3
    %r9 = load i32, ptr @intt
    %r10 = getelementptr [10000 x i32], ptr @ints, i32 0, i32 %r9
    %r11 = load i32, ptr @intt
    %r12 = getelementptr [10000 x i32], ptr @ints, i32 0, i32 %r11
    %r13 = load i32, ptr %r12
    %r15 = add i32 %r13,%r0
    store i32 %r15, ptr %r10
    ret void
}
define i32 @find()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = call i32 @chapop()
    store i32 %r0, ptr @c
    %r1 = load i32, ptr @ii
    %r2 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r1
    %r3 = add i32 32,0
    store i32 %r3, ptr %r2
    %r4 = load i32, ptr @ii
    %r5 = add i32 1,0
    %r6 = add i32 %r4,%r5
    %r7 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r6
    %r8 = load i32, ptr @c
    store i32 %r8, ptr %r7
    %r9 = load i32, ptr @ii
    %r10 = add i32 2,0
    %r11 = add i32 %r9,%r10
    store i32 %r11, ptr @ii
    %r12 = load i32, ptr @chat
    %r13 = icmp eq i32 %r12,0
    br i1 %r13, label %L2, label %L3
L2:  ;
    %r14 = add i32 0,0
    ret i32 %r14
L3:  ;
    %r15 = add i32 1,0
    ret i32 %r15
}
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = add i32 0,0
    store i32 %r0, ptr @intt
    %r1 = add i32 0,0
    store i32 %r1, ptr @chat
    %r3 = getelementptr [10000 x i32], ptr @get, i32 0
    %r4 = call i32 @getstr(ptr %r3)
    br label %L2
L2:  ;
    %r5 = load i32, ptr @i
    %r7 = icmp slt i32 %r5,%r4
    br i1 %r7, label %L3, label %L4
L3:  ;
    %r8 = load i32, ptr @i
    %r9 = getelementptr [10000 x i32], ptr @get, i32 0, i32 %r8
    %r10 = load i32, ptr %r9
    %r11 = call i32 @isdigit(i32 %r10)
    %r12 = icmp eq i32 %r11,1
    br i1 %r12, label %L5, label %L6
L4:  ;
    br label %L76
L5:  ;
    %r13 = load i32, ptr @ii
    %r14 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r13
    %r15 = load i32, ptr @i
    %r16 = getelementptr [10000 x i32], ptr @get, i32 0, i32 %r15
    %r17 = load i32, ptr %r16
    store i32 %r17, ptr %r14
    %r18 = load i32, ptr @ii
    %r19 = add i32 1,0
    %r20 = add i32 %r18,%r19
    store i32 %r20, ptr @ii
    br label %L7
L6:  ;
    %r21 = load i32, ptr @i
    %r22 = getelementptr [10000 x i32], ptr @get, i32 0, i32 %r21
    %r23 = load i32, ptr %r22
    %r24 = icmp eq i32 %r23,40
    br i1 %r24, label %L8, label %L9
L7:  ;
    %r187 = load i32, ptr @i
    %r188 = add i32 1,0
    %r189 = add i32 %r187,%r188
    store i32 %r189, ptr @i
    br label %L2
L8:  ;
    %r25 = add i32 40,0
    call void @chapush(i32 %r25)
    br label %L9
L9:  ;
    %r26 = load i32, ptr @i
    %r27 = getelementptr [10000 x i32], ptr @get, i32 0, i32 %r26
    %r28 = load i32, ptr %r27
    %r29 = icmp eq i32 %r28,94
    br i1 %r29, label %L10, label %L11
L10:  ;
    %r30 = add i32 94,0
    call void @chapush(i32 %r30)
    br label %L11
L11:  ;
    %r31 = load i32, ptr @i
    %r32 = getelementptr [10000 x i32], ptr @get, i32 0, i32 %r31
    %r33 = load i32, ptr %r32
    %r34 = icmp eq i32 %r33,41
    br i1 %r34, label %L12, label %L13
L12:  ;
    %r35 = call i32 @chapop()
    store i32 %r35, ptr @c
    br label %L14
L13:  ;
    %r50 = load i32, ptr @i
    %r51 = getelementptr [10000 x i32], ptr @get, i32 0, i32 %r50
    %r52 = load i32, ptr %r51
    %r53 = icmp eq i32 %r52,43
    br i1 %r53, label %L17, label %L18
L14:  ;
    %r36 = load i32, ptr @c
    %r37 = icmp ne i32 %r36,40
    br i1 %r37, label %L15, label %L16
L15:  ;
    %r38 = load i32, ptr @ii
    %r39 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r38
    %r40 = add i32 32,0
    store i32 %r40, ptr %r39
    %r41 = load i32, ptr @ii
    %r42 = add i32 1,0
    %r43 = add i32 %r41,%r42
    %r44 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r43
    %r45 = load i32, ptr @c
    store i32 %r45, ptr %r44
    %r46 = load i32, ptr @ii
    %r47 = add i32 2,0
    %r48 = add i32 %r46,%r47
    store i32 %r48, ptr @ii
    %r49 = call i32 @chapop()
    store i32 %r49, ptr @c
    br label %L14
L16:  ;
    br label %L13
L17:  ;
    br label %L19
L18:  ;
    %r81 = load i32, ptr @i
    %r82 = getelementptr [10000 x i32], ptr @get, i32 0, i32 %r81
    %r83 = load i32, ptr %r82
    %r84 = icmp eq i32 %r83,45
    br i1 %r84, label %L30, label %L31
L19:  ;
    %r54 = load i32, ptr @chat
    %r55 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r54
    %r56 = load i32, ptr %r55
    %r57 = icmp eq i32 %r56,43
    br i1 %r57, label %L20, label %L26
L20:  ;
    %r78 = call i32 @find()
    %r79 = icmp eq i32 %r78,0
    br i1 %r79, label %L27, label %L28
L21:  ;
    %r80 = add i32 43,0
    call void @chapush(i32 %r80)
    br label %L18
L22:  ;
    %r74 = load i32, ptr @chat
    %r75 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r74
    %r76 = load i32, ptr %r75
    %r77 = icmp eq i32 %r76,94
    br i1 %r77, label %L20, label %L21
L23:  ;
    %r70 = load i32, ptr @chat
    %r71 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r70
    %r72 = load i32, ptr %r71
    %r73 = icmp eq i32 %r72,37
    br i1 %r73, label %L20, label %L22
L24:  ;
    %r66 = load i32, ptr @chat
    %r67 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r66
    %r68 = load i32, ptr %r67
    %r69 = icmp eq i32 %r68,47
    br i1 %r69, label %L20, label %L23
L25:  ;
    %r62 = load i32, ptr @chat
    %r63 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r62
    %r64 = load i32, ptr %r63
    %r65 = icmp eq i32 %r64,42
    br i1 %r65, label %L20, label %L24
L26:  ;
    %r58 = load i32, ptr @chat
    %r59 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r58
    %r60 = load i32, ptr %r59
    %r61 = icmp eq i32 %r60,45
    br i1 %r61, label %L20, label %L25
L27:  ;
    br label %L21
L28:  ;
    br label %L19
L30:  ;
    br label %L32
L31:  ;
    %r112 = load i32, ptr @i
    %r113 = getelementptr [10000 x i32], ptr @get, i32 0, i32 %r112
    %r114 = load i32, ptr %r113
    %r115 = icmp eq i32 %r114,42
    br i1 %r115, label %L43, label %L44
L32:  ;
    %r85 = load i32, ptr @chat
    %r86 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r85
    %r87 = load i32, ptr %r86
    %r88 = icmp eq i32 %r87,43
    br i1 %r88, label %L33, label %L39
L33:  ;
    %r109 = call i32 @find()
    %r110 = icmp eq i32 %r109,0
    br i1 %r110, label %L40, label %L41
L34:  ;
    %r111 = add i32 45,0
    call void @chapush(i32 %r111)
    br label %L31
L35:  ;
    %r105 = load i32, ptr @chat
    %r106 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r105
    %r107 = load i32, ptr %r106
    %r108 = icmp eq i32 %r107,94
    br i1 %r108, label %L33, label %L34
L36:  ;
    %r101 = load i32, ptr @chat
    %r102 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r101
    %r103 = load i32, ptr %r102
    %r104 = icmp eq i32 %r103,37
    br i1 %r104, label %L33, label %L35
L37:  ;
    %r97 = load i32, ptr @chat
    %r98 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r97
    %r99 = load i32, ptr %r98
    %r100 = icmp eq i32 %r99,47
    br i1 %r100, label %L33, label %L36
L38:  ;
    %r93 = load i32, ptr @chat
    %r94 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r93
    %r95 = load i32, ptr %r94
    %r96 = icmp eq i32 %r95,42
    br i1 %r96, label %L33, label %L37
L39:  ;
    %r89 = load i32, ptr @chat
    %r90 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r89
    %r91 = load i32, ptr %r90
    %r92 = icmp eq i32 %r91,45
    br i1 %r92, label %L33, label %L38
L40:  ;
    br label %L34
L41:  ;
    br label %L32
L43:  ;
    br label %L45
L44:  ;
    %r135 = load i32, ptr @i
    %r136 = getelementptr [10000 x i32], ptr @get, i32 0, i32 %r135
    %r137 = load i32, ptr %r136
    %r138 = icmp eq i32 %r137,47
    br i1 %r138, label %L54, label %L55
L45:  ;
    %r116 = load i32, ptr @chat
    %r117 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r116
    %r118 = load i32, ptr %r117
    %r119 = icmp eq i32 %r118,42
    br i1 %r119, label %L46, label %L50
L46:  ;
    %r132 = call i32 @find()
    %r133 = icmp eq i32 %r132,0
    br i1 %r133, label %L51, label %L52
L47:  ;
    %r134 = add i32 42,0
    call void @chapush(i32 %r134)
    br label %L44
L48:  ;
    %r128 = load i32, ptr @chat
    %r129 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r128
    %r130 = load i32, ptr %r129
    %r131 = icmp eq i32 %r130,94
    br i1 %r131, label %L46, label %L47
L49:  ;
    %r124 = load i32, ptr @chat
    %r125 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r124
    %r126 = load i32, ptr %r125
    %r127 = icmp eq i32 %r126,37
    br i1 %r127, label %L46, label %L48
L50:  ;
    %r120 = load i32, ptr @chat
    %r121 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r120
    %r122 = load i32, ptr %r121
    %r123 = icmp eq i32 %r122,47
    br i1 %r123, label %L46, label %L49
L51:  ;
    br label %L47
L52:  ;
    br label %L45
L54:  ;
    br label %L56
L55:  ;
    %r158 = load i32, ptr @i
    %r159 = getelementptr [10000 x i32], ptr @get, i32 0, i32 %r158
    %r160 = load i32, ptr %r159
    %r161 = icmp eq i32 %r160,37
    br i1 %r161, label %L65, label %L66
L56:  ;
    %r139 = load i32, ptr @chat
    %r140 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r139
    %r141 = load i32, ptr %r140
    %r142 = icmp eq i32 %r141,42
    br i1 %r142, label %L57, label %L61
L57:  ;
    %r155 = call i32 @find()
    %r156 = icmp eq i32 %r155,0
    br i1 %r156, label %L62, label %L63
L58:  ;
    %r157 = add i32 47,0
    call void @chapush(i32 %r157)
    br label %L55
L59:  ;
    %r151 = load i32, ptr @chat
    %r152 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r151
    %r153 = load i32, ptr %r152
    %r154 = icmp eq i32 %r153,94
    br i1 %r154, label %L57, label %L58
L60:  ;
    %r147 = load i32, ptr @chat
    %r148 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r147
    %r149 = load i32, ptr %r148
    %r150 = icmp eq i32 %r149,37
    br i1 %r150, label %L57, label %L59
L61:  ;
    %r143 = load i32, ptr @chat
    %r144 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r143
    %r145 = load i32, ptr %r144
    %r146 = icmp eq i32 %r145,47
    br i1 %r146, label %L57, label %L60
L62:  ;
    br label %L58
L63:  ;
    br label %L56
L65:  ;
    br label %L67
L66:  ;
    %r181 = load i32, ptr @ii
    %r182 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r181
    %r183 = add i32 32,0
    store i32 %r183, ptr %r182
    %r184 = load i32, ptr @ii
    %r185 = add i32 1,0
    %r186 = add i32 %r184,%r185
    store i32 %r186, ptr @ii
    br label %L7
L67:  ;
    %r162 = load i32, ptr @chat
    %r163 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r162
    %r164 = load i32, ptr %r163
    %r165 = icmp eq i32 %r164,42
    br i1 %r165, label %L68, label %L72
L68:  ;
    %r178 = call i32 @find()
    %r179 = icmp eq i32 %r178,0
    br i1 %r179, label %L73, label %L74
L69:  ;
    %r180 = add i32 37,0
    call void @chapush(i32 %r180)
    br label %L66
L70:  ;
    %r174 = load i32, ptr @chat
    %r175 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r174
    %r176 = load i32, ptr %r175
    %r177 = icmp eq i32 %r176,94
    br i1 %r177, label %L68, label %L69
L71:  ;
    %r170 = load i32, ptr @chat
    %r171 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r170
    %r172 = load i32, ptr %r171
    %r173 = icmp eq i32 %r172,37
    br i1 %r173, label %L68, label %L70
L72:  ;
    %r166 = load i32, ptr @chat
    %r167 = getelementptr [10000 x i32], ptr @chas, i32 0, i32 %r166
    %r168 = load i32, ptr %r167
    %r169 = icmp eq i32 %r168,47
    br i1 %r169, label %L68, label %L71
L73:  ;
    br label %L69
L74:  ;
    br label %L67
L76:  ;
    %r190 = load i32, ptr @chat
    %r191 = icmp sgt i32 %r190,0
    br i1 %r191, label %L77, label %L78
L77:  ;
    %r193 = call i32 @chapop()
    %r194 = load i32, ptr @ii
    %r195 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r194
    %r196 = add i32 32,0
    store i32 %r196, ptr %r195
    %r197 = load i32, ptr @ii
    %r198 = add i32 1,0
    %r199 = add i32 %r197,%r198
    %r200 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r199
    store i32 %r193, ptr %r200
    %r202 = load i32, ptr @ii
    %r203 = add i32 2,0
    %r204 = add i32 %r202,%r203
    store i32 %r204, ptr @ii
    br label %L76
L78:  ;
    %r205 = load i32, ptr @ii
    %r206 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r205
    %r207 = add i32 64,0
    store i32 %r207, ptr %r206
    %r208 = add i32 1,0
    store i32 %r208, ptr @i
    br label %L79
L79:  ;
    %r209 = load i32, ptr @i
    %r210 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r209
    %r211 = load i32, ptr %r210
    %r212 = icmp ne i32 %r211,64
    br i1 %r212, label %L80, label %L81
L80:  ;
    %r213 = load i32, ptr @i
    %r214 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r213
    %r215 = load i32, ptr %r214
    %r216 = icmp eq i32 %r215,43
    br i1 %r216, label %L82, label %L89
L81:  ;
    %r320 = add i32 1,0
    %r321 = getelementptr [10000 x i32], ptr @ints, i32 0, i32 %r320
    %r322 = load i32, ptr %r321
    call void @putint(i32 %r322)
    %r323 = add i32 0,0
    ret i32 %r323
L82:  ;
    %r238 = call i32 @intpop()
    %r240 = call i32 @intpop()
    %r242 = add i32 0,0
    %r243 = load i32, ptr @i
    %r244 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r243
    %r245 = load i32, ptr %r244
    %r246 = icmp eq i32 %r245,43
    br i1 %r246, label %L90, label %L91
L83:  ;
    %r286 = load i32, ptr @i
    %r287 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r286
    %r288 = load i32, ptr %r287
    %r289 = icmp ne i32 %r288,32
    br i1 %r289, label %L102, label %L103
L84:  ;
    %r317 = load i32, ptr @i
    %r318 = add i32 1,0
    %r319 = add i32 %r317,%r318
    store i32 %r319, ptr @i
    br label %L79
L85:  ;
    %r233 = load i32, ptr @i
    %r234 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r233
    %r235 = load i32, ptr %r234
    %r236 = icmp eq i32 %r235,94
    br i1 %r236, label %L82, label %L83
L86:  ;
    %r229 = load i32, ptr @i
    %r230 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r229
    %r231 = load i32, ptr %r230
    %r232 = icmp eq i32 %r231,37
    br i1 %r232, label %L82, label %L85
L87:  ;
    %r225 = load i32, ptr @i
    %r226 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r225
    %r227 = load i32, ptr %r226
    %r228 = icmp eq i32 %r227,47
    br i1 %r228, label %L82, label %L86
L88:  ;
    %r221 = load i32, ptr @i
    %r222 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r221
    %r223 = load i32, ptr %r222
    %r224 = icmp eq i32 %r223,42
    br i1 %r224, label %L82, label %L87
L89:  ;
    %r217 = load i32, ptr @i
    %r218 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r217
    %r219 = load i32, ptr %r218
    %r220 = icmp eq i32 %r219,45
    br i1 %r220, label %L82, label %L88
L90:  ;
    %r249 = add i32 %r238,%r240
    br label %L91
L91:  ;
    %r335 = phi i32 [%r242,%L82],[%r249,%L90]
    %r250 = load i32, ptr @i
    %r251 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r250
    %r252 = load i32, ptr %r251
    %r253 = icmp eq i32 %r252,45
    br i1 %r253, label %L92, label %L93
L92:  ;
    %r256 = sub i32 %r240,%r238
    br label %L93
L93:  ;
    %r331 = phi i32 [%r335,%L91],[%r256,%L92]
    %r257 = load i32, ptr @i
    %r258 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r257
    %r259 = load i32, ptr %r258
    %r260 = icmp eq i32 %r259,42
    br i1 %r260, label %L94, label %L95
L94:  ;
    %r263 = mul i32 %r238,%r240
    br label %L95
L95:  ;
    %r330 = phi i32 [%r331,%L93],[%r263,%L94]
    %r264 = load i32, ptr @i
    %r265 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r264
    %r266 = load i32, ptr %r265
    %r267 = icmp eq i32 %r266,47
    br i1 %r267, label %L96, label %L97
L96:  ;
    %r270 = sdiv i32 %r240,%r238
    br label %L97
L97:  ;
    %r333 = phi i32 [%r330,%L95],[%r270,%L96]
    %r271 = load i32, ptr @i
    %r272 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r271
    %r273 = load i32, ptr %r272
    %r274 = icmp eq i32 %r273,37
    br i1 %r274, label %L98, label %L99
L98:  ;
    %r277 = srem i32 %r240,%r238
    br label %L99
L99:  ;
    %r329 = phi i32 [%r333,%L97],[%r277,%L98]
    %r278 = load i32, ptr @i
    %r279 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r278
    %r280 = load i32, ptr %r279
    %r281 = icmp eq i32 %r280,94
    br i1 %r281, label %L100, label %L101
L100:  ;
    %r284 = call i32 @power(i32 %r240,i32 %r238)
    br label %L101
L101:  ;
    %r328 = phi i32 [%r329,%L99],[%r284,%L100]
    call void @intpush(i32 %r328)
    br label %L84
L102:  ;
    %r290 = load i32, ptr @i
    %r291 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r290
    %r292 = load i32, ptr %r291
    %r293 = add i32 48,0
    %r294 = sub i32 %r292,%r293
    call void @intpush(i32 %r294)
    %r295 = add i32 1,0
    store i32 %r295, ptr @ii
    br label %L104
L103:  ;
    br label %L84
L104:  ;
    %r296 = load i32, ptr @i
    %r297 = load i32, ptr @ii
    %r298 = add i32 %r296,%r297
    %r299 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r298
    %r300 = load i32, ptr %r299
    %r301 = icmp ne i32 %r300,32
    br i1 %r301, label %L105, label %L106
L105:  ;
    %r302 = load i32, ptr @i
    %r303 = load i32, ptr @ii
    %r304 = add i32 %r302,%r303
    %r305 = getelementptr [10000 x i32], ptr @get2, i32 0, i32 %r304
    %r306 = load i32, ptr %r305
    %r307 = add i32 48,0
    %r308 = sub i32 %r306,%r307
    call void @intadd(i32 %r308)
    %r309 = load i32, ptr @ii
    %r310 = add i32 1,0
    %r311 = add i32 %r309,%r310
    store i32 %r311, ptr @ii
    br label %L104
L106:  ;
    %r312 = load i32, ptr @i
    %r313 = load i32, ptr @ii
    %r314 = add i32 %r312,%r313
    %r315 = add i32 1,0
    %r316 = sub i32 %r314,%r315
    store i32 %r316, ptr @i
    br label %L103
}
