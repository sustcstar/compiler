%{
    #include"syntax.tab.h"
    #include"parseTree.hpp"
    #include<string.h>

    #define YY_DECL extern "C" int yylex() 

    int yycolno = 1;
    int error = 0;
    int htoi(char *s);

    // 用来计算位置
    #define YY_USER_ACTION \
        yylloc.first_line = yylineno; \
        yylloc.first_column = yycolno; \
        yylloc.last_line = yylineno; \
        yylloc.last_column = yycolno + yyleng; \
        yycolno += yyleng;
%}
%option yylineno
%x COMMENT

number 0|[1-9][0-9]*
hex ("0x"|"0X")(0|[1-9a-fA-F][0-9a-fA-F]*)
hex_form \'\\x[0-9a-fA-F][0-9a-fA-F]\'

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
AND "&&"
OR \|\|
NOT !
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
WRONGID {number}{ID}
empty [ \r\t]
%%
{INT} { int value; 
    if(yytext[0] == '0' && (yytext[1] == 'x' || yytext[1] == 'X')){
        value = htoi(yytext);
    }
    else{
        value = atoi(yytext);
    }
    yylval.node = newNode("INT", TERMINAL_VALUE, (char *)value, yylineno, 0); return INT; }
{FLOAT} { 
    char *value; value = (char *)malloc(sizeof(yyleng)+1); 
    for(int i = 0;i < yyleng;i++){ value[i] = yytext[i]; value[yyleng] = '\0'; }
    yylval.node = newNode("FLOAT", TERMINAL_VALUE, value, yylineno, 0); return FLOAT; }
{CHAR} { 
    char *value; value = (char *)malloc(sizeof(yyleng)+1); 
    for(int i = 0;i < yyleng;i++){ value[i] = yytext[i]; value[yyleng] = '\0'; }
    yylval.node = newNode("CHAR", TERMINAL_VALUE, value, yylineno, 0); return CHAR; }
{TYPE} { 
    char *value; value = (char *)malloc(sizeof(yyleng)+1); 
    for(int i = 0;i < yyleng;i++){ value[i] = yytext[i]; value[yyleng] = '\0'; }
    yylval.node = newNode("TYPE", TERMINAL_VALUE, value, yylineno, 0); return TYPE; }
{STRUCT} { yylval.node = newNode("STRUCT", TERMINAL_NOVALUE, NULL, yylineno, 0); return STRUCT; }
{IF} { yylval.node = newNode("IF", TERMINAL_NOVALUE, NULL, yylineno, 0); return IF; }
{ELSE} { yylval.node = newNode("ELSE", TERMINAL_NOVALUE, NULL, yylineno, 0); return ELSE; }
{WHILE} { yylval.node = newNode("WHILE", TERMINAL_NOVALUE, NULL, yylineno, 0); return WHILE; }
{RETURN} { yylval.node = newNode("RETURN", TERMINAL_NOVALUE, NULL, yylineno, 0); return RETURN; }
{ID} { 
    char *value; value = (char *)malloc(sizeof(yyleng)+1); 
    for(int i = 0;i < yyleng;i++){ value[i] = yytext[i]; value[yyleng] = '\0'; }
    yylval.node = newNode("ID", TERMINAL_VALUE, value, yylineno, 0); return ID; }
{DOT} { yylval.node = newNode("DOT", TERMINAL_NOVALUE, NULL, yylineno, 0); return DOT; }
{SEMI} { yylval.node = newNode("SEMI", TERMINAL_NOVALUE, NULL, yylineno, 0); return SEMI; }
{COMMA} { yylval.node = newNode("COMMA", TERMINAL_NOVALUE, NULL, yylineno, 0); return COMMA; }
{ASSIGN} { yylval.node = newNode("ASSIGN", TERMINAL_NOVALUE, NULL, yylineno, 0); return ASSIGN; }
{LT} { yylval.node = newNode("LT", TERMINAL_NOVALUE, NULL, yylineno, 0); return LT; }
{LE} { yylval.node = newNode("LE", TERMINAL_NOVALUE, NULL, yylineno, 0); return LE; }
{GT} { yylval.node = newNode("GT", TERMINAL_NOVALUE, NULL, yylineno, 0); return GT; }
{GE} { yylval.node = newNode("GE", TERMINAL_NOVALUE, NULL, yylineno, 0); return GE; }
{NE} { yylval.node = newNode("NE", TERMINAL_NOVALUE, NULL, yylineno, 0); return NE; }
{EQ} { yylval.node = newNode("EQ", TERMINAL_NOVALUE, NULL, yylineno, 0); return EQ; }
{PLUS} { yylval.node = newNode("PLUS", TERMINAL_NOVALUE, NULL, yylineno, 0); return PLUS; }
{MINUS} { yylval.node = newNode("MINUS", TERMINAL_NOVALUE, NULL, yylineno, 0); return MINUS; }
{MUL} { yylval.node = newNode("MUL", TERMINAL_NOVALUE, NULL, yylineno, 0); return MUL; }
{DIV} { yylval.node = newNode("DIV", TERMINAL_NOVALUE, NULL, yylineno, 0); return DIV; }
{AND} { yylval.node = newNode("AND", TERMINAL_NOVALUE, NULL, yylineno, 0); return AND; }
{OR} { yylval.node = newNode("OR", TERMINAL_NOVALUE, NULL, yylineno, 0); return OR; }
{NOT} { yylval.node = newNode("NOT", TERMINAL_NOVALUE, NULL, yylineno, 0); return NOT; }
{LP} { yylval.node = newNode("LP", TERMINAL_NOVALUE, NULL, yylineno, 0); return LP; }
{RP} { yylval.node = newNode("RP", TERMINAL_NOVALUE, NULL, yylineno, 0); return RP; }
{LB} { yylval.node = newNode("LB", TERMINAL_NOVALUE, NULL, yylineno, 0); return LB; }
{RB} { yylval.node = newNode("RB", TERMINAL_NOVALUE, NULL, yylineno, 0); return RB; }
{LC} { yylval.node = newNode("LC", TERMINAL_NOVALUE, NULL, yylineno, 0); return LC; }
{RC} { yylval.node = newNode("RC", TERMINAL_NOVALUE, NULL, yylineno, 0); return RC; }
"\n" { yycolno = 1; }
"//" { char c; while((c=yyinput()) != '\n'); }
"/*" { BEGIN(COMMENT); }
<COMMENT>"*/" { BEGIN(INITIAL); }
<COMMENT>.    { }
<COMMENT>\n   { }
<COMMENT>"/*"   {
    error = 1; 
    printf("Error type A at Line %d: multi-line comments cannot be nested\n", yylineno);
}

{empty} {}
{WRONGID} { error = 1; printf("Error type A at Line %d: Unknown characters \'%s\'\n", 
yylineno, yytext); }
. { error = 1; printf("Error type A at Line %d: Unknown characters \'%s\'\n", 
yylineno, yytext); }
%%

int alphatolower(int c){
    if (c >= 'A' && c <= 'Z'){
        return c + 'a' - 'A';
    }
    else{
        return c;
    }
}

/* 将十六进制字符串转化成十进制整数 */
int htoi(char *s){
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X')){
        i = 2;
    }
    else{
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i){
        if (alphatolower(s[i]) > '9'){
            n = 16 * n + (10 + alphatolower(s[i]) - 'a');
        }
        else{
            n = 16 * n + (alphatolower(s[i]) - '0');
        }
    }
    return n;
}