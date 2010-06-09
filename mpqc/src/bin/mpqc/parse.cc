/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_NOT = 258,
     T_MOLECULE = 259,
     T_MULTIPLICITY = 260,
     T_CHARGE = 261,
     T_METHOD = 262,
     T_BASIS = 263,
     T_AUXBASIS = 264,
     T_DFBASIS = 265,
     T_EQUALS = 266,
     T_OPTIMIZE = 267,
     T_GRADIENT = 268,
     T_BEG_OPT = 269,
     T_END_OPT = 270,
     T_CARTESIAN = 271,
     T_INTERNAL = 272,
     T_REDUNDANT = 273,
     T_RESTART = 274,
     T_CHECKPOINT = 275,
     T_COLON = 276,
     T_SYMMETRY = 277,
     T_MEMORY = 278,
     T_TMPDIR = 279,
     T_TMPSTORE = 280,
     T_DEBUG = 281,
     T_ACCURACY = 282,
     T_BOHR = 283,
     T_ANGSTROM = 284,
     T_FREQUENCIES = 285,
     T_LINDEP = 286,
     T_MAXITER = 287,
     T_SCF = 288,
     T_UC = 289,
     T_DOCC = 290,
     T_SOCC = 291,
     T_FROZEN_DOCC = 292,
     T_FROZEN_UOCC = 293,
     T_ALPHA = 294,
     T_BETA = 295,
     T_XC = 296,
     T_GRID = 297,
     T_RI = 298,
     T_F12 = 299,
     T_APP = 300,
     T_ANSATZ = 301,
     T_OO_INPUT_KEYWORD = 302,
     T_STRING = 303,
     T_BOOL = 304
   };
#endif
/* Tokens.  */
#define T_NOT 258
#define T_MOLECULE 259
#define T_MULTIPLICITY 260
#define T_CHARGE 261
#define T_METHOD 262
#define T_BASIS 263
#define T_AUXBASIS 264
#define T_DFBASIS 265
#define T_EQUALS 266
#define T_OPTIMIZE 267
#define T_GRADIENT 268
#define T_BEG_OPT 269
#define T_END_OPT 270
#define T_CARTESIAN 271
#define T_INTERNAL 272
#define T_REDUNDANT 273
#define T_RESTART 274
#define T_CHECKPOINT 275
#define T_COLON 276
#define T_SYMMETRY 277
#define T_MEMORY 278
#define T_TMPDIR 279
#define T_TMPSTORE 280
#define T_DEBUG 281
#define T_ACCURACY 282
#define T_BOHR 283
#define T_ANGSTROM 284
#define T_FREQUENCIES 285
#define T_LINDEP 286
#define T_MAXITER 287
#define T_SCF 288
#define T_UC 289
#define T_DOCC 290
#define T_SOCC 291
#define T_FROZEN_DOCC 292
#define T_FROZEN_UOCC 293
#define T_ALPHA 294
#define T_BETA 295
#define T_XC 296
#define T_GRID 297
#define T_RI 298
#define T_F12 299
#define T_APP 300
#define T_ANSATZ 301
#define T_OO_INPUT_KEYWORD 302
#define T_STRING 303
#define T_BOOL 304




/* Copy the first part of user declarations.  */
#line 1 "parse.yy"

#ifdef DEC
#include <math.h>
#else
#include <stdlib.h>
#endif
#include <string.h>
#ifdef BISON
#define YYDEBUG 0
#if YYDEBUG != 0
int yydebug =1;
#endif /* YYDEBUG != 0 */
#endif /* BISON */
#if defined(SABER)
#define xmalloc malloc
#endif
#if defined(SGI)
#include <alloca.h>
#endif
#include "mpqcin.h"
#define yyerror sc::MPQCIn::yerror
#define yyparse sc::MPQCIn::yparse
#define yylex sc::MPQCIn::ylex
#define yynerrs MPQCInyynerrs
#define yychar MPQCInyychar


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 28 "parse.yy"
{
  char *str;
  int i;
  std::vector<int> *nniv;
  }
/* Line 193 of yacc.c.  */
#line 227 "parse.tmp.cc"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 240 "parse.tmp.cc"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   135

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  50
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  89
/* YYNRULES -- Number of states.  */
#define YYNSTATES  168

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   304

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,     9,    10,    15,    19,    23,
      27,    32,    37,    42,    47,    52,    56,    60,    64,    68,
      72,    76,    80,    84,    88,    92,    96,   100,   104,   108,
     112,   116,   120,   122,   126,   129,   130,   134,   135,   138,
     139,   141,   143,   145,   148,   151,   152,   158,   162,   163,
     166,   167,   171,   175,   176,   179,   180,   182,   184,   188,
     189,   192,   193,   197,   201,   205,   209,   213,   217,   221,
     222,   225,   226,   228,   232,   233,   236,   237,   239,   243,
     244,   247,   248,   250,   254,   255,   258,   259,   263,   265
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      51,     0,    -1,    52,    -1,    52,    53,    -1,    -1,    -1,
       4,    21,    54,    60,    -1,    22,    21,    84,    -1,     5,
      21,    84,    -1,     6,    21,    84,    -1,     7,    21,    84,
      69,    -1,     8,    21,    84,    72,    -1,     9,    21,    84,
      75,    -1,    10,    21,    84,    78,    -1,    12,    21,    85,
      57,    -1,    13,    21,    85,    -1,    30,    21,    85,    -1,
      19,    21,    85,    -1,    20,    21,    85,    -1,    23,    21,
      84,    -1,    25,    21,    84,    -1,    24,    21,    84,    -1,
      27,    21,    84,    -1,    31,    21,    84,    -1,    35,    21,
      55,    -1,    36,    21,    55,    -1,    39,    21,    55,    -1,
      40,    21,    55,    -1,    37,    21,    55,    -1,    38,    21,
      55,    -1,    33,    21,    81,    -1,    26,    21,    84,    -1,
      84,    -1,    14,    56,    15,    -1,    56,    84,    -1,    -1,
      14,    58,    15,    -1,    -1,    58,    59,    -1,    -1,    16,
      -1,    17,    -1,    18,    -1,    66,    61,    -1,    61,    62,
      -1,    -1,    84,    84,    84,    84,    63,    -1,    14,    64,
      15,    -1,    -1,    64,    65,    -1,    -1,     6,    11,    84,
      -1,    14,    67,    15,    -1,    -1,    67,    68,    -1,    -1,
      28,    -1,    29,    -1,    14,    70,    15,    -1,    -1,    70,
      71,    -1,    -1,    41,    11,    84,    -1,    42,    11,    84,
      -1,    44,    11,    84,    -1,    45,    11,    84,    -1,    43,
      11,    84,    -1,    46,    11,    84,    -1,    14,    73,    15,
      -1,    -1,    73,    74,    -1,    -1,    34,    -1,    14,    76,
      15,    -1,    -1,    76,    77,    -1,    -1,    34,    -1,    14,
      79,    15,    -1,    -1,    79,    80,    -1,    -1,    34,    -1,
      14,    82,    15,    -1,    -1,    82,    83,    -1,    -1,    32,
      11,    84,    -1,    48,    -1,    49,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    53,    53,    56,    57,    60,    60,    62,    64,    66,
      68,    70,    72,    74,    76,    78,    80,    82,    84,    86,
      88,    90,    92,    94,    96,    98,   100,   102,   104,   106,
     108,   109,   114,   115,   120,   121,   125,   126,   130,   131,
     135,   136,   137,   140,   143,   144,   147,   152,   153,   157,
     158,   162,   166,   167,   171,   172,   176,   177,   181,   182,
     186,   187,   191,   192,   193,   194,   195,   196,   200,   201,
     205,   206,   210,   214,   215,   219,   220,   224,   228,   229,
     233,   234,   238,   242,   243,   247,   248,   252,   255,   258
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_NOT", "T_MOLECULE", "T_MULTIPLICITY",
  "T_CHARGE", "T_METHOD", "T_BASIS", "T_AUXBASIS", "T_DFBASIS", "T_EQUALS",
  "T_OPTIMIZE", "T_GRADIENT", "T_BEG_OPT", "T_END_OPT", "T_CARTESIAN",
  "T_INTERNAL", "T_REDUNDANT", "T_RESTART", "T_CHECKPOINT", "T_COLON",
  "T_SYMMETRY", "T_MEMORY", "T_TMPDIR", "T_TMPSTORE", "T_DEBUG",
  "T_ACCURACY", "T_BOHR", "T_ANGSTROM", "T_FREQUENCIES", "T_LINDEP",
  "T_MAXITER", "T_SCF", "T_UC", "T_DOCC", "T_SOCC", "T_FROZEN_DOCC",
  "T_FROZEN_UOCC", "T_ALPHA", "T_BETA", "T_XC", "T_GRID", "T_RI", "T_F12",
  "T_APP", "T_ANSATZ", "T_OO_INPUT_KEYWORD", "T_STRING", "T_BOOL",
  "$accept", "input", "assignments", "assignment", "@1",
  "nonnegative_int_vector", "nonnegative_int_sequence",
  "optimize_options_list", "optimize_options", "optimize_option",
  "molecule", "atoms", "atom", "atom_options_list", "atom_options",
  "atom_option", "molecule_options_list", "molecule_options",
  "molecule_option", "method_options_list", "method_options",
  "method_option", "basis_options_list", "basis_options", "basis_option",
  "abasis_options_list", "abasis_options", "abasis_option",
  "dbasis_options_list", "dbasis_options", "dbasis_option",
  "scf_options_list", "scf_options", "scf_option", "string", "bool", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    50,    51,    52,    52,    54,    53,    53,    53,    53,
      53,    53,    53,    53,    53,    53,    53,    53,    53,    53,
      53,    53,    53,    53,    53,    53,    53,    53,    53,    53,
      53,    53,    55,    55,    56,    56,    57,    57,    58,    58,
      59,    59,    59,    60,    61,    61,    62,    63,    63,    64,
      64,    65,    66,    66,    67,    67,    68,    68,    69,    69,
      70,    70,    71,    71,    71,    71,    71,    71,    72,    72,
      73,    73,    74,    75,    75,    76,    76,    77,    78,    78,
      79,    79,    80,    81,    81,    82,    82,    83,    84,    85
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     0,     0,     4,     3,     3,     3,
       4,     4,     4,     4,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     3,     2,     0,     3,     0,     2,     0,
       1,     1,     1,     2,     2,     0,     5,     3,     0,     2,
       0,     3,     3,     0,     2,     0,     1,     1,     3,     0,
       2,     0,     3,     3,     3,     3,     3,     3,     3,     0,
       2,     0,     1,     3,     0,     2,     0,     1,     3,     0,
       2,     0,     1,     3,     0,     2,     0,     3,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     2,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       3,     5,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      84,     0,     0,     0,     0,     0,     0,    53,    88,     8,
       9,    59,    69,    74,    79,    89,    37,    15,    17,    18,
       7,    19,    21,    20,    31,    22,    16,    23,    86,    30,
      35,    24,    32,    25,    28,    29,    26,    27,    55,     6,
      45,    61,    10,    71,    11,    76,    12,    81,    13,    39,
      14,     0,     0,     0,    43,     0,     0,     0,     0,     0,
      83,     0,    85,    33,    34,    52,    56,    57,    54,    44,
       0,    58,     0,     0,     0,     0,     0,     0,    60,    68,
      72,    70,    73,    77,    75,    78,    82,    80,    36,    40,
      41,    42,    38,     0,     0,     0,     0,     0,     0,     0,
       0,    87,     0,    62,    63,    66,    64,    65,    67,    48,
      50,    46,     0,     0,    47,    49,     0,    51
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    30,    57,    81,   102,   100,   109,   142,
      89,   104,   119,   161,   162,   165,    90,   103,   118,    92,
     105,   128,    94,   106,   131,    96,   107,   134,    98,   108,
     137,    79,   101,   112,    82,    66
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -33
static const yytype_int8 yypact[] =
{
     -33,     6,    14,   -33,   -14,   -12,     7,    11,    25,    36,
      48,    50,    52,    63,    65,    66,    72,    73,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
     -33,   -33,   -17,   -17,   -17,   -17,   -17,   -17,    46,    46,
      46,    46,   -17,   -17,   -17,   -17,   -17,   -17,    46,   -17,
      94,    -6,    -6,    -6,    -6,    -6,    -6,    95,   -33,   -33,
     -33,    96,   105,   107,   108,   -33,   109,   -33,   -33,   -33,
     -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,
     -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,
     -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,
     -33,    28,    10,     1,   -17,    20,    33,    40,    41,    74,
     -33,   113,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,
     -17,   -33,   114,   115,   116,   117,   118,   119,   -33,   -33,
     -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,
     -33,   -33,   -33,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -33,   -17,   -33,   -33,   -33,   -33,   -33,   -33,   121,
     -33,   -33,    53,   120,   -33,   -33,   -17,   -33
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -33,   -33,   -33,   -33,   -33,    27,   -33,   -33,   -33,   -33,
     -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,
     -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,
     -33,   -33,   -33,   -33,   -32,    37
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      59,    60,    61,    62,    63,    64,     3,    31,    80,    32,
      70,    71,    72,    73,    74,    75,   115,    77,     4,     5,
       6,     7,     8,     9,    10,   113,    11,    12,    33,   116,
     117,    58,    34,    13,    14,   121,    15,    16,    17,    18,
      19,    20,    58,   110,    21,    22,    35,    23,   129,    24,
      25,    26,    27,    28,    29,   132,   135,    36,    58,   163,
     111,   122,   123,   124,   125,   126,   127,   130,   164,    37,
     114,    38,   120,    39,   133,   136,    67,    68,    69,    83,
      84,    85,    86,    87,    40,    76,    41,    42,   144,   138,
     139,   140,   141,    43,    44,    65,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    78,    88,
      91,   151,   152,   153,   154,   155,   156,   157,   158,    93,
     159,    95,    97,    99,   143,   145,   146,   147,   148,   149,
     150,   166,     0,     0,   167,   160
};

static const yytype_int16 yycheck[] =
{
      32,    33,    34,    35,    36,    37,     0,    21,    14,    21,
      42,    43,    44,    45,    46,    47,    15,    49,     4,     5,
       6,     7,     8,     9,    10,    15,    12,    13,    21,    28,
      29,    48,    21,    19,    20,    15,    22,    23,    24,    25,
      26,    27,    48,    15,    30,    31,    21,    33,    15,    35,
      36,    37,    38,    39,    40,    15,    15,    21,    48,     6,
      32,    41,    42,    43,    44,    45,    46,    34,    15,    21,
     102,    21,   104,    21,    34,    34,    39,    40,    41,    52,
      53,    54,    55,    56,    21,    48,    21,    21,   120,    15,
      16,    17,    18,    21,    21,    49,    21,    21,    21,    21,
      21,    21,    21,    21,    21,    21,    21,    21,    14,    14,
      14,   143,   144,   145,   146,   147,   148,   149,   150,    14,
     152,    14,    14,    14,    11,    11,    11,    11,    11,    11,
      11,    11,    -1,    -1,   166,    14
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    51,    52,     0,     4,     5,     6,     7,     8,     9,
      10,    12,    13,    19,    20,    22,    23,    24,    25,    26,
      27,    30,    31,    33,    35,    36,    37,    38,    39,    40,
      53,    21,    21,    21,    21,    21,    21,    21,    21,    21,
      21,    21,    21,    21,    21,    21,    21,    21,    21,    21,
      21,    21,    21,    21,    21,    21,    21,    54,    48,    84,
      84,    84,    84,    84,    84,    49,    85,    85,    85,    85,
      84,    84,    84,    84,    84,    84,    85,    84,    14,    81,
      14,    55,    84,    55,    55,    55,    55,    55,    14,    60,
      66,    14,    69,    14,    72,    14,    75,    14,    78,    14,
      57,    82,    56,    67,    61,    70,    73,    76,    79,    58,
      15,    32,    83,    15,    84,    15,    28,    29,    68,    62,
      84,    15,    41,    42,    43,    44,    45,    46,    71,    15,
      34,    74,    15,    34,    77,    15,    34,    80,    15,    16,
      17,    18,    59,    11,    84,    11,    11,    11,    11,    11,
      11,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      14,    63,    64,     6,    15,    65,    11,    84
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      MPQCInylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus

#else

#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus

#else

#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE MPQCInylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &MPQCInylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &MPQCInylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = MPQCInylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:
#line 60 "parse.yy"
    { begin_molecule(); ;}
    break;

  case 6:
#line 61 "parse.yy"
    { end_molecule(); ;}
    break;

  case 7:
#line 63 "parse.yy"
    { set_symmetry((yyvsp[(3) - (3)].str)); ;}
    break;

  case 8:
#line 65 "parse.yy"
    { set_multiplicity((yyvsp[(3) - (3)].str)); ;}
    break;

  case 9:
#line 67 "parse.yy"
    { set_charge((yyvsp[(3) - (3)].str)); ;}
    break;

  case 10:
#line 69 "parse.yy"
    { set_method((yyvsp[(3) - (4)].str)); ;}
    break;

  case 11:
#line 71 "parse.yy"
    { basis_.set_name((yyvsp[(3) - (4)].str)); ;}
    break;

  case 12:
#line 73 "parse.yy"
    { auxbasis_.set_name((yyvsp[(3) - (4)].str)); ;}
    break;

  case 13:
#line 75 "parse.yy"
    { dfbasis_.set_name((yyvsp[(3) - (4)].str)); ;}
    break;

  case 14:
#line 77 "parse.yy"
    { set_optimize((yyvsp[(3) - (4)].i)); ;}
    break;

  case 15:
#line 79 "parse.yy"
    { set_gradient((yyvsp[(3) - (3)].i)); ;}
    break;

  case 16:
#line 81 "parse.yy"
    { set_frequencies((yyvsp[(3) - (3)].i)); ;}
    break;

  case 17:
#line 83 "parse.yy"
    { set_restart((yyvsp[(3) - (3)].i)); ;}
    break;

  case 18:
#line 85 "parse.yy"
    { set_checkpoint((yyvsp[(3) - (3)].i)); ;}
    break;

  case 19:
#line 87 "parse.yy"
    { set_memory((yyvsp[(3) - (3)].str)); ;}
    break;

  case 20:
#line 89 "parse.yy"
    { set_tmpstore((yyvsp[(3) - (3)].str)); ;}
    break;

  case 21:
#line 91 "parse.yy"
    { set_tmpdir((yyvsp[(3) - (3)].str)); ;}
    break;

  case 22:
#line 93 "parse.yy"
    { set_accuracy((yyvsp[(3) - (3)].str)); ;}
    break;

  case 23:
#line 95 "parse.yy"
    { set_lindep((yyvsp[(3) - (3)].str)); ;}
    break;

  case 24:
#line 97 "parse.yy"
    { set_docc((yyvsp[(3) - (3)].nniv)); ;}
    break;

  case 25:
#line 99 "parse.yy"
    { set_socc((yyvsp[(3) - (3)].nniv)); ;}
    break;

  case 26:
#line 101 "parse.yy"
    { set_alpha((yyvsp[(3) - (3)].nniv)); ;}
    break;

  case 27:
#line 103 "parse.yy"
    { set_beta((yyvsp[(3) - (3)].nniv)); ;}
    break;

  case 28:
#line 105 "parse.yy"
    { set_frozen_docc((yyvsp[(3) - (3)].nniv)); ;}
    break;

  case 29:
#line 107 "parse.yy"
    { set_frozen_uocc((yyvsp[(3) - (3)].nniv)); ;}
    break;

  case 31:
#line 110 "parse.yy"
    { set_debug((yyvsp[(3) - (3)].str)); ;}
    break;

  case 32:
#line 114 "parse.yy"
    { (yyval.nniv) = make_nnivec(0,(yyvsp[(1) - (1)].str)); ;}
    break;

  case 33:
#line 116 "parse.yy"
    { (yyval.nniv) = (yyvsp[(2) - (3)].nniv); ;}
    break;

  case 34:
#line 120 "parse.yy"
    { (yyval.nniv) = make_nnivec((yyvsp[(1) - (2)].nniv),(yyvsp[(2) - (2)].str)); ;}
    break;

  case 35:
#line 121 "parse.yy"
    { (yyval.nniv) = make_nnivec(0,0); ;}
    break;

  case 40:
#line 135 "parse.yy"
    { set_opt_type(T_CARTESIAN); ;}
    break;

  case 41:
#line 136 "parse.yy"
    { set_opt_type(T_INTERNAL); ;}
    break;

  case 42:
#line 137 "parse.yy"
    { set_redund_coor(1); ;}
    break;

  case 46:
#line 148 "parse.yy"
    { add_atom((yyvsp[(1) - (5)].str),(yyvsp[(2) - (5)].str),(yyvsp[(3) - (5)].str),(yyvsp[(4) - (5)].str)); ;}
    break;

  case 51:
#line 162 "parse.yy"
    { set_atom_charge((yyvsp[(3) - (3)].str)); ;}
    break;

  case 56:
#line 176 "parse.yy"
    { set_molecule_bohr(1); ;}
    break;

  case 57:
#line 177 "parse.yy"
    { set_molecule_bohr(0); ;}
    break;

  case 62:
#line 191 "parse.yy"
    { set_dftmethod_xc((yyvsp[(3) - (3)].str)); ;}
    break;

  case 63:
#line 192 "parse.yy"
    { set_dftmethod_grid((yyvsp[(3) - (3)].str)); ;}
    break;

  case 64:
#line 193 "parse.yy"
    { set_r12method_f12((yyvsp[(3) - (3)].str)); ;}
    break;

  case 65:
#line 194 "parse.yy"
    { set_r12method_app((yyvsp[(3) - (3)].str)); ;}
    break;

  case 66:
#line 195 "parse.yy"
    { set_r12method_ri((yyvsp[(3) - (3)].str)); ;}
    break;

  case 67:
#line 196 "parse.yy"
    { set_r12method_ansatz((yyvsp[(3) - (3)].str)); ;}
    break;

  case 72:
#line 210 "parse.yy"
    { basis_.set_uc(true); ;}
    break;

  case 77:
#line 224 "parse.yy"
    { auxbasis_.set_uc(true); ;}
    break;

  case 82:
#line 238 "parse.yy"
    { dfbasis_.set_uc(true); ;}
    break;

  case 87:
#line 252 "parse.yy"
    { set_scf_maxiter((yyvsp[(3) - (3)].str)); ;}
    break;

  case 88:
#line 255 "parse.yy"
    { (yyval.str) = (yyvsp[(1) - (1)].str); ;}
    break;

  case 89:
#line 258 "parse.yy"
    { (yyval.i) = (yyvsp[(1) - (1)].i); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 1828 "parse.tmp.cc"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &MPQCInylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = MPQCInylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &MPQCInylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 261 "parse.yy"


