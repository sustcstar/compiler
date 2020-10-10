%{
    #include"syntax.tab.h"
    // just let you know you have macros!
    // #define EXIT_OK 0
    // #define EXIT_FAIL 1

    // and global variables
    // int chars = 0;
    // int words = 0;
    // int lines = 0;
%}
%option yylineno

number 0|[1-9][0-9]*
hex ("0x"|"0X")(0|[1-9a-fA-F][0-9a-fA-F]*)
hex_form \\x[0-9a-fA-F][0-9a-fA-F]

INT {number}|{hex}
FLOAT [0-9]+"."[0-9]+
CHAR \'.\'|{hex_form}
ID [A-Za-z_][A-Za-z0-9_]*
TYPE "int"|"float"|"char"
STRUCT struct
IF if
ELSE else
WHILE while
RETURN return
DOT \.
SEMI ;
COMMA ,
ASSIGN =
LT <
LE <=
GT >
GE >=
NE !=
EQ ==
PLUS \+
MINUS -
MUL \*
DIV \/
AND &&
OR \|\|
NOT !
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
empty [ \n\r\t]
%%
{INT} { return INT; }
{FLOAT} { return FLOAT; }
{CHAR} { return CHAR; }
{TYPE} { return TYPE; }
{STRUCT} { return STRUCT; }
{IF} { return IF; }
{ELSE} { return ELSE; }
{WHILE} { return WHILE; }
{RETURN} { return RETURN; }
{ID} { return ID; }
{DOT} { return DOT; }
{SEMI} { return SEMI; }
{COMMA} { return COMMA; }
{ASSIGN} { return ASSIGN; }
{LT} { return LT; }
{LE} { return LE; }
{GT} { return GT; }
{GE} { return GE; }
{NE} { return NE; }
{EQ} { return EQ; }
{PLUS} { return PLUS; }
{MINUS} { return MINUS; }
{MUL} { return MUL; }
{DIV} { return DIV; }
{AND} { return AND; }
{OR} { return OR; }
{NOT} { return NOT; }
{LP} { return LP; }
{RP} { return RP; }
{LB} { return LB; }
{RB} { return RB; }
{LC} { return LC; }
{RC} { return RC; }
{empty} {}
. { printf("Error type A at Line %d: Unknown characters \'%s\'\n", 
yylineno, yytext); }
%%
// int main(int argc, char **argv){
//     char *file_path;
//     if(argc < 2){
//         fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
//         return EXIT_FAIL;
//     }
//     else if(argc == 2){
//         file_path = argv[1];
//         if(!(yyin = fopen(file_path, "r"))){
//             perror(argv[1]);
//             return EXIT_FAIL;
//         }
//         yylex();
//         printf("%-8d%-8d%-8d%s\n", lines, words, chars, file_path);
//         return EXIT_OK;
//     }
//     else{
//         fputs("Too much arguments!\n", stderr);
//         return EXIT_FAIL;
//     }
// }
// {INT} { printf("INT %s\n", yytext); }
// {FLOAT} { printf("FLOAT %s\n", yytext); }
// {CHAR} { printf("CHAR %s\n", yytext); }
// {TYPE} { printf("TYPE %s\n", yytext); }
// {STRUCT} { printf("STRUCT %s\n", yytext); }
// {IF} { printf("IF %s\n", yytext); }
// {ELSE} { printf("ELSE %s\n", yytext); }
// {WHILE} { printf("WHILE %s\n", yytext); }
// {RETURN} { printf("RETURN %s\n", yytext); }
// {ID} { printf("ID %s\n", yytext); }
// {DOT} { printf("DOT %s\n", yytext); }
// {SEMI} { printf("SEMI %s\n", yytext); }
// {COMMA} { printf("COMMA %s\n", yytext); }
// {ASSIGN} { printf("ASSIGN %s\n", yytext); }
// {LT} { printf("LT %s\n", yytext); }
// {LE} { printf("LE %s\n", yytext); }
// {GT} { printf("GT %s\n", yytext); }
// {GE} { printf("GE %s\n", yytext); }
// {NE} { printf("NE %s\n", yytext); }
// {EQ} { printf("EQ %s\n", yytext); }
// {PLUS} { printf("PLUS %s\n", yytext); }
// {MINUS} { printf("MINUS %s\n", yytext); }
// {MUL} { printf("MUL %s\n", yytext); }
// {DIV} { printf("DIV %s\n", yytext); }
// {AND} { printf("AND %s\n", yytext); }
// {OR} { printf("OR %s\n", yytext); }
// {NOT} { printf("NOT %s\n", yytext); }
// {LP} { printf("LP %s\n", yytext); }
// {RP} { printf("RP %s\n", yytext); }
// {LB} { printf("LB %s\n", yytext); }
// {RB} { printf("RB %s\n", yytext); }
// {LC} { printf("LC %s\n", yytext); }
// {RC} { printf("RC %s\n", yytext); }

// {INT} { }
// {FLOAT} { }
// {CHAR} { }
// {TYPE} { }
// {STRUCT} { }
// {IF} { }
// {ELSE} {  }
// {WHILE} {  }
// {RETURN} {  }
// {ID} { }
// {DOT} {  }
// {SEMI} {  }
// {COMMA} {  }
// {ASSIGN} {  }
// {LT} { }
// {LE} { }
// {GT} { }
// {GE} { }
// {NE} { }
// {EQ} { }
// {PLUS} {  }
// {MINUS} { }
// {MUL} {  }
// {DIV} {  }
// {AND} {  }
// {OR} {  }
// {NOT} { }
// {LP} { }
// {RP} {  }
// {LB} {  }
// {RB} {  }
// {LC} {  }
// {RC} {  }
