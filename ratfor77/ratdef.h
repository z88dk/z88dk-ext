#define ACCENT  96
#define AND     38
#define APPEND
#define ATSIGN  64
#define BACKSLASH       92
#define BACKSPACE       8
#define BANG    33
#define BAR     124
#define BIGA    65
#define BIGB    66
#define BIGC    67
#define BIGD    68
#define BIGE    69
#define BIGF    70
#define BIGG    71
#define BIGH    72
#define BIGI    73
#define BIGJ    74
#define BIGK    75
#define BIGL    76
#define BIGM    77
#define BIGN    78
#define BIGO    79
#define BIGP    80
#define BIGQ    81
#define BIGR    82
#define BIGS    83
#define BIGT    84
#define BIGU    85
#define BIGV    86
#define BIGW    87
#define BIGX    88
#define BIGY    89
#define BIGZ    90
#define BLANK   32
#define CARET   94
#define COLON   58
#define COMMA   44
#define CRLF    13
#define DIG0    48
#define DIG1    49
#define DIG2    50
#define DIG3    51
#define DIG4    52
#define DIG5    53
#define DIG6    54
#define DIG7    55
#define DIG8    56
#define DIG9    57
#define DOLLAR  36
#define DQUOTE  34
#define EOS     0
#define EQUALS  61
#define ESCAPE  ATSIGN
#define GREATER 62
#define HUGE    30000
#define LBRACE  123
#define LBRACK  91
#define LESS    60
#define LETA    97
#define LETB    98
#define LETC    99
#define LETD    100
#define LETE    101
#define LETF    102
#define LETG    103
#define LETH    104
#define LETI    105
#define LETJ    106
#define LETK    107
#define LETL    108
#define LETM    109
#define LETN    110
#define LETO    111
#define LETP    112
#define LETQ    113
#define LETR    114
#define LETS    115
#define LETT    116
#define LETU    117
#define LETV    118
#define LETW    119
#define LETX    120
#define LETY    121
#define LETZ    122
#define LPAREN  40
#define MINUS   45
#define NEWLINE 10
#define NO      0
#define NOT     126
#define OR      BAR	/* same as | */
#define PERCENT 37
#define PERIOD  46
#define PLUS    43
#define QMARK   63
#define RBRACE  125
#define RBRACK  93
#define RPAREN  41
#define SEMICOL 59
#define SHARP   35
#define SLASH   47
#define SQUOTE  39
#define STAR    42
#define TAB     9
#define TILDE   126
#define UNDERLINE       95
#define YES     1

#define LIMIT   134217728
#define LIM1    28
#define LIM2    -28

/*
 * lexical analyser symbols
 *
 */

#define LETTER		1
#define DIGIT   	2
#define ALPHA   	3
#define LEXBREAK   	4
#define LEXDIGITS   	5
#define LEXDO   	6
#define LEXELSE   	7
#define LEXFOR   	8
#define LEXIF   	9
#define LEXNEXT   	10
#define LEXOTHER   	11
#define LEXREPEAT   	12
#define LEXUNTIL   	13
#define LEXWHILE   	14
#define LEXRETURN   	15
#define LEXEND   	16
#define LEXSTOP   	17
#define LEXSTRING   	18
#define LEXSWITCH	19
#define LEXCASE		20
#define LEXDEFAULT	21
#define DEFTYPE   	22

#define MAXCHARS   	10   	/* characters for outnum */
#define MAXDEF   	200   	/* max chars in a defn */
#define MAXSWITCH       300     /* max stack for switch statement */
#define CUTOFF          3       /* min number of cases necessary to generate
*/
                                /* a dispatch table */
#define DENSITY         2
#define MAXFORSTK   	200   	/* max space for for reinit clauses */
#define MAXFNAMES   	350  	/* max chars in filename stack NFILES*MAXNAME */
#define MAXNAME   	64   	/* file name size in gettok */
#define MAXSTACK   	100   	/* max stack depth for parser */
#define MAXTBL   	15000   /* max chars in all definitions */
#define MAXTOK   	132   	/* max chars in a token */
#define NFILES   	7   	/* max depth of file inclusion */

#define RADIX   	PERCENT /* % indicates alternate radix */
#define BUFSIZE   	300   	/* pushback buffer for ngetch and putbak */

