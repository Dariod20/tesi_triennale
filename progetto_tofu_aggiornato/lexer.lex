%{   
#include "parser.h"                                                                                                                     
#include "def.h"
int line = 1;                                                       
Value lexval;
%}                                                                       
%option noyywrap

spacing     	([ \t])+
letter      	[A-Za-z]
digit       	[0-9]
intconst    	{digit}+
strconst    	\"([^\"])*\"
boolconst   	false|true
id          	{letter}({letter}|{digit})*
op	  		[/+\-\*<>=]
comment	  	#.*
sugar       	[:;,()\[\]!\?]
%%
{spacing}   	;
{comment}	  	;
\n          	{line++;}
{sugar}     	{return(yytext[0]);}
variables   	{return(VARIABLES);}
functions   	{return(FUNCTIONS);}
if   	  	{return(IF);}
then    	  	{return(THEN);}
else    	  	{return(ELSE);}
end      	  	{return(END);}
body  	  	{return(BODY);}
show    	  	{return(SHOW);}
"->"			{return(FUNC_RET);}
and	  	  	{return(AND);}
or		  	{return(OR);}
"=="	   	  	{return(EQ);}
"!="	   	  	{return(NE);}
">="	   	  	{return(GE);}
"<="	   	  	{return(LE);}
{op}    		{return(yytext[0]);}
"++" 	  	{return(APP);}
"not"	  	{return(NOT);}
int         	{return(INT);}
string      	{return(STRING);}
bool        	{return(BOOL);}
{intconst}  	{lexval.ival = atoi(yytext); return(INTCONST);}
{strconst}  	{lexval.sval = newstring(yytext); return(STRCONST);}
{boolconst} 	{lexval.ival = (yytext[0] == 'f' ? 0 : 1); return(BOOLCONST);}
{id}        	{lexval.sval = newstring(yytext); return(ID);}
.           	{return(ERROR);}
%%

char *newstring(char *s)
{
  char *p;
  
  p = malloc(strlen(s)+1);
  strcpy(p, s);
  return(p);
}


