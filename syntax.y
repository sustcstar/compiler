%{
    #include"lex.yy.c"
    parseTree *root;
    void yyerror(const char*);
%}

%locations

%union{
    struct parseTree *node;
}

//key words
%token <node> STRUCT RETURN IF ELSE WHILE
//type
%token <node> INT CHAR FLOAT TYPE 
//symbol
%token <node> SEMI COMMA DOT
//brackets
%token <node> LC RC LB RB LP RP 
//calculation operator
%token <node> MUL DIV PLUS MINUS
//relation operator
%token <node> LT LE GT GE EQ NE
//logical operator
%token <node> AND OR NOT
//others
%token <node> ASSIGN ID

//--------------------------------nonterminals
%type <node> Program ExtDefList ExtDef ExtDecList
%type <node> Specifier StructSpecifier
%type <node> VarDec FunDec VarList ParamDec
%type <node> CompSt StmtList Stmt
%type <node> DefList Def DecList Dec
%type <node> Exp Args

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

        // yylloc.first_line = yylineno; \
        // yylloc.first_column = yycolno; \
        // yylloc.last_line = yylineno; \
        // yylloc.last_column = yycolno + yyleng; \
        // printf("first_line = %d\nfirst_column = %d\nlast_line = %d\nlast_column = %d\n", @1.first_line, @1.first_column, @1.last_line, @1.last_column); 

%%
// high-level definition
Program: ExtDefList { root = newNode("Program", NONTERMINAL, NULL, @1.first_line, 1, $1); }
    ;
ExtDefList: ExtDef ExtDefList { $$ = newNode("ExtDefList", NONTERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | %empty { $$ = newNode("ExtDefList", NONTERMINAL, NULL, yylineno, 0); }
    ;
ExtDef: Specifier ExtDecList SEMI { $$ = newNode("ExtDef", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Specifier SEMI { $$ = newNode("ExtDef", NONTERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | Specifier FunDec CompSt { $$ = newNode("ExtDef", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    ;
ExtDecList: VarDec { $$ = newNode("ExtDecList", NONTERMINAL, NULL, @1.first_line, 1, $1); }
    | VarDec COMMA ExtDecList { $$ = newNode("ExtDecList", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    ;

// specifier
Specifier: TYPE { $$ = newNode("Specifier", NONTERMINAL, NULL, @1.first_line, 1, $1); }
    | StructSpecifier { $$ = newNode("Specifier", NONTERMINAL, NULL, @1.first_line, 1, $1); }
    ;
StructSpecifier: STRUCT ID LC DefList RC { $$ = newNode("StructSpecifier", NONTERMINAL, NULL, @1.first_line, 5, $1, $2, $3, $4, $5); }
    | STRUCT ID { $$ = newNode("StructSpecifier", NONTERMINAL, NULL, @1.first_line, 2, $1, $2); }
    ;

// declarator
VarDec: ID { $$ = newNode("VarDec", NONTERMINAL, NULL, @1.first_line, 1, $1); }
    | VarDec LB INT RB { $$ = newNode("VarDec", NONTERMINAL, NULL, @1.first_line, 4, $1, $2, $3, $4); }
    ;
FunDec: ID LP VarList RP { $$ = newNode("FunDec", NONTERMINAL, NULL, @1.first_line, 4, $1, $2, $3, $4); }
    | ID LP RP { $$ = newNode("FunDec", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    ;
VarList: ParamDec COMMA VarList { $$ = newNode("VarList", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | ParamDec { $$ = newNode("VarList", NONTERMINAL, NULL, @1.first_line, 1, $1); }
    ;
ParamDec: Specifier VarDec { $$ = newNode("ParamDec", NONTERMINAL, NULL, @1.first_line, 2, $1, $2); }
    ;

// statement
CompSt: LC DefList StmtList RC { $$ = newNode("CompSt", NONTERMINAL, NULL, @1.first_line, 4, $1, $2, $3, $4); }
    ;
StmtList: Stmt StmtList { $$ = newNode("StmtList", NONTERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | %empty { $$ = newNode("StmtList", NONTERMINAL, NULL, yylineno, 0); }
    ;
Stmt: Exp SEMI { $$ = newNode("Stmt", NONTERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | CompSt { $$ = newNode("Stmt", NONTERMINAL, NULL, @1.first_line, 1, $1); }
    | RETURN Exp SEMI { $$ = newNode("Stmt", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | IF LP Exp RP Stmt { $$ = newNode("Stmt", NONTERMINAL, NULL, @1.first_line, 5, $1, $2, $3, $4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt { $$ = newNode("Stmt", NONTERMINAL, NULL, @1.first_line, 7, $1, $2, $3, $4, $5, $6, $7); }
    | WHILE LP Exp RP Stmt { $$ = newNode("Stmt", NONTERMINAL, NULL, @1.first_line, 5, $1, $2, $3, $4, $5); }
    ;

// local definition
DefList: Def DefList { $$ = newNode("DefList", NONTERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | %empty { $$ = newNode("DefList", NONTERMINAL, NULL, yylineno, 0); }
    ;
Def: Specifier DecList SEMI { $$ = newNode("Def", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    ;
DecList: Dec { $$ = newNode("DecList", NONTERMINAL, NULL, @1.first_line, 1, $1); }
    | Dec COMMA DecList { $$ = newNode("DecList", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    ;
Dec: VarDec { $$ = newNode("Dec", NONTERMINAL, NULL, @1.first_line, 1, $1); }
    | VarDec ASSIGN Exp { $$ = newNode("Dec", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    ;
    
// Expression
Exp: Exp ASSIGN Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp AND Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp OR Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp LT Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp LE Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp GT Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp GE Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp NE Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp EQ Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp PLUS Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp MINUS Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp MUL Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp DIV Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | LP Exp RP { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | MINUS Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | NOT Exp { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | ID LP Args RP { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 4, $1, $2, $3, $4); }
    | ID LP RP { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp LB Exp RB  { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 4, $1, $2, $3, $4); }
    | Exp DOT ID { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | ID  { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 1, $1); }
    | INT { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 1, $1); }
    | FLOAT { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 1, $1); }
    | CHAR { $$ = newNode("Exp", NONTERMINAL, NULL, @1.first_line, 1, $1); }
    ;
Args: Exp COMMA Args { $$ = newNode("Args", NONTERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp { $$ = newNode("Args", NONTERMINAL, NULL, @1.first_line, 1, $1); }
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
    preOrderPrint(root, 0);
    return 0;
}
