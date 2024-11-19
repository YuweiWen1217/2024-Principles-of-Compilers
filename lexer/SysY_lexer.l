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

SINGLE_COMMENT \/\/.*\n?          
MULTI_COMMENT \/\*([^*]|\*+[^/])*\*\/
ID         [_a-zA-Z][_a-zA-Z0-9]* 
INT_CONST  ([1-9][0-9]*)|0           
OPERATOR   [\.\+\-\*\/\=\<\!\%\>]     
SYMBOL     [\{\}\;\(\)\,\[\]]         
WS [ \t\f\r\v]
OCTAL_CONST 0[0-7]+
HEXA_CONST  0[xX][0-9a-fA-F]+

FLOAT_CONST (([0-9]*[.][0-9]+|[0-9]+[.][0-9]*)([eE][+-]?[0-9]+)?)|([0-9]+[eE][+-]?[0-9]+)
HEX_FLOAT 0[xX]([0-9A-Fa-f]+[.]?[0-9A-Fa-f]*|[.][0-9A-Fa-f]+)([pP][+-]?[0-9]+)


%%

{SINGLE_COMMENT} {cur_col_number = col_number; col_number = 0; ++line_number;}
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

"/*" {  cur_col_number = col_number;
        col_number += strlen(yytext);
        yylval.error_msg = yytext;
        return ERROR;} 

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
    cur_col_number = col_number;
    col_number += strlen(yytext);
    // yylval.int_token = strtol(yytext, nullptr, 8);
    yylval.int_token = 0;
    for (int i = 1; yytext[i] != '\0'; ++i) {
        yylval.int_token = yylval.int_token * 8 + (yytext[i] - '0');
    }
    return INT_CONST;
}

{HEXA_CONST} {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.int_token = 0;
    for (int i = 2; yytext[i] != '\0'; ++i) {
        char c = yytext[i];
        int digit = (c >= '0' && c <= '9') ? (c - '0') : 
                    (c >= 'a' && c <= 'f') ? (c - 'a' + 10) : 
                    (c - 'A' + 10);
        yylval.int_token = yylval.int_token * 16 + digit;
    }
    return INT_CONST;
}

{FLOAT_CONST} {
    cur_col_number = col_number;
    col_number += strlen(yytext);

    double value = 0.0;          //最终结果
    int exponent = 0;           //指数
    bool hasDecimal = false;    //指示小数点前后
    double decimalFactor = 1.0;  //指示小数位数
    int i = 0;

    // 处理小数部分
    while (yytext[i] != '\0' && yytext[i] != 'e' && yytext[i] != 'E') 
    {
        if (yytext[i] == '.')
        {
            hasDecimal = true;
        } 
        else
        {
            if (hasDecimal) 
            {
                //小数部分
                decimalFactor *= 0.1;
                value += (yytext[i] - '0') * decimalFactor;
            } 
            else 
            {
                // 整数部分
                value = value * 10.0 + (yytext[i] - '0');
            }
        }
        i++;
    }

    // 处理指数部分
    if (yytext[i] == 'e' || yytext[i] == 'E') 
    {
        i++;
        int expSign = 1;
        if (yytext[i] == '+' || yytext[i] == '-') 
        {
            if (yytext[i] == '-') 
            {
                expSign = -1;
            }
            i++;
        }
        while (yytext[i] >= '0' && yytext[i] <= '9') 
        {
            exponent = exponent * 10 + (yytext[i] - '0');
            i++;
        }
        exponent *= expSign;
        value *= pow(10, exponent);  
    } 
    yylval.float_token = (float)value;
    return FLOAT_CONST;
}

{HEX_FLOAT} {
    cur_col_number = col_number;
    col_number += strlen(yytext);


    double value = 0.0;
    int exponent = 0;
    bool hasFraction = false;
    double decimalFactor = 1.0;
    int i = 2;       //跳过 "0x/0x"
    double addvalue = 0.0;

    // 处理小数部分
    while (yytext[i] != '\0' && yytext[i] != 'p' && yytext[i] != 'P') 
    {
        if (yytext[i] == '.') 
        {
            hasFraction = true;
        } 
        else
        {
            if (hasFraction) 
            {
                decimalFactor *= 0.0625; // 1/16
                addvalue = (yytext[i] >= '0' && yytext[i] <= '9') ? (yytext[i] - '0') :
                         (yytext[i] >= 'a' && yytext[i] <= 'f') ? (yytext[i] - 'a' + 10) :
                         (yytext[i] - 'A' + 10);
                value += decimalFactor * addvalue;
            } 
            else
            {   value *= 16.0;
                value += (yytext[i] >= '0' && yytext[i] <= '9') ? (yytext[i] - '0') :
                        (yytext[i] >= 'a' && yytext[i] <= 'f') ? (yytext[i] - 'a' + 10) :
                        (yytext[i] - 'A' + 10);
            }
        }
        i++;
    }

    // 处理指数部分
    if (yytext[i] == 'p' || yytext[i] == 'P') 
    {
        i++;
        int expSign = 1;
        if (yytext[i] == '+' || yytext[i] == '-') 
        {
            if (yytext[i] == '-') 
            {
                expSign = -1;
            }
            i++;
        }
        while (yytext[i] >= '0' && yytext[i] <= '9')
        {
            exponent = exponent * 10 + (yytext[i] - '0');
            i++;
        }
        exponent *= expSign;
        value *= pow(2, exponent);
    }

    yylval.float_token = (float)value;
    return FLOAT_CONST;
}



    /*unknown tokens, return ERROR*/
. {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.error_msg = yytext;
    return ERROR;
}
%%


