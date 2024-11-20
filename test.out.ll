Program Semant
CompUnit_FuncDef Semant
FuncDef Semant
FuncFParam Semant
BlockItem_Decl Semant
VarDecl Semant
VarDefNoInit Semant
BlockItem_Decl Semant
return_stmt Semant
Lval Semant
1
CompUnit_FuncDef Semant
FuncDef Semant


开始中间代码生成了。

Program CodeIR
FunctionDef CodeIR
Block CodeIR
BlockItemDecl CodeIR
VarDecl CodeIR
VarDefNoInit CodeIR
BlockItemStmt CodeIR
ReturnStmt CodeIR
Lval CodeIR
111  1
IRgenTypeConverse: Handling type conversion
FunctionDef CodeIR
Block CodeIR
Program
  FuncDef   Name:getstr   ReturnType: Int
    FuncFParam   name:a   Type:Int   scope:1

    Block   Size:2
      VarDecls   Type: Int
        VarDef   name:length   scope:1

      ReturnStmt:
        Lval   Type: Int      name:length   scope:1
  FuncDef   Name:main   ReturnType: Int
    Block   Size:0
