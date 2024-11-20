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
@k = global i32 zeroinitializer
define float @params_f40(float %r0,float %r1,float %r2,float %r3,float %r4,float %r5,float %r6,float %r7,float %r8,float %r9,float %r10,float %r11,float %r12,float %r13,float %r14,float %r15,float %r16,float %r17,float %r18,float %r19,float %r20,float %r21,float %r22,float %r23,float %r24,float %r25,float %r26,float %r27,float %r28,float %r29,float %r30,float %r31,float %r32,float %r33,float %r34,float %r35,float %r36,float %r37,float %r38,float %r39)
{
L0:  ;
    %r82 = alloca [10 x float]
    %r79 = alloca float
    %r78 = alloca float
    %r77 = alloca float
    %r76 = alloca float
    %r75 = alloca float
    %r74 = alloca float
    %r73 = alloca float
    %r72 = alloca float
    %r71 = alloca float
    %r70 = alloca float
    %r69 = alloca float
    %r68 = alloca float
    %r67 = alloca float
    %r66 = alloca float
    %r65 = alloca float
    %r64 = alloca float
    %r63 = alloca float
    %r62 = alloca float
    %r61 = alloca float
    %r60 = alloca float
    %r59 = alloca float
    %r58 = alloca float
    %r57 = alloca float
    %r56 = alloca float
    %r55 = alloca float
    %r54 = alloca float
    %r53 = alloca float
    %r52 = alloca float
    %r51 = alloca float
    %r50 = alloca float
    %r49 = alloca float
    %r48 = alloca float
    %r47 = alloca float
    %r46 = alloca float
    %r45 = alloca float
    %r44 = alloca float
    %r43 = alloca float
    %r42 = alloca float
    %r41 = alloca float
    %r40 = alloca float
    store float %r0, ptr %r40
    store float %r1, ptr %r41
    store float %r2, ptr %r42
    store float %r3, ptr %r43
    store float %r4, ptr %r44
    store float %r5, ptr %r45
    store float %r6, ptr %r46
    store float %r7, ptr %r47
    store float %r8, ptr %r48
    store float %r9, ptr %r49
    store float %r10, ptr %r50
    store float %r11, ptr %r51
    store float %r12, ptr %r52
    store float %r13, ptr %r53
    store float %r14, ptr %r54
    store float %r15, ptr %r55
    store float %r16, ptr %r56
    store float %r17, ptr %r57
    store float %r18, ptr %r58
    store float %r19, ptr %r59
    store float %r20, ptr %r60
    store float %r21, ptr %r61
    store float %r22, ptr %r62
    store float %r23, ptr %r63
    store float %r24, ptr %r64
    store float %r25, ptr %r65
    store float %r26, ptr %r66
    store float %r27, ptr %r67
    store float %r28, ptr %r68
    store float %r29, ptr %r69
    store float %r30, ptr %r70
    store float %r31, ptr %r71
    store float %r32, ptr %r72
    store float %r33, ptr %r73
    store float %r34, ptr %r74
    store float %r35, ptr %r75
    store float %r36, ptr %r76
    store float %r37, ptr %r77
    store float %r38, ptr %r78
    store float %r39, ptr %r79
    br label %L1
L1:  ;
    %r80 = load float, ptr %r79
    %r81 = fcmp one float %r80,0x0
    br i1 %r81, label %L2, label %L3
L2:  ;
    %r83 = getelementptr [10 x float], ptr %r82, i32 0, i32 0
    %r84 = fadd float 0x0,0x0
    store float %r84, ptr %r83
    %r85 = getelementptr [10 x float], ptr %r82, i32 0, i32 1
    %r86 = fadd float 0x0,0x0
    store float %r86, ptr %r85
    %r87 = getelementptr [10 x float], ptr %r82, i32 0, i32 2
    %r88 = fadd float 0x0,0x0
    store float %r88, ptr %r87
    %r89 = getelementptr [10 x float], ptr %r82, i32 0, i32 3
    %r90 = fadd float 0x0,0x0
    store float %r90, ptr %r89
    %r91 = getelementptr [10 x float], ptr %r82, i32 0, i32 4
    %r92 = fadd float 0x0,0x0
    store float %r92, ptr %r91
    %r93 = getelementptr [10 x float], ptr %r82, i32 0, i32 5
    %r94 = fadd float 0x0,0x0
    store float %r94, ptr %r93
    %r95 = getelementptr [10 x float], ptr %r82, i32 0, i32 6
    %r96 = fadd float 0x0,0x0
    store float %r96, ptr %r95
    %r97 = getelementptr [10 x float], ptr %r82, i32 0, i32 7
    %r98 = fadd float 0x0,0x0
    store float %r98, ptr %r97
    %r99 = getelementptr [10 x float], ptr %r82, i32 0, i32 8
    %r100 = fadd float 0x0,0x0
    store float %r100, ptr %r99
    %r101 = getelementptr [10 x float], ptr %r82, i32 0, i32 9
    %r102 = fadd float 0x0,0x0
    store float %r102, ptr %r101
    %r103 = add i32 10,0
    %r104 = getelementptr [10 x float], ptr %r82, i32 0
    call void @putfarray(i32 %r103,ptr %r104)
    %r105 = load i32, ptr @k
    %r106 = getelementptr [10 x float], ptr %r82, i32 0, i32 %r105
    %r107 = load float, ptr %r106
    ret float %r107
    br label %L4
L3:  ;
    %r108 = load float, ptr %r41
    %r109 = load float, ptr %r42
    %r110 = load float, ptr %r43
    %r111 = load float, ptr %r44
    %r112 = load float, ptr %r45
    %r113 = load float, ptr %r46
    %r114 = load float, ptr %r47
    %r115 = load float, ptr %r48
    %r116 = load float, ptr %r49
    %r117 = load float, ptr %r50
    %r118 = load float, ptr %r51
    %r119 = load float, ptr %r52
    %r120 = load float, ptr %r53
    %r121 = load float, ptr %r54
    %r122 = load float, ptr %r55
    %r123 = load float, ptr %r56
    %r124 = load float, ptr %r57
    %r125 = load float, ptr %r58
    %r126 = load float, ptr %r59
    %r127 = load float, ptr %r60
    %r128 = load float, ptr %r61
    %r129 = load float, ptr %r62
    %r130 = load float, ptr %r63
    %r131 = load float, ptr %r64
    %r132 = load float, ptr %r65
    %r133 = load float, ptr %r66
    %r134 = load float, ptr %r67
    %r135 = load float, ptr %r68
    %r136 = load float, ptr %r69
    %r137 = load float, ptr %r70
    %r138 = load float, ptr %r71
    %r139 = load float, ptr %r72
    %r140 = load float, ptr %r73
    %r141 = load float, ptr %r74
    %r142 = load float, ptr %r75
    %r143 = load float, ptr %r76
    %r144 = load float, ptr %r77
    %r145 = load float, ptr %r78
    %r146 = load float, ptr %r79
    %r147 = load float, ptr %r40
    %r148 = load float, ptr %r41
    %r149 = fadd float %r147,%r148
    %r150 = load float, ptr %r42
    %r151 = fadd float %r149,%r150
    %r152 = call float @params_f40(float %r108,float %r109,float %r110,float %r111,float %r112,float %r113,float %r114,float %r115,float %r116,float %r117,float %r118,float %r119,float %r120,float %r121,float %r122,float %r123,float %r124,float %r125,float %r126,float %r127,float %r128,float %r129,float %r130,float %r131,float %r132,float %r133,float %r134,float %r135,float %r136,float %r137,float %r138,float %r139,float %r140,float %r141,float %r142,float %r143,float %r144,float %r145,float %r146,float %r151)
    ret float %r152
    br label %L4
L4:  ;
    ret float 0x0
}
define i32 @params_mix(float %r0,ptr %r1,i32 %r2,ptr %r3,float %r4,i32 %r5,float %r6,float %r7,ptr %r8,ptr %r9,i32 %r10,i32 %r11,ptr %r12,ptr %r13,ptr %r14,i32 %r15,ptr %r16,ptr %r17,float %r18,float %r19,float %r20,ptr %r21,i32 %r22,float %r23,float %r24,float %r25,ptr %r26,ptr %r27,ptr %r28,ptr %r29,ptr %r30,float %r31,float %r32,ptr %r33,i32 %r34,ptr %r35,ptr %r36,float %r37,float %r38,ptr %r39,ptr %r40,i32 %r41,i32 %r42,float %r43,float %r44,ptr %r45,i32 %r46,ptr %r47,i32 %r48,ptr %r49,ptr %r50,float %r51,float %r52,ptr %r53,i32 %r54,ptr %r55,ptr %r56,float %r57,i32 %r58,float %r59,ptr %r60,ptr %r61,float %r62,i32 %r63)
{
L0:  ;
    %r147 = alloca i32
    %r120 = alloca [10 x i32]
    %r99 = alloca [10 x float]
    %r98 = alloca i32
    %r97 = alloca float
    %r96 = alloca float
    %r95 = alloca i32
    %r94 = alloca float
    %r93 = alloca i32
    %r92 = alloca float
    %r91 = alloca float
    %r90 = alloca i32
    %r89 = alloca i32
    %r88 = alloca float
    %r87 = alloca float
    %r86 = alloca i32
    %r85 = alloca i32
    %r84 = alloca float
    %r83 = alloca float
    %r82 = alloca i32
    %r81 = alloca float
    %r80 = alloca float
    %r79 = alloca float
    %r78 = alloca float
    %r77 = alloca float
    %r76 = alloca i32
    %r75 = alloca float
    %r74 = alloca float
    %r73 = alloca float
    %r72 = alloca i32
    %r71 = alloca i32
    %r70 = alloca i32
    %r69 = alloca float
    %r68 = alloca float
    %r67 = alloca i32
    %r66 = alloca float
    %r65 = alloca i32
    %r64 = alloca float
    store float %r0, ptr %r64
    store i32 %r2, ptr %r65
    store float %r4, ptr %r66
    store i32 %r5, ptr %r67
    store float %r6, ptr %r68
    store float %r7, ptr %r69
    store i32 %r10, ptr %r70
    store i32 %r11, ptr %r71
    store i32 %r15, ptr %r72
    store float %r18, ptr %r73
    store float %r19, ptr %r74
    store float %r20, ptr %r75
    store i32 %r22, ptr %r76
    store float %r23, ptr %r77
    store float %r24, ptr %r78
    store float %r25, ptr %r79
    store float %r31, ptr %r80
    store float %r32, ptr %r81
    store i32 %r34, ptr %r82
    store float %r37, ptr %r83
    store float %r38, ptr %r84
    store i32 %r41, ptr %r85
    store i32 %r42, ptr %r86
    store float %r43, ptr %r87
    store float %r44, ptr %r88
    store i32 %r46, ptr %r89
    store i32 %r48, ptr %r90
    store float %r51, ptr %r91
    store float %r52, ptr %r92
    store i32 %r54, ptr %r93
    store float %r57, ptr %r94
    store i32 %r58, ptr %r95
    store float %r59, ptr %r96
    store float %r62, ptr %r97
    store i32 %r63, ptr %r98
    br label %L1
L1:  ;
    %r100 = getelementptr [10 x float], ptr %r99, i32 0, i32 0
    %r101 = fadd float 0x0,0x0
    store float %r101, ptr %r100
    %r102 = getelementptr [10 x float], ptr %r99, i32 0, i32 1
    %r103 = fadd float 0x0,0x0
    store float %r103, ptr %r102
    %r104 = getelementptr [10 x float], ptr %r99, i32 0, i32 2
    %r105 = fadd float 0x0,0x0
    store float %r105, ptr %r104
    %r106 = getelementptr [10 x float], ptr %r99, i32 0, i32 3
    %r107 = fadd float 0x0,0x0
    store float %r107, ptr %r106
    %r108 = getelementptr [10 x float], ptr %r99, i32 0, i32 4
    %r109 = fadd float 0x0,0x0
    store float %r109, ptr %r108
    %r110 = getelementptr [10 x float], ptr %r99, i32 0, i32 5
    %r111 = fadd float 0x0,0x0
    store float %r111, ptr %r110
    %r112 = getelementptr [10 x float], ptr %r99, i32 0, i32 6
    %r113 = fadd float 0x0,0x0
    store float %r113, ptr %r112
    %r114 = getelementptr [10 x float], ptr %r99, i32 0, i32 7
    %r115 = fadd float 0x0,0x0
    store float %r115, ptr %r114
    %r116 = getelementptr [10 x float], ptr %r99, i32 0, i32 8
    %r117 = fadd float 0x0,0x0
    store float %r117, ptr %r116
    %r118 = getelementptr [10 x float], ptr %r99, i32 0, i32 9
    %r119 = fadd float 0x0,0x0
    store float %r119, ptr %r118
    %r121 = getelementptr [10 x i32], ptr %r120, i32 0, i32 0
    %r122 = add i32 0,0
    store i32 %r122, ptr %r121
    %r123 = getelementptr [10 x i32], ptr %r120, i32 0, i32 1
    %r124 = add i32 0,0
    store i32 %r124, ptr %r123
    %r125 = getelementptr [10 x i32], ptr %r120, i32 0, i32 2
    %r126 = add i32 0,0
    store i32 %r126, ptr %r125
    %r127 = getelementptr [10 x i32], ptr %r120, i32 0, i32 3
    %r128 = add i32 0,0
    store i32 %r128, ptr %r127
    %r129 = getelementptr [10 x i32], ptr %r120, i32 0, i32 4
    %r130 = add i32 0,0
    store i32 %r130, ptr %r129
    %r131 = getelementptr [10 x i32], ptr %r120, i32 0, i32 5
    %r132 = add i32 0,0
    store i32 %r132, ptr %r131
    %r133 = getelementptr [10 x i32], ptr %r120, i32 0, i32 6
    %r134 = add i32 0,0
    store i32 %r134, ptr %r133
    %r135 = getelementptr [10 x i32], ptr %r120, i32 0, i32 7
    %r136 = add i32 0,0
    store i32 %r136, ptr %r135
    %r137 = getelementptr [10 x i32], ptr %r120, i32 0, i32 8
    %r138 = add i32 0,0
    store i32 %r138, ptr %r137
    %r139 = getelementptr [10 x i32], ptr %r120, i32 0, i32 9
    %r140 = add i32 0,0
    store i32 %r140, ptr %r139
    %r141 = load i32, ptr %r98
    %r142 = icmp ne i32 %r141,0
    br i1 %r142, label %L2, label %L3
L2:  ;
    %r143 = add i32 10,0
    %r144 = getelementptr [10 x float], ptr %r99, i32 0
    call void @putfarray(i32 %r143,ptr %r144)
    %r145 = add i32 10,0
    %r146 = getelementptr [10 x i32], ptr %r120, i32 0
    call void @putarray(i32 %r145,ptr %r146)
    %r148 = add i32 0,0
    store i32 %r148, ptr %r147
    br label %L5
L3:  ;
    %r174 = load float, ptr %r64
    %r175 = getelementptr [10 x i32], ptr %r120, i32 0
    %r176 = load i32, ptr %r65
    %r177 = getelementptr [10 x float], ptr %r99, i32 0
    %r178 = load float, ptr %r66
    %r179 = load i32, ptr %r67
    %r180 = load float, ptr %r68
    %r181 = load float, ptr %r69
    %r182 = getelementptr float, ptr %r8
    %r183 = getelementptr i32, ptr %r9
    %r184 = load i32, ptr %r70
    %r185 = load i32, ptr %r71
    %r186 = getelementptr float, ptr %r12
    %r187 = getelementptr i32, ptr %r13
    %r188 = getelementptr i32, ptr %r14
    %r189 = load i32, ptr %r72
    %r190 = getelementptr float, ptr %r16
    %r191 = getelementptr float, ptr %r17
    %r192 = load float, ptr %r73
    %r193 = load float, ptr %r74
    %r194 = load float, ptr %r75
    %r195 = getelementptr float, ptr %r21
    %r196 = load i32, ptr %r76
    %r197 = load float, ptr %r77
    %r198 = load float, ptr %r78
    %r199 = load float, ptr %r79
    %r200 = getelementptr i32, ptr %r26
    %r201 = getelementptr float, ptr %r27
    %r202 = getelementptr i32, ptr %r28
    %r203 = getelementptr i32, ptr %r29
    %r204 = getelementptr float, ptr %r30
    %r205 = load float, ptr %r80
    %r206 = load float, ptr %r81
    %r207 = getelementptr i32, ptr %r33
    %r208 = load i32, ptr %r82
    %r209 = getelementptr float, ptr %r35
    %r210 = getelementptr float, ptr %r36
    %r211 = load float, ptr %r83
    %r212 = load float, ptr %r84
    %r213 = getelementptr i32, ptr %r39
    %r214 = getelementptr i32, ptr %r40
    %r215 = load i32, ptr %r85
    %r216 = load i32, ptr %r86
    %r217 = load float, ptr %r87
    %r218 = load float, ptr %r88
    %r219 = getelementptr i32, ptr %r45
    %r220 = load i32, ptr %r89
    %r221 = getelementptr float, ptr %r47
    %r222 = load i32, ptr %r90
    %r223 = getelementptr i32, ptr %r49
    %r224 = getelementptr i32, ptr %r50
    %r225 = load float, ptr %r91
    %r226 = load float, ptr %r92
    %r227 = getelementptr float, ptr %r53
    %r228 = load i32, ptr %r93
    %r229 = getelementptr i32, ptr %r55
    %r230 = getelementptr float, ptr %r56
    %r231 = load float, ptr %r94
    %r232 = load i32, ptr %r95
    %r233 = load float, ptr %r96
    %r234 = getelementptr float, ptr %r60
    %r235 = getelementptr float, ptr %r61
    %r236 = load i32, ptr %r98
    %r237 = sitofp i32 %r236 to float
    %r238 = load float, ptr %r97
    %r239 = fptosi float %r238 to i32
    %r240 = call i32 @params_mix(float %r174,ptr %r175,i32 %r176,ptr %r177,float %r178,i32 %r179,float %r180,float %r181,ptr %r182,ptr %r183,i32 %r184,i32 %r185,ptr %r186,ptr %r187,ptr %r188,i32 %r189,ptr %r190,ptr %r191,float %r192,float %r193,float %r194,ptr %r195,i32 %r196,float %r197,float %r198,float %r199,ptr %r200,ptr %r201,ptr %r202,ptr %r203,ptr %r204,float %r205,float %r206,ptr %r207,i32 %r208,ptr %r209,ptr %r210,float %r211,float %r212,ptr %r213,ptr %r214,i32 %r215,i32 %r216,float %r217,float %r218,ptr %r219,i32 %r220,ptr %r221,i32 %r222,ptr %r223,ptr %r224,float %r225,float %r226,ptr %r227,i32 %r228,ptr %r229,ptr %r230,float %r231,i32 %r232,float %r233,ptr %r234,ptr %r235,float %r237,i32 %r239)
    ret i32 %r240
    br label %L4
L4:  ;
    ret i32 0
L5:  ;
    %r149 = load i32, ptr %r147
    %r150 = icmp slt i32 %r149,10
    br i1 %r150, label %L6, label %L7
L6:  ;
    %r151 = load i32, ptr %r147
    %r152 = getelementptr [10 x i32], ptr %r120, i32 0, i32 %r151
    %r153 = load i32, ptr %r147
    %r154 = getelementptr [10 x i32], ptr %r120, i32 0, i32 %r153
    %r155 = load i32, ptr %r154
    %r156 = sitofp i32 %r155 to float
    %r157 = load i32, ptr %r147
    %r158 = getelementptr [10 x float], ptr %r99, i32 0, i32 %r157
    %r159 = load float, ptr %r158
    %r160 = fsub float %r156,%r159
    %r161 = fptosi float %r160 to i32
    store i32 %r161, ptr %r152
    %r162 = load i32, ptr %r147
    %r163 = add i32 1,0
    %r164 = add i32 %r162,%r163
    store i32 %r164, ptr %r147
    br label %L5
L7:  ;
    %r165 = load i32, ptr @k
    %r166 = getelementptr [10 x i32], ptr %r120, i32 0, i32 %r165
    %r167 = load i32, ptr %r166
    %r168 = sitofp i32 %r167 to float
    %r169 = add i32 8,0
    %r170 = getelementptr [10 x float], ptr %r99, i32 0, i32 %r169
    %r171 = load float, ptr %r170
    %r172 = fmul float %r168,%r171
    %r173 = fptosi float %r172 to i32
    ret i32 %r173
    br label %L4
}
define float @params_f40_i24(i32 %r0,i32 %r1,i32 %r2,float %r3,i32 %r4,i32 %r5,i32 %r6,float %r7,float %r8,float %r9,i32 %r10,float %r11,float %r12,i32 %r13,float %r14,i32 %r15,float %r16,float %r17,float %r18,float %r19,float %r20,float %r21,i32 %r22,float %r23,i32 %r24,i32 %r25,float %r26,float %r27,float %r28,float %r29,float %r30,i32 %r31,float %r32,i32 %r33,float %r34,float %r35,float %r36,float %r37,i32 %r38,i32 %r39,float %r40,float %r41,float %r42,i32 %r43,float %r44,i32 %r45,i32 %r46,float %r47,float %r48,float %r49,float %r50,i32 %r51,i32 %r52,i32 %r53,float %r54,float %r55,float %r56,float %r57,float %r58,float %r59,i32 %r60,float %r61,i32 %r62,float %r63)
{
L0:  ;
    %r172 = alloca i32
    %r151 = alloca [8 x i32]
    %r130 = alloca [10 x float]
    %r127 = alloca float
    %r126 = alloca i32
    %r125 = alloca float
    %r124 = alloca i32
    %r123 = alloca float
    %r122 = alloca float
    %r121 = alloca float
    %r120 = alloca float
    %r119 = alloca float
    %r118 = alloca float
    %r117 = alloca i32
    %r116 = alloca i32
    %r115 = alloca i32
    %r114 = alloca float
    %r113 = alloca float
    %r112 = alloca float
    %r111 = alloca float
    %r110 = alloca i32
    %r109 = alloca i32
    %r108 = alloca float
    %r107 = alloca i32
    %r106 = alloca float
    %r105 = alloca float
    %r104 = alloca float
    %r103 = alloca i32
    %r102 = alloca i32
    %r101 = alloca float
    %r100 = alloca float
    %r99 = alloca float
    %r98 = alloca float
    %r97 = alloca i32
    %r96 = alloca float
    %r95 = alloca i32
    %r94 = alloca float
    %r93 = alloca float
    %r92 = alloca float
    %r91 = alloca float
    %r90 = alloca float
    %r89 = alloca i32
    %r88 = alloca i32
    %r87 = alloca float
    %r86 = alloca i32
    %r85 = alloca float
    %r84 = alloca float
    %r83 = alloca float
    %r82 = alloca float
    %r81 = alloca float
    %r80 = alloca float
    %r79 = alloca i32
    %r78 = alloca float
    %r77 = alloca i32
    %r76 = alloca float
    %r75 = alloca float
    %r74 = alloca i32
    %r73 = alloca float
    %r72 = alloca float
    %r71 = alloca float
    %r70 = alloca i32
    %r69 = alloca i32
    %r68 = alloca i32
    %r67 = alloca float
    %r66 = alloca i32
    %r65 = alloca i32
    %r64 = alloca i32
    store i32 %r0, ptr %r64
    store i32 %r1, ptr %r65
    store i32 %r2, ptr %r66
    store float %r3, ptr %r67
    store i32 %r4, ptr %r68
    store i32 %r5, ptr %r69
    store i32 %r6, ptr %r70
    store float %r7, ptr %r71
    store float %r8, ptr %r72
    store float %r9, ptr %r73
    store i32 %r10, ptr %r74
    store float %r11, ptr %r75
    store float %r12, ptr %r76
    store i32 %r13, ptr %r77
    store float %r14, ptr %r78
    store i32 %r15, ptr %r79
    store float %r16, ptr %r80
    store float %r17, ptr %r81
    store float %r18, ptr %r82
    store float %r19, ptr %r83
    store float %r20, ptr %r84
    store float %r21, ptr %r85
    store i32 %r22, ptr %r86
    store float %r23, ptr %r87
    store i32 %r24, ptr %r88
    store i32 %r25, ptr %r89
    store float %r26, ptr %r90
    store float %r27, ptr %r91
    store float %r28, ptr %r92
    store float %r29, ptr %r93
    store float %r30, ptr %r94
    store i32 %r31, ptr %r95
    store float %r32, ptr %r96
    store i32 %r33, ptr %r97
    store float %r34, ptr %r98
    store float %r35, ptr %r99
    store float %r36, ptr %r100
    store float %r37, ptr %r101
    store i32 %r38, ptr %r102
    store i32 %r39, ptr %r103
    store float %r40, ptr %r104
    store float %r41, ptr %r105
    store float %r42, ptr %r106
    store i32 %r43, ptr %r107
    store float %r44, ptr %r108
    store i32 %r45, ptr %r109
    store i32 %r46, ptr %r110
    store float %r47, ptr %r111
    store float %r48, ptr %r112
    store float %r49, ptr %r113
    store float %r50, ptr %r114
    store i32 %r51, ptr %r115
    store i32 %r52, ptr %r116
    store i32 %r53, ptr %r117
    store float %r54, ptr %r118
    store float %r55, ptr %r119
    store float %r56, ptr %r120
    store float %r57, ptr %r121
    store float %r58, ptr %r122
    store float %r59, ptr %r123
    store i32 %r60, ptr %r124
    store float %r61, ptr %r125
    store i32 %r62, ptr %r126
    store float %r63, ptr %r127
    br label %L1
L1:  ;
    %r128 = load i32, ptr %r64
    %r129 = icmp ne i32 %r128,0
    br i1 %r129, label %L2, label %L3
L2:  ;
    %r131 = getelementptr [10 x float], ptr %r130, i32 0, i32 0
    %r132 = fadd float 0x0,0x0
    store float %r132, ptr %r131
    %r133 = getelementptr [10 x float], ptr %r130, i32 0, i32 1
    %r134 = fadd float 0x0,0x0
    store float %r134, ptr %r133
    %r135 = getelementptr [10 x float], ptr %r130, i32 0, i32 2
    %r136 = fadd float 0x0,0x0
    store float %r136, ptr %r135
    %r137 = getelementptr [10 x float], ptr %r130, i32 0, i32 3
    %r138 = fadd float 0x0,0x0
    store float %r138, ptr %r137
    %r139 = getelementptr [10 x float], ptr %r130, i32 0, i32 4
    %r140 = fadd float 0x0,0x0
    store float %r140, ptr %r139
    %r141 = getelementptr [10 x float], ptr %r130, i32 0, i32 5
    %r142 = fadd float 0x0,0x0
    store float %r142, ptr %r141
    %r143 = getelementptr [10 x float], ptr %r130, i32 0, i32 6
    %r144 = fadd float 0x0,0x0
    store float %r144, ptr %r143
    %r145 = getelementptr [10 x float], ptr %r130, i32 0, i32 7
    %r146 = fadd float 0x0,0x0
    store float %r146, ptr %r145
    %r147 = getelementptr [10 x float], ptr %r130, i32 0, i32 8
    %r148 = fadd float 0x0,0x0
    store float %r148, ptr %r147
    %r149 = getelementptr [10 x float], ptr %r130, i32 0, i32 9
    %r150 = fadd float 0x0,0x0
    store float %r150, ptr %r149
    %r152 = getelementptr [8 x i32], ptr %r151, i32 0, i32 0
    %r153 = add i32 0,0
    store i32 %r153, ptr %r152
    %r154 = getelementptr [8 x i32], ptr %r151, i32 0, i32 1
    %r155 = add i32 0,0
    store i32 %r155, ptr %r154
    %r156 = getelementptr [8 x i32], ptr %r151, i32 0, i32 2
    %r157 = add i32 0,0
    store i32 %r157, ptr %r156
    %r158 = getelementptr [8 x i32], ptr %r151, i32 0, i32 3
    %r159 = add i32 0,0
    store i32 %r159, ptr %r158
    %r160 = getelementptr [8 x i32], ptr %r151, i32 0, i32 4
    %r161 = add i32 0,0
    store i32 %r161, ptr %r160
    %r162 = getelementptr [8 x i32], ptr %r151, i32 0, i32 5
    %r163 = add i32 0,0
    store i32 %r163, ptr %r162
    %r164 = getelementptr [8 x i32], ptr %r151, i32 0, i32 6
    %r165 = add i32 0,0
    store i32 %r165, ptr %r164
    %r166 = getelementptr [8 x i32], ptr %r151, i32 0, i32 7
    %r167 = add i32 0,0
    store i32 %r167, ptr %r166
    %r168 = add i32 10,0
    %r169 = getelementptr [10 x float], ptr %r130, i32 0
    call void @putfarray(i32 %r168,ptr %r169)
    %r170 = add i32 8,0
    %r171 = getelementptr [8 x i32], ptr %r151, i32 0
    call void @putarray(i32 %r170,ptr %r171)
    %r173 = add i32 0,0
    store i32 %r173, ptr %r172
    br label %L5
L3:  ;
    %r194 = load i32, ptr %r68
    %r195 = load i32, ptr %r65
    %r196 = load i32, ptr %r66
    %r197 = load float, ptr %r67
    %r198 = load i32, ptr %r68
    %r199 = load i32, ptr %r69
    %r200 = load i32, ptr %r70
    %r201 = load float, ptr %r71
    %r202 = load float, ptr %r72
    %r203 = load float, ptr %r73
    %r204 = load i32, ptr %r74
    %r205 = load float, ptr %r75
    %r206 = load float, ptr %r76
    %r207 = load i32, ptr %r77
    %r208 = load float, ptr %r78
    %r209 = load i32, ptr %r79
    %r210 = load float, ptr %r80
    %r211 = load float, ptr %r81
    %r212 = load float, ptr %r82
    %r213 = load float, ptr %r83
    %r214 = load float, ptr %r84
    %r215 = load float, ptr %r85
    %r216 = load i32, ptr %r86
    %r217 = load float, ptr %r87
    %r218 = load i32, ptr %r88
    %r219 = load i32, ptr %r89
    %r220 = load float, ptr %r90
    %r221 = load float, ptr %r91
    %r222 = load float, ptr %r92
    %r223 = load float, ptr %r93
    %r224 = load float, ptr %r94
    %r225 = load i32, ptr %r95
    %r226 = load float, ptr %r96
    %r227 = load i32, ptr %r97
    %r228 = load float, ptr %r98
    %r229 = load float, ptr %r99
    %r230 = load float, ptr %r100
    %r231 = load float, ptr %r101
    %r232 = load i32, ptr %r102
    %r233 = load i32, ptr %r103
    %r234 = load float, ptr %r104
    %r235 = load float, ptr %r105
    %r236 = load float, ptr %r106
    %r237 = load i32, ptr %r107
    %r238 = load float, ptr %r108
    %r239 = load i32, ptr %r109
    %r240 = load i32, ptr %r110
    %r241 = load float, ptr %r111
    %r242 = load float, ptr %r112
    %r243 = load float, ptr %r113
    %r244 = load float, ptr %r114
    %r245 = load i32, ptr %r115
    %r246 = load i32, ptr %r116
    %r247 = load i32, ptr %r117
    %r248 = load float, ptr %r118
    %r249 = load float, ptr %r119
    %r250 = load float, ptr %r120
    %r251 = load float, ptr %r121
    %r252 = load float, ptr %r122
    %r253 = load float, ptr %r123
    %r254 = load i32, ptr %r124
    %r255 = load float, ptr %r125
    %r256 = load i32, ptr %r126
    %r257 = load float, ptr %r127
    %r258 = call float @params_f40_i24(i32 %r194,i32 %r195,i32 %r196,float %r197,i32 %r198,i32 %r199,i32 %r200,float %r201,float %r202,float %r203,i32 %r204,float %r205,float %r206,i32 %r207,float %r208,i32 %r209,float %r210,float %r211,float %r212,float %r213,float %r214,float %r215,i32 %r216,float %r217,i32 %r218,i32 %r219,float %r220,float %r221,float %r222,float %r223,float %r224,i32 %r225,float %r226,i32 %r227,float %r228,float %r229,float %r230,float %r231,i32 %r232,i32 %r233,float %r234,float %r235,float %r236,i32 %r237,float %r238,i32 %r239,i32 %r240,float %r241,float %r242,float %r243,float %r244,i32 %r245,i32 %r246,i32 %r247,float %r248,float %r249,float %r250,float %r251,float %r252,float %r253,i32 %r254,float %r255,i32 %r256,float %r257)
    ret float %r258
    br label %L4
L4:  ;
    ret float 0x0
L5:  ;
    %r174 = load i32, ptr %r172
    %r175 = icmp slt i32 %r174,8
    br i1 %r175, label %L6, label %L7
L6:  ;
    %r176 = load i32, ptr %r172
    %r177 = getelementptr [8 x i32], ptr %r151, i32 0, i32 %r176
    %r178 = load i32, ptr %r172
    %r179 = getelementptr [8 x i32], ptr %r151, i32 0, i32 %r178
    %r180 = load i32, ptr %r179
    %r181 = sitofp i32 %r180 to float
    %r182 = load i32, ptr %r172
    %r183 = getelementptr [10 x float], ptr %r130, i32 0, i32 %r182
    %r184 = load float, ptr %r183
    %r185 = fsub float %r181,%r184
    %r186 = fptosi float %r185 to i32
    store i32 %r186, ptr %r177
    %r187 = load i32, ptr %r172
    %r188 = add i32 1,0
    %r189 = add i32 %r187,%r188
    store i32 %r189, ptr %r172
    br label %L5
L7:  ;
    %r190 = load i32, ptr @k
    %r191 = getelementptr [8 x i32], ptr %r151, i32 0, i32 %r190
    %r192 = load i32, ptr %r191
    %r193 = sitofp i32 %r192 to float
    ret float %r193
    br label %L4
}
define float @params_fa40(ptr %r0,ptr %r1,ptr %r2,ptr %r3,ptr %r4,ptr %r5,ptr %r6,ptr %r7,ptr %r8,ptr %r9,ptr %r10,ptr %r11,ptr %r12,ptr %r13,ptr %r14,ptr %r15,ptr %r16,ptr %r17,ptr %r18,ptr %r19,ptr %r20,ptr %r21,ptr %r22,ptr %r23,ptr %r24,ptr %r25,ptr %r26,ptr %r27,ptr %r28,ptr %r29,ptr %r30,ptr %r31,ptr %r32,ptr %r33,ptr %r34,ptr %r35,ptr %r36,ptr %r37,ptr %r38,ptr %r39)
{
L0:  ;
    %r40 = alloca [10 x float]
    br label %L1
L1:  ;
    %r41 = getelementptr [10 x float], ptr %r40, i32 0, i32 0
    %r42 = fadd float 0x0,0x0
    store float %r42, ptr %r41
    %r43 = getelementptr [10 x float], ptr %r40, i32 0, i32 1
    %r44 = fadd float 0x0,0x0
    store float %r44, ptr %r43
    %r45 = getelementptr [10 x float], ptr %r40, i32 0, i32 2
    %r46 = fadd float 0x0,0x0
    store float %r46, ptr %r45
    %r47 = getelementptr [10 x float], ptr %r40, i32 0, i32 3
    %r48 = fadd float 0x0,0x0
    store float %r48, ptr %r47
    %r49 = getelementptr [10 x float], ptr %r40, i32 0, i32 4
    %r50 = fadd float 0x0,0x0
    store float %r50, ptr %r49
    %r51 = getelementptr [10 x float], ptr %r40, i32 0, i32 5
    %r52 = fadd float 0x0,0x0
    store float %r52, ptr %r51
    %r53 = getelementptr [10 x float], ptr %r40, i32 0, i32 6
    %r54 = fadd float 0x0,0x0
    store float %r54, ptr %r53
    %r55 = getelementptr [10 x float], ptr %r40, i32 0, i32 7
    %r56 = fadd float 0x0,0x0
    store float %r56, ptr %r55
    %r57 = getelementptr [10 x float], ptr %r40, i32 0, i32 8
    %r58 = fadd float 0x0,0x0
    store float %r58, ptr %r57
    %r59 = getelementptr [10 x float], ptr %r40, i32 0, i32 9
    %r60 = fadd float 0x0,0x0
    store float %r60, ptr %r59
    %r61 = load i32, ptr @k
    %r62 = getelementptr float, ptr %r39, i32 %r61
    %r63 = load float, ptr %r62
    %r64 = fcmp one float %r63,0x0
    %r65 = zext i1 %r64 to i32
    %r66 = icmp ne i32 %r65,0
    %r67 = zext i1 %r66 to i32
    %r68 = sitofp i32 %r67 to float
    %r69 = fcmp one float %r68,0x0
    %r70 = zext i1 %r69 to i32
    %r71 = sitofp i32 %r70 to float
    %r72 = fcmp one float %r71,0x0
    br i1 %r72, label %L2, label %L3
L2:  ;
    %r73 = add i32 10,0
    %r74 = getelementptr [10 x float], ptr %r40, i32 0
    call void @putfarray(i32 %r73,ptr %r74)
    %r75 = load i32, ptr @k
    %r76 = getelementptr [10 x float], ptr %r40, i32 0, i32 %r75
    %r77 = load float, ptr %r76
    ret float %r77
    br label %L4
L3:  ;
    %r78 = getelementptr float, ptr %r1
    %r79 = getelementptr float, ptr %r2
    %r80 = getelementptr float, ptr %r3
    %r81 = getelementptr float, ptr %r4
    %r82 = getelementptr float, ptr %r5
    %r83 = getelementptr float, ptr %r6
    %r84 = getelementptr float, ptr %r7
    %r85 = getelementptr float, ptr %r8
    %r86 = getelementptr float, ptr %r9
    %r87 = getelementptr float, ptr %r10
    %r88 = getelementptr float, ptr %r11
    %r89 = getelementptr float, ptr %r12
    %r90 = getelementptr float, ptr %r13
    %r91 = getelementptr float, ptr %r14
    %r92 = getelementptr float, ptr %r15
    %r93 = getelementptr float, ptr %r16
    %r94 = getelementptr float, ptr %r17
    %r95 = getelementptr float, ptr %r18
    %r96 = getelementptr float, ptr %r19
    %r97 = getelementptr float, ptr %r20
    %r98 = getelementptr float, ptr %r21
    %r99 = getelementptr float, ptr %r22
    %r100 = getelementptr float, ptr %r23
    %r101 = getelementptr float, ptr %r24
    %r102 = getelementptr float, ptr %r25
    %r103 = getelementptr float, ptr %r26
    %r104 = getelementptr float, ptr %r27
    %r105 = getelementptr float, ptr %r28
    %r106 = getelementptr float, ptr %r29
    %r107 = getelementptr float, ptr %r30
    %r108 = getelementptr float, ptr %r31
    %r109 = getelementptr float, ptr %r32
    %r110 = getelementptr float, ptr %r33
    %r111 = getelementptr float, ptr %r34
    %r112 = getelementptr float, ptr %r35
    %r113 = getelementptr float, ptr %r36
    %r114 = getelementptr float, ptr %r37
    %r115 = getelementptr float, ptr %r38
    %r116 = getelementptr float, ptr %r39
    %r117 = getelementptr [10 x float], ptr %r40, i32 0
    %r118 = call float @params_fa40(ptr %r78,ptr %r79,ptr %r80,ptr %r81,ptr %r82,ptr %r83,ptr %r84,ptr %r85,ptr %r86,ptr %r87,ptr %r88,ptr %r89,ptr %r90,ptr %r91,ptr %r92,ptr %r93,ptr %r94,ptr %r95,ptr %r96,ptr %r97,ptr %r98,ptr %r99,ptr %r100,ptr %r101,ptr %r102,ptr %r103,ptr %r104,ptr %r105,ptr %r106,ptr %r107,ptr %r108,ptr %r109,ptr %r110,ptr %r111,ptr %r112,ptr %r113,ptr %r114,ptr %r115,ptr %r116,ptr %r117)
    ret float %r118
    br label %L4
L4:  ;
    ret float 0x0
}
define i32 @main()
{
L0:  ;
    %r525 = alloca i32
    %r443 = alloca float
    %r185 = alloca float
    %r23 = alloca float
    %r2 = alloca i32
    %r1 = alloca [24 x [3 x i32]]
    %r0 = alloca [40 x [3 x float]]
    br label %L1
L1:  ;
    call void @llvm.memset.p0.i32(ptr %r0,i8 63,i32 480,i1 0)
    call void @llvm.memset.p0.i32(ptr %r1,i8 63,i32 288,i1 0)
    %r3 = add i32 0,0
    store i32 %r3, ptr %r2
    %r4 = call i32 @getint()
    store i32 %r4, ptr @k
    %r5 = add i32 0,0
    store i32 %r5, ptr %r2
    br label %L2
L2:  ;
    %r6 = load i32, ptr %r2
    %r7 = icmp slt i32 %r6,40
    br i1 %r7, label %L3, label %L4
L3:  ;
    %r8 = load i32, ptr %r2
    %r9 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r8
    %r10 = call i32 @getfarray(ptr %r9)
    %r11 = load i32, ptr %r2
    %r12 = add i32 1,0
    %r13 = add i32 %r11,%r12
    store i32 %r13, ptr %r2
    br label %L2
L4:  ;
    %r14 = add i32 0,0
    store i32 %r14, ptr %r2
    br label %L5
L5:  ;
    %r15 = load i32, ptr %r2
    %r16 = icmp slt i32 %r15,24
    br i1 %r16, label %L6, label %L7
L6:  ;
    %r17 = load i32, ptr %r2
    %r18 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r17
    %r19 = call i32 @getarray(ptr %r18)
    %r20 = load i32, ptr %r2
    %r21 = add i32 1,0
    %r22 = add i32 %r20,%r21
    store i32 %r22, ptr %r2
    br label %L5
L7:  ;
    %r24 = add i32 0,0
    %r25 = load i32, ptr @k
    %r26 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r24, i32 %r25
    %r27 = load float, ptr %r26
    %r28 = add i32 1,0
    %r29 = load i32, ptr @k
    %r30 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r28, i32 %r29
    %r31 = load float, ptr %r30
    %r32 = add i32 2,0
    %r33 = load i32, ptr @k
    %r34 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r32, i32 %r33
    %r35 = load float, ptr %r34
    %r36 = add i32 3,0
    %r37 = load i32, ptr @k
    %r38 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r36, i32 %r37
    %r39 = load float, ptr %r38
    %r40 = add i32 4,0
    %r41 = load i32, ptr @k
    %r42 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r40, i32 %r41
    %r43 = load float, ptr %r42
    %r44 = add i32 5,0
    %r45 = load i32, ptr @k
    %r46 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r44, i32 %r45
    %r47 = load float, ptr %r46
    %r48 = add i32 6,0
    %r49 = load i32, ptr @k
    %r50 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r48, i32 %r49
    %r51 = load float, ptr %r50
    %r52 = add i32 7,0
    %r53 = load i32, ptr @k
    %r54 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r52, i32 %r53
    %r55 = load float, ptr %r54
    %r56 = add i32 8,0
    %r57 = load i32, ptr @k
    %r58 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r56, i32 %r57
    %r59 = load float, ptr %r58
    %r60 = add i32 9,0
    %r61 = load i32, ptr @k
    %r62 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r60, i32 %r61
    %r63 = load float, ptr %r62
    %r64 = add i32 10,0
    %r65 = load i32, ptr @k
    %r66 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r64, i32 %r65
    %r67 = load float, ptr %r66
    %r68 = add i32 11,0
    %r69 = load i32, ptr @k
    %r70 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r68, i32 %r69
    %r71 = load float, ptr %r70
    %r72 = add i32 12,0
    %r73 = load i32, ptr @k
    %r74 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r72, i32 %r73
    %r75 = load float, ptr %r74
    %r76 = add i32 13,0
    %r77 = load i32, ptr @k
    %r78 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r76, i32 %r77
    %r79 = load float, ptr %r78
    %r80 = add i32 14,0
    %r81 = load i32, ptr @k
    %r82 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r80, i32 %r81
    %r83 = load float, ptr %r82
    %r84 = add i32 15,0
    %r85 = load i32, ptr @k
    %r86 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r84, i32 %r85
    %r87 = load float, ptr %r86
    %r88 = add i32 16,0
    %r89 = load i32, ptr @k
    %r90 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r88, i32 %r89
    %r91 = load float, ptr %r90
    %r92 = add i32 17,0
    %r93 = load i32, ptr @k
    %r94 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r92, i32 %r93
    %r95 = load float, ptr %r94
    %r96 = add i32 18,0
    %r97 = load i32, ptr @k
    %r98 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r96, i32 %r97
    %r99 = load float, ptr %r98
    %r100 = add i32 19,0
    %r101 = load i32, ptr @k
    %r102 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r100, i32 %r101
    %r103 = load float, ptr %r102
    %r104 = add i32 20,0
    %r105 = load i32, ptr @k
    %r106 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r104, i32 %r105
    %r107 = load float, ptr %r106
    %r108 = add i32 21,0
    %r109 = load i32, ptr @k
    %r110 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r108, i32 %r109
    %r111 = load float, ptr %r110
    %r112 = add i32 22,0
    %r113 = load i32, ptr @k
    %r114 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r112, i32 %r113
    %r115 = load float, ptr %r114
    %r116 = add i32 23,0
    %r117 = load i32, ptr @k
    %r118 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r116, i32 %r117
    %r119 = load float, ptr %r118
    %r120 = add i32 24,0
    %r121 = load i32, ptr @k
    %r122 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r120, i32 %r121
    %r123 = load float, ptr %r122
    %r124 = add i32 25,0
    %r125 = load i32, ptr @k
    %r126 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r124, i32 %r125
    %r127 = load float, ptr %r126
    %r128 = add i32 26,0
    %r129 = load i32, ptr @k
    %r130 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r128, i32 %r129
    %r131 = load float, ptr %r130
    %r132 = add i32 27,0
    %r133 = load i32, ptr @k
    %r134 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r132, i32 %r133
    %r135 = load float, ptr %r134
    %r136 = add i32 28,0
    %r137 = load i32, ptr @k
    %r138 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r136, i32 %r137
    %r139 = load float, ptr %r138
    %r140 = add i32 29,0
    %r141 = load i32, ptr @k
    %r142 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r140, i32 %r141
    %r143 = load float, ptr %r142
    %r144 = add i32 30,0
    %r145 = load i32, ptr @k
    %r146 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r144, i32 %r145
    %r147 = load float, ptr %r146
    %r148 = add i32 31,0
    %r149 = load i32, ptr @k
    %r150 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r148, i32 %r149
    %r151 = load float, ptr %r150
    %r152 = add i32 32,0
    %r153 = load i32, ptr @k
    %r154 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r152, i32 %r153
    %r155 = load float, ptr %r154
    %r156 = add i32 33,0
    %r157 = load i32, ptr @k
    %r158 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r156, i32 %r157
    %r159 = load float, ptr %r158
    %r160 = add i32 34,0
    %r161 = load i32, ptr @k
    %r162 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r160, i32 %r161
    %r163 = load float, ptr %r162
    %r164 = add i32 35,0
    %r165 = load i32, ptr @k
    %r166 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r164, i32 %r165
    %r167 = load float, ptr %r166
    %r168 = add i32 36,0
    %r169 = load i32, ptr @k
    %r170 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r168, i32 %r169
    %r171 = load float, ptr %r170
    %r172 = add i32 37,0
    %r173 = load i32, ptr @k
    %r174 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r172, i32 %r173
    %r175 = load float, ptr %r174
    %r176 = add i32 38,0
    %r177 = load i32, ptr @k
    %r178 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r176, i32 %r177
    %r179 = load float, ptr %r178
    %r180 = add i32 39,0
    %r181 = load i32, ptr @k
    %r182 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r180, i32 %r181
    %r183 = load float, ptr %r182
    %r184 = call float @params_f40(float %r27,float %r31,float %r35,float %r39,float %r43,float %r47,float %r51,float %r55,float %r59,float %r63,float %r67,float %r71,float %r75,float %r79,float %r83,float %r87,float %r91,float %r95,float %r99,float %r103,float %r107,float %r111,float %r115,float %r119,float %r123,float %r127,float %r131,float %r135,float %r139,float %r143,float %r147,float %r151,float %r155,float %r159,float %r163,float %r167,float %r171,float %r175,float %r179,float %r183)
    store float %r184, ptr %r23
    %r186 = add i32 23,0
    %r187 = load i32, ptr @k
    %r188 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r186, i32 %r187
    %r189 = load i32, ptr %r188
    %r190 = add i32 2,0
    %r191 = load i32, ptr @k
    %r192 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r190, i32 %r191
    %r193 = load i32, ptr %r192
    %r194 = add i32 6,0
    %r195 = load i32, ptr @k
    %r196 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r194, i32 %r195
    %r197 = load i32, ptr %r196
    %r198 = add i32 4,0
    %r199 = load i32, ptr @k
    %r200 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r198, i32 %r199
    %r201 = load float, ptr %r200
    %r202 = add i32 1,0
    %r203 = load i32, ptr @k
    %r204 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r202, i32 %r203
    %r205 = load i32, ptr %r204
    %r206 = add i32 4,0
    %r207 = load i32, ptr @k
    %r208 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r206, i32 %r207
    %r209 = load i32, ptr %r208
    %r210 = add i32 5,0
    %r211 = load i32, ptr @k
    %r212 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r210, i32 %r211
    %r213 = load i32, ptr %r212
    %r214 = add i32 8,0
    %r215 = load i32, ptr @k
    %r216 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r214, i32 %r215
    %r217 = load float, ptr %r216
    %r218 = add i32 15,0
    %r219 = load i32, ptr @k
    %r220 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r218, i32 %r219
    %r221 = load float, ptr %r220
    %r222 = add i32 7,0
    %r223 = load i32, ptr @k
    %r224 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r222, i32 %r223
    %r225 = load float, ptr %r224
    %r226 = add i32 22,0
    %r227 = load i32, ptr @k
    %r228 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r226, i32 %r227
    %r229 = load i32, ptr %r228
    %r230 = add i32 3,0
    %r231 = load i32, ptr @k
    %r232 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r230, i32 %r231
    %r233 = load float, ptr %r232
    %r234 = add i32 28,0
    %r235 = load i32, ptr @k
    %r236 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r234, i32 %r235
    %r237 = load float, ptr %r236
    %r238 = add i32 0,0
    %r239 = load i32, ptr @k
    %r240 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r238, i32 %r239
    %r241 = load i32, ptr %r240
    %r242 = add i32 37,0
    %r243 = load i32, ptr @k
    %r244 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r242, i32 %r243
    %r245 = load float, ptr %r244
    %r246 = add i32 19,0
    %r247 = load i32, ptr @k
    %r248 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r246, i32 %r247
    %r249 = load i32, ptr %r248
    %r250 = add i32 30,0
    %r251 = load i32, ptr @k
    %r252 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r250, i32 %r251
    %r253 = load float, ptr %r252
    %r254 = add i32 12,0
    %r255 = load i32, ptr @k
    %r256 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r254, i32 %r255
    %r257 = load float, ptr %r256
    %r258 = add i32 1,0
    %r259 = load i32, ptr @k
    %r260 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r258, i32 %r259
    %r261 = load float, ptr %r260
    %r262 = add i32 11,0
    %r263 = load i32, ptr @k
    %r264 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r262, i32 %r263
    %r265 = load float, ptr %r264
    %r266 = add i32 38,0
    %r267 = load i32, ptr @k
    %r268 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r266, i32 %r267
    %r269 = load float, ptr %r268
    %r270 = add i32 6,0
    %r271 = load i32, ptr @k
    %r272 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r270, i32 %r271
    %r273 = load float, ptr %r272
    %r274 = add i32 7,0
    %r275 = load i32, ptr @k
    %r276 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r274, i32 %r275
    %r277 = load i32, ptr %r276
    %r278 = add i32 32,0
    %r279 = load i32, ptr @k
    %r280 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r278, i32 %r279
    %r281 = load float, ptr %r280
    %r282 = add i32 10,0
    %r283 = load i32, ptr @k
    %r284 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r282, i32 %r283
    %r285 = load i32, ptr %r284
    %r286 = add i32 13,0
    %r287 = load i32, ptr @k
    %r288 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r286, i32 %r287
    %r289 = load i32, ptr %r288
    %r290 = add i32 20,0
    %r291 = load i32, ptr @k
    %r292 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r290, i32 %r291
    %r293 = load float, ptr %r292
    %r294 = add i32 33,0
    %r295 = load i32, ptr @k
    %r296 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r294, i32 %r295
    %r297 = load float, ptr %r296
    %r298 = add i32 23,0
    %r299 = load i32, ptr @k
    %r300 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r298, i32 %r299
    %r301 = load float, ptr %r300
    %r302 = add i32 9,0
    %r303 = load i32, ptr @k
    %r304 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r302, i32 %r303
    %r305 = load float, ptr %r304
    %r306 = add i32 25,0
    %r307 = load i32, ptr @k
    %r308 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r306, i32 %r307
    %r309 = load float, ptr %r308
    %r310 = add i32 8,0
    %r311 = load i32, ptr @k
    %r312 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r310, i32 %r311
    %r313 = load i32, ptr %r312
    %r314 = add i32 39,0
    %r315 = load i32, ptr @k
    %r316 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r314, i32 %r315
    %r317 = load float, ptr %r316
    %r318 = add i32 17,0
    %r319 = load i32, ptr @k
    %r320 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r318, i32 %r319
    %r321 = load i32, ptr %r320
    %r322 = add i32 21,0
    %r323 = load i32, ptr @k
    %r324 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r322, i32 %r323
    %r325 = load float, ptr %r324
    %r326 = add i32 16,0
    %r327 = load i32, ptr @k
    %r328 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r326, i32 %r327
    %r329 = load float, ptr %r328
    %r330 = add i32 5,0
    %r331 = load i32, ptr @k
    %r332 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r330, i32 %r331
    %r333 = load float, ptr %r332
    %r334 = add i32 34,0
    %r335 = load i32, ptr @k
    %r336 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r334, i32 %r335
    %r337 = load float, ptr %r336
    %r338 = add i32 18,0
    %r339 = load i32, ptr @k
    %r340 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r338, i32 %r339
    %r341 = load i32, ptr %r340
    %r342 = add i32 9,0
    %r343 = load i32, ptr @k
    %r344 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r342, i32 %r343
    %r345 = load i32, ptr %r344
    %r346 = add i32 14,0
    %r347 = load i32, ptr @k
    %r348 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r346, i32 %r347
    %r349 = load float, ptr %r348
    %r350 = add i32 10,0
    %r351 = load i32, ptr @k
    %r352 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r350, i32 %r351
    %r353 = load float, ptr %r352
    %r354 = add i32 0,0
    %r355 = load i32, ptr @k
    %r356 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r354, i32 %r355
    %r357 = load float, ptr %r356
    %r358 = add i32 12,0
    %r359 = load i32, ptr @k
    %r360 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r358, i32 %r359
    %r361 = load i32, ptr %r360
    %r362 = add i32 31,0
    %r363 = load i32, ptr @k
    %r364 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r362, i32 %r363
    %r365 = load float, ptr %r364
    %r366 = add i32 11,0
    %r367 = load i32, ptr @k
    %r368 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r366, i32 %r367
    %r369 = load i32, ptr %r368
    %r370 = add i32 16,0
    %r371 = load i32, ptr @k
    %r372 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r370, i32 %r371
    %r373 = load i32, ptr %r372
    %r374 = add i32 27,0
    %r375 = load i32, ptr @k
    %r376 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r374, i32 %r375
    %r377 = load float, ptr %r376
    %r378 = add i32 24,0
    %r379 = load i32, ptr @k
    %r380 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r378, i32 %r379
    %r381 = load float, ptr %r380
    %r382 = add i32 13,0
    %r383 = load i32, ptr @k
    %r384 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r382, i32 %r383
    %r385 = load float, ptr %r384
    %r386 = add i32 29,0
    %r387 = load i32, ptr @k
    %r388 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r386, i32 %r387
    %r389 = load float, ptr %r388
    %r390 = add i32 3,0
    %r391 = load i32, ptr @k
    %r392 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r390, i32 %r391
    %r393 = load i32, ptr %r392
    %r394 = add i32 21,0
    %r395 = load i32, ptr @k
    %r396 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r394, i32 %r395
    %r397 = load i32, ptr %r396
    %r398 = add i32 20,0
    %r399 = load i32, ptr @k
    %r400 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r398, i32 %r399
    %r401 = load i32, ptr %r400
    %r402 = add i32 18,0
    %r403 = load i32, ptr @k
    %r404 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r402, i32 %r403
    %r405 = load float, ptr %r404
    %r406 = add i32 19,0
    %r407 = load i32, ptr @k
    %r408 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r406, i32 %r407
    %r409 = load float, ptr %r408
    %r410 = add i32 22,0
    %r411 = load i32, ptr @k
    %r412 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r410, i32 %r411
    %r413 = load float, ptr %r412
    %r414 = add i32 26,0
    %r415 = load i32, ptr @k
    %r416 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r414, i32 %r415
    %r417 = load float, ptr %r416
    %r418 = add i32 36,0
    %r419 = load i32, ptr @k
    %r420 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r418, i32 %r419
    %r421 = load float, ptr %r420
    %r422 = add i32 17,0
    %r423 = load i32, ptr @k
    %r424 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r422, i32 %r423
    %r425 = load float, ptr %r424
    %r426 = add i32 15,0
    %r427 = load i32, ptr @k
    %r428 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r426, i32 %r427
    %r429 = load i32, ptr %r428
    %r430 = add i32 2,0
    %r431 = load i32, ptr @k
    %r432 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r430, i32 %r431
    %r433 = load float, ptr %r432
    %r434 = add i32 14,0
    %r435 = load i32, ptr @k
    %r436 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r434, i32 %r435
    %r437 = load i32, ptr %r436
    %r438 = add i32 35,0
    %r439 = load i32, ptr @k
    %r440 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r438, i32 %r439
    %r441 = load float, ptr %r440
    %r442 = call float @params_f40_i24(i32 %r189,i32 %r193,i32 %r197,float %r201,i32 %r205,i32 %r209,i32 %r213,float %r217,float %r221,float %r225,i32 %r229,float %r233,float %r237,i32 %r241,float %r245,i32 %r249,float %r253,float %r257,float %r261,float %r265,float %r269,float %r273,i32 %r277,float %r281,i32 %r285,i32 %r289,float %r293,float %r297,float %r301,float %r305,float %r309,i32 %r313,float %r317,i32 %r321,float %r325,float %r329,float %r333,float %r337,i32 %r341,i32 %r345,float %r349,float %r353,float %r357,i32 %r361,float %r365,i32 %r369,i32 %r373,float %r377,float %r381,float %r385,float %r389,i32 %r393,i32 %r397,i32 %r401,float %r405,float %r409,float %r413,float %r417,float %r421,float %r425,i32 %r429,float %r433,i32 %r437,float %r441)
    store float %r442, ptr %r185
    %r444 = add i32 0,0
    %r445 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r444
    %r446 = add i32 1,0
    %r447 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r446
    %r448 = add i32 2,0
    %r449 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r448
    %r450 = add i32 3,0
    %r451 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r450
    %r452 = add i32 4,0
    %r453 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r452
    %r454 = add i32 5,0
    %r455 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r454
    %r456 = add i32 6,0
    %r457 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r456
    %r458 = add i32 7,0
    %r459 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r458
    %r460 = add i32 8,0
    %r461 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r460
    %r462 = add i32 9,0
    %r463 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r462
    %r464 = add i32 10,0
    %r465 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r464
    %r466 = add i32 11,0
    %r467 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r466
    %r468 = add i32 12,0
    %r469 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r468
    %r470 = add i32 13,0
    %r471 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r470
    %r472 = add i32 14,0
    %r473 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r472
    %r474 = add i32 15,0
    %r475 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r474
    %r476 = add i32 16,0
    %r477 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r476
    %r478 = add i32 17,0
    %r479 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r478
    %r480 = add i32 18,0
    %r481 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r480
    %r482 = add i32 19,0
    %r483 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r482
    %r484 = add i32 20,0
    %r485 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r484
    %r486 = add i32 21,0
    %r487 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r486
    %r488 = add i32 22,0
    %r489 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r488
    %r490 = add i32 23,0
    %r491 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r490
    %r492 = add i32 24,0
    %r493 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r492
    %r494 = add i32 25,0
    %r495 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r494
    %r496 = add i32 26,0
    %r497 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r496
    %r498 = add i32 27,0
    %r499 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r498
    %r500 = add i32 28,0
    %r501 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r500
    %r502 = add i32 29,0
    %r503 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r502
    %r504 = add i32 30,0
    %r505 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r504
    %r506 = add i32 31,0
    %r507 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r506
    %r508 = add i32 32,0
    %r509 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r508
    %r510 = add i32 33,0
    %r511 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r510
    %r512 = add i32 34,0
    %r513 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r512
    %r514 = add i32 35,0
    %r515 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r514
    %r516 = add i32 36,0
    %r517 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r516
    %r518 = add i32 37,0
    %r519 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r518
    %r520 = add i32 38,0
    %r521 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r520
    %r522 = add i32 39,0
    %r523 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r522
    %r524 = call float @params_fa40(ptr %r445,ptr %r447,ptr %r449,ptr %r451,ptr %r453,ptr %r455,ptr %r457,ptr %r459,ptr %r461,ptr %r463,ptr %r465,ptr %r467,ptr %r469,ptr %r471,ptr %r473,ptr %r475,ptr %r477,ptr %r479,ptr %r481,ptr %r483,ptr %r485,ptr %r487,ptr %r489,ptr %r491,ptr %r493,ptr %r495,ptr %r497,ptr %r499,ptr %r501,ptr %r503,ptr %r505,ptr %r507,ptr %r509,ptr %r511,ptr %r513,ptr %r515,ptr %r517,ptr %r519,ptr %r521,ptr %r523)
    store float %r524, ptr %r443
    %r526 = add i32 0,0
    %r527 = load i32, ptr @k
    %r528 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r526, i32 %r527
    %r529 = load float, ptr %r528
    %r530 = add i32 0,0
    %r531 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r530
    %r532 = add i32 1,0
    %r533 = load i32, ptr @k
    %r534 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r532, i32 %r533
    %r535 = load i32, ptr %r534
    %r536 = add i32 1,0
    %r537 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r536
    %r538 = add i32 2,0
    %r539 = load i32, ptr @k
    %r540 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r538, i32 %r539
    %r541 = load float, ptr %r540
    %r542 = add i32 2,0
    %r543 = load i32, ptr @k
    %r544 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r542, i32 %r543
    %r545 = load i32, ptr %r544
    %r546 = add i32 3,0
    %r547 = load i32, ptr @k
    %r548 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r546, i32 %r547
    %r549 = load float, ptr %r548
    %r550 = add i32 4,0
    %r551 = load i32, ptr @k
    %r552 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r550, i32 %r551
    %r553 = load float, ptr %r552
    %r554 = add i32 5,0
    %r555 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r554
    %r556 = add i32 3,0
    %r557 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r556
    %r558 = add i32 4,0
    %r559 = load i32, ptr @k
    %r560 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r558, i32 %r559
    %r561 = load i32, ptr %r560
    %r562 = add i32 5,0
    %r563 = load i32, ptr @k
    %r564 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r562, i32 %r563
    %r565 = load i32, ptr %r564
    %r566 = add i32 6,0
    %r567 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r566
    %r568 = add i32 6,0
    %r569 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r568
    %r570 = add i32 7,0
    %r571 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r570
    %r572 = add i32 8,0
    %r573 = load i32, ptr @k
    %r574 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r572, i32 %r573
    %r575 = load i32, ptr %r574
    %r576 = add i32 7,0
    %r577 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r576
    %r578 = add i32 8,0
    %r579 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r578
    %r580 = add i32 9,0
    %r581 = load i32, ptr @k
    %r582 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r580, i32 %r581
    %r583 = load float, ptr %r582
    %r584 = add i32 10,0
    %r585 = load i32, ptr @k
    %r586 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r584, i32 %r585
    %r587 = load float, ptr %r586
    %r588 = add i32 11,0
    %r589 = load i32, ptr @k
    %r590 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r588, i32 %r589
    %r591 = load float, ptr %r590
    %r592 = add i32 12,0
    %r593 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r592
    %r594 = add i32 9,0
    %r595 = load i32, ptr @k
    %r596 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r594, i32 %r595
    %r597 = load i32, ptr %r596
    %r598 = add i32 13,0
    %r599 = load i32, ptr @k
    %r600 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r598, i32 %r599
    %r601 = load float, ptr %r600
    %r602 = add i32 14,0
    %r603 = load i32, ptr @k
    %r604 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r602, i32 %r603
    %r605 = load float, ptr %r604
    %r606 = add i32 15,0
    %r607 = load i32, ptr @k
    %r608 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r606, i32 %r607
    %r609 = load float, ptr %r608
    %r610 = add i32 10,0
    %r611 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r610
    %r612 = add i32 16,0
    %r613 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r612
    %r614 = add i32 11,0
    %r615 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r614
    %r616 = add i32 12,0
    %r617 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r616
    %r618 = add i32 17,0
    %r619 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r618
    %r620 = add i32 18,0
    %r621 = load i32, ptr @k
    %r622 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r620, i32 %r621
    %r623 = load float, ptr %r622
    %r624 = add i32 19,0
    %r625 = load i32, ptr @k
    %r626 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r624, i32 %r625
    %r627 = load float, ptr %r626
    %r628 = add i32 13,0
    %r629 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r628
    %r630 = add i32 14,0
    %r631 = load i32, ptr @k
    %r632 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r630, i32 %r631
    %r633 = load i32, ptr %r632
    %r634 = add i32 20,0
    %r635 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r634
    %r636 = add i32 21,0
    %r637 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r636
    %r638 = add i32 22,0
    %r639 = load i32, ptr @k
    %r640 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r638, i32 %r639
    %r641 = load float, ptr %r640
    %r642 = add i32 23,0
    %r643 = load i32, ptr @k
    %r644 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r642, i32 %r643
    %r645 = load float, ptr %r644
    %r646 = add i32 15,0
    %r647 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r646
    %r648 = add i32 16,0
    %r649 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r648
    %r650 = add i32 17,0
    %r651 = load i32, ptr @k
    %r652 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r650, i32 %r651
    %r653 = load i32, ptr %r652
    %r654 = add i32 18,0
    %r655 = load i32, ptr @k
    %r656 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r654, i32 %r655
    %r657 = load i32, ptr %r656
    %r658 = add i32 24,0
    %r659 = load i32, ptr @k
    %r660 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r658, i32 %r659
    %r661 = load float, ptr %r660
    %r662 = add i32 25,0
    %r663 = load i32, ptr @k
    %r664 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r662, i32 %r663
    %r665 = load float, ptr %r664
    %r666 = add i32 19,0
    %r667 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r666
    %r668 = add i32 20,0
    %r669 = load i32, ptr @k
    %r670 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r668, i32 %r669
    %r671 = load i32, ptr %r670
    %r672 = add i32 26,0
    %r673 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r672
    %r674 = add i32 21,0
    %r675 = load i32, ptr @k
    %r676 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r674, i32 %r675
    %r677 = load i32, ptr %r676
    %r678 = add i32 22,0
    %r679 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r678
    %r680 = add i32 23,0
    %r681 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r680
    %r682 = add i32 27,0
    %r683 = load i32, ptr @k
    %r684 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r682, i32 %r683
    %r685 = load float, ptr %r684
    %r686 = add i32 28,0
    %r687 = load i32, ptr @k
    %r688 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r686, i32 %r687
    %r689 = load float, ptr %r688
    %r690 = add i32 29,0
    %r691 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r690
    %r692 = add i32 0,0
    %r693 = load i32, ptr @k
    %r694 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r692, i32 %r693
    %r695 = load i32, ptr %r694
    %r696 = add i32 1,0
    %r697 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r696
    %r698 = add i32 30,0
    %r699 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r698
    %r700 = add i32 31,0
    %r701 = load i32, ptr @k
    %r702 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r700, i32 %r701
    %r703 = load float, ptr %r702
    %r704 = add i32 2,0
    %r705 = load i32, ptr @k
    %r706 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r704, i32 %r705
    %r707 = load i32, ptr %r706
    %r708 = add i32 32,0
    %r709 = load i32, ptr @k
    %r710 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r708, i32 %r709
    %r711 = load float, ptr %r710
    %r712 = add i32 33,0
    %r713 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r712
    %r714 = add i32 34,0
    %r715 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r714
    %r716 = add i32 35,0
    %r717 = load i32, ptr @k
    %r718 = getelementptr [40 x [3 x float]], ptr %r0, i32 0, i32 %r716, i32 %r717
    %r719 = load float, ptr %r718
    %r720 = add i32 3,0
    %r721 = load i32, ptr @k
    %r722 = getelementptr [24 x [3 x i32]], ptr %r1, i32 0, i32 %r720, i32 %r721
    %r723 = load i32, ptr %r722
    %r724 = call i32 @params_mix(float %r529,ptr %r531,i32 %r535,ptr %r537,float %r541,i32 %r545,float %r549,float %r553,ptr %r555,ptr %r557,i32 %r561,i32 %r565,ptr %r567,ptr %r569,ptr %r571,i32 %r575,ptr %r577,ptr %r579,float %r583,float %r587,float %r591,ptr %r593,i32 %r597,float %r601,float %r605,float %r609,ptr %r611,ptr %r613,ptr %r615,ptr %r617,ptr %r619,float %r623,float %r627,ptr %r629,i32 %r633,ptr %r635,ptr %r637,float %r641,float %r645,ptr %r647,ptr %r649,i32 %r653,i32 %r657,float %r661,float %r665,ptr %r667,i32 %r671,ptr %r673,i32 %r677,ptr %r679,ptr %r681,float %r685,float %r689,ptr %r691,i32 %r695,ptr %r697,ptr %r699,float %r703,i32 %r707,float %r711,ptr %r713,ptr %r715,float %r719,i32 %r723)
    store i32 %r724, ptr %r525
    %r725 = load float, ptr %r23
    call void @putfloat(float %r725)
    %r726 = add i32 10,0
    call void @putch(i32 %r726)
    %r727 = load float, ptr %r185
    call void @putfloat(float %r727)
    %r728 = add i32 10,0
    call void @putch(i32 %r728)
    %r729 = load float, ptr %r443
    call void @putfloat(float %r729)
    %r730 = add i32 10,0
    call void @putch(i32 %r730)
    %r731 = load i32, ptr %r525
    call void @putint(i32 %r731)
    %r732 = add i32 10,0
    call void @putch(i32 %r732)
    %r733 = add i32 0,0
    ret i32 %r733
}
