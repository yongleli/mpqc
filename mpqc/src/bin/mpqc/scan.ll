%option c++ prefix="MPQCIn" yylineno

%{

#if !defined(SUN4)
#include <string.h>
#endif

#include <util/misc/exenv.h>
#include <util/container/array.h>

#include "parse.h"
#include "mpqcin.h"

static inline char *
cstr(char *yytext)
{
  if (MPQCIn::checking()) return 0;
  char *ret;
  int strlenyytext = strlen(yytext);
  ret = (char *)malloc(strlenyytext+1);
  if (!ret) {
    ExEnv::out() << "MPQC: malloc failed"
                 << endl;
    abort();
  }
  strcpy(ret,yytext);
  return ret;
}

%}
string  [A-Za-z0-9_\.*+-/]*
qstring \"[^"\n]+\"
%%
"molecule"      { return T_MOLECULE; }
"multiplicity"  { return T_MULTIPLICITY; }
"optimize"      { return T_OPTIMIZE; }
"gradient"      { return T_GRADIENT; }
"frequencies"   { return T_FREQUENCIES; }
"yes"           { yylval.i = 1; return T_BOOL; }
"no"            { yylval.i = 0; return T_BOOL; }
"charge"        { return T_CHARGE; }
"method"        { return T_METHOD; }
"basis"         { return T_BASIS; }
"cartesian"     { return T_CARTESIAN; }
"internal"      { return T_INTERNAL; }
"redundant"     { return T_REDUNDANT; }
"restart"       { return T_RESTART; }
"checkpoint"    { return T_CHECKPOINT; }
"xc"            { return T_XC; }
"symmetry"      { return T_SYMMETRY; }
"bohr"          { return T_BOHR; }
"angstrom"      { return T_ANGSTROM; }
"docc"          { return T_DOCC; }
"socc"          { return T_SOCC; }
"alpha"         { return T_ALPHA; }
"beta"          { return T_BETA; }
"frozen_docc"   { return T_FROZEN_DOCC; }
"frozen_uocc"   { return T_FROZEN_UOCC; }
"grid"          { return T_GRID; }
"="             { return T_EQUALS; }
":"             { return T_COLON; }
"("             { return T_BEG_OPT; }
")"             { return T_END_OPT; }
"<"             { return T_OO_INPUT_KEYWORD; }
"mpqc"          { return T_OO_INPUT_KEYWORD; }
{string}        { yylval.str = cstr(yytext);
                  return T_STRING;
                  }
{qstring}       { if (MPQCIn::checking()) return T_STRING;
                  yylval.str = (char *)malloc(strlen(yytext));
                  if (!yylval.str) {
                    ExEnv::out() << "MPQC: {qstring} rule: malloc failed"
                                 << endl;
                    abort();
                    }
                  strcpy(yylval.str,&yytext[1]);
                  yylval.str[strlen(yylval.str)-1] = '\0';
                  return(T_STRING);
                  }
[ \t]+          ; 
%.*$            ;
[\n\r\f]        ;
.               { ExEnv::out() << "MPQC: Illegal character: \""
                               << yytext[0] <<"\"" << endl; }
%%

int
MPQCInwrap()
{
  return 1;
}
