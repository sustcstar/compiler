%{
    #include"lex.yy.c"
    void yyerror(const char*);
%}
//key words
%token STRUCT RETURN IF ELSE WHILE
//type
%token INT CHAR FLOAT TYPE 
//symbol
%token SEMI COMMA DOT
//brackets
%token LC RC LB RB LP RP 
//calculation operator
%token MUL DIV PLUS MINUS
//relation operator
%token LT LE GT GE EQ NE
//logical operator
%token AND OR NOT
//others
%token ASSIGN ID

%right ASSIGN
%left OR
%left AND
%left LT LE GT GE EQ NE
%left PLUS MINUS
%left MUL DIV
%right NOT
%left LC RC LB RB LP RP DOT
// %nonassoc INT CHAR FLOAT ID
// %nonassoc ELSE


%%
// high-level definition
Program: ExtDefList
    ;
ExtDefList: ExtDef ExtDefList
    | %empty
    ;
ExtDef: Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    ;
ExtDecList: VarDec 
    | VarDec COMMA ExtDecList
    ;

// specifier
Specifier: TYPE
    | StructSpecifier
    ;
StructSpecifier: STRUCT ID LC DefList RC 
    | STRUCT ID
    ;

// declarator
VarDec: ID 
    | VarDec LB INT RB
    ;
FunDec: ID LP VarList RP 
    | ID LP RP 
    ;
VarList: ParamDec COMMA VarList
    | ParamDec 
    ;
ParamDec: Specifier VarDec 
    ;

// statement
CompSt: LC DefList StmtList RC 
    ;
StmtList: Stmt StmtList
    | %empty 
    ;
Stmt: Exp SEMI 
    | CompSt 
    | RETURN Exp SEMI 
    | IF LP Exp RP Stmt
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt 
    ;

// local definition
DefList: Def DefList
    | %empty
    ;
Def: Specifier DecList SEMI 
    ;
DecList: Dec 
    | Dec COMMA DecList 
    ;
Dec: VarDec 
    | VarDec ASSIGN Exp 
    ;
    
// Expression
Exp: Exp ASSIGN Exp
    | Exp AND Exp 
    | Exp OR Exp 
    | Exp LT Exp 
    | Exp LE Exp 
    | Exp GT Exp 
    | Exp GE Exp 
    | Exp NE Exp 
    | Exp EQ Exp 
    | Exp PLUS Exp 
    | Exp MINUS Exp 
    | Exp MUL Exp 
    | Exp DIV Exp 
    | LP Exp RP 
    | MINUS Exp 
    | NOT Exp 
    | ID LP Args RP
    | ID LP RP 
    | Exp LB Exp RB 
    | Exp DOT ID 
    | ID 
    | INT 
    | FLOAT 
    | CHAR 
    ;
Args: Exp COMMA Args
    | Exp 
    ;
%%

void yyerror(const char *s){
    printf("syntax error: \n");
}

int main(int argc, char **argv){
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(-1);
    }
    // read a file
    else if(!(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        exit(-1);
    }
    yyparse();
    return 0;
}
