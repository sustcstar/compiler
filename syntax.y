%{
    #include"lex.yy.c"
    void yyerror(const char*);
%}

%token LC RC LB RB COLON COMMA
%token STRING NUMBER LEADING_ZERO_NUMBER
%token TRUE FALSE VNULL
%%

// | LC Members COMMA RC error { puts("should be closed with closing brace, recovered"); }

Json:
      Value
    ;
Value:
      Object
    | Array
    | STRING
    | NUMBER
    | LEADING_ZERO_NUMBER error { puts("Numbers cannot have leading zeroes, recovered"); }
    | TRUE
    | FALSE
    | VNULL
    ;
Object:
      LC RC
    // | LC Members COMMA { puts("Comma instead if closing brace, recovered"); }
    | LC Members RC
    | LC Members RC STRING error { puts("redundant string after object, recovered"); }
    ;
Members:
      Member
    | Member COMMA Members
    ;
Member:
      STRING COLON Value
    | STRING COLON Value COMMA error { puts("Extra comma, recovered"); }
    | STRING COLON COLON Value error { puts("Double colon, recovered"); }
    | STRING Value error { puts("Missing colon, recovered"); }
    | STRING COMMA Value error { puts("Comma instead of colon, recovered"); }
    ;
Array:
      LB RB
    | LB Values RB
    | LB Values COMMA RB  error { puts("extra comma, recovered"); } 
    | LB Values RB COMMA error { puts("Comma after the close, recovered"); } 
    | LB Values RC error { puts("unmatched right bracket, recovered"); }
    | LB Values RB RB error { puts("redundant close, recovered"); }
    | LB Value COLON Values RB error { puts("Colon instead of comma, recovered"); }
    | LB Values error { puts("unclosed array, recovered"); }
    ;
Values:
      Value
    | Value COMMA error { puts("extra comma, recovered"); }
    | Value COMMA COMMA error { puts("double extra comma, recovered"); }
    | Value COMMA Values
    | COMMA Values error { puts("missing value, recovered"); }
    ;
%%

void yyerror(const char *s){
    printf("syntax error: ");
}

int main(int argc, char **argv){
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(-1);
    }
    else if(!(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        exit(-1);
    }
    yyparse();
    return 0;
}
