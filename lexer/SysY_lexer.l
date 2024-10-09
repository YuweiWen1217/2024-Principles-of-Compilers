%option noyywrap
%{
#include <cmath>
#include <vector>
#include <cstring>
#include "../include/symtab.h"
#include "../parser/SysY_parser.tab.h"

extern int line_number;
extern int col_number;
extern int cur_col_number;
extern IdTable id_table;
void yyerror(char* s, ...);
%}

SINGLE_COMMENT \/\/.*\n          
MULTI_COMMENT \/\*([^*]|\*+[^/])*\*\/
ID         [_a-zA-Z][_a-zA-Z0-9]* 
INT_CONST  ([1-9][0-9]*)|0           
OPERATOR   [\.\+\-\*\/\=\<\!\%\>]     
SYMBOL     [\{\}\;\(\)\,\[\]]         
WS [ \t\f\r\v]
OCTAL_CONST 0[0-7]+
HEXA_CONST  0[xX][0-9a-fA-F]+



%%

{SINGLE_COMMENT} {cur_col_number = col_number; col_number += strlen(yytext); ++line_number;}
{MULTI_COMMENT} {
    for (char* p = yytext; *p; ++p) {
        if (*p == '\n') {
            ++line_number;
            col_number = 0;
        } else {
            ++col_number;
        }
    }
}

"<=" {  cur_col_number = col_number;
        col_number += strlen(yytext);
        return LEQ;}
">=" {  cur_col_number = col_number;
        col_number += strlen(yytext);
        return GEQ;}
"==" {  cur_col_number = col_number;
        col_number += strlen(yytext);
        return EQ;}
"!=" {  cur_col_number = col_number;
        col_number += strlen(yytext);
        return NE;}
"&&" {  cur_col_number = col_number;
        col_number += strlen(yytext);
        return AND;}         
"||" {  cur_col_number = col_number;
        col_number += strlen(yytext);
        return OR;}               
"const" {   cur_col_number = col_number;
            col_number += strlen(yytext);
            return CONST;}     
"if" {  cur_col_number = col_number;
        col_number += strlen(yytext);
        return IF;}       
"else" {cur_col_number = col_number;
        col_number += strlen(yytext);
        return ELSE;}  
"while" {   cur_col_number = col_number;
            col_number += strlen(yytext);
            return WHILE;}
"void" {cur_col_number = col_number;
        col_number += strlen(yytext);
        return NONE_TYPE;}    
"int" { cur_col_number = col_number;
        col_number += strlen(yytext);
        return INT;}    
"float" {   cur_col_number = col_number;
            col_number += strlen(yytext);
            return FLOAT;}      
"return" {  cur_col_number = col_number;
            col_number += strlen(yytext);
            return RETURN;}    
"break" {   cur_col_number = col_number;
            col_number += strlen(yytext);
            return BREAK;}     
"continue" {    cur_col_number = col_number;
                col_number += strlen(yytext);
                return CONTINUE;}

\n {++line_number;col_number = 0;}

{WS} {col_number += strlen(yytext);}

{OPERATOR} {    cur_col_number = col_number;
                col_number += strlen(yytext);
                return yytext[0];}

{SYMBOL} {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    return yytext[0];}

{ID} {  cur_col_number = col_number;
        col_number += strlen(yytext);
        yylval.symbol_token = id_table.add_id(std::string(yytext));
        return IDENT;}


{INT_CONST} {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    // yylval.int_token = stoi(std::string(yytext));
    yylval.int_token = 0;
    for(int i = 0;yytext[i];i++){
        yylval.int_token *= 10;
        yylval.int_token += yytext[i] - '0';
    }
    return INT_CONST;
}
    /*
      在词法分析中，你只需要考虑yylval的三种类型，分别为int_token, float_token, symbol_token
      之所以要定义这三种类型，是因为我们只返回这个token的类型是不够的，我们还需要知道这个token的属性
      例如我们需要知道INT_CONST具体的值是多少
    */

{OCTAL_CONST} {
    cur_col_number = col_number; // 保存当前 token 开始位置
    col_number += strlen(yytext); // 更新结束位置
    yylval.int_token = strtol(yytext, nullptr, 8); // 处理八进制整型常量
    return INT_CONST;
}

{HEXA_CONST} {
    cur_col_number = col_number; // 保存当前 token 开始位置
    col_number += strlen(yytext); // 更新结束位置
    yylval.int_token = strtol(yytext, nullptr, 16); // 处理十六进制整型常量
    return INT_CONST;
}





    /*unknown tokens, return ERROR*/
. {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.error_msg = yytext;
    return ERROR;
}
%%



