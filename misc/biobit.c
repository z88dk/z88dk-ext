/*
 * BIO-BIT - I/O control language
 * for 8bit IN + 8bit OUT digital interfaces
 *
 * By Stefano Bodrato, 2026
 *
 * Build (z88dk):
 *   zcc +zx -lndos -create-app -clib=ansi -pragma-define:ansicolumns=51 -Cz--extreme -zorg=28000 -o biobit biobit.c
 *   zcc +zx81 -lndos -create-app -o biobit biobit.c    (32K only, to load: POKE 16389,255 | NEW | LOAD "")
 *
 * Minimal BASIC-like line editor + micro interpreter for bitwise I/O logic.
 * Target: z88dk-friendly ANSI C, no dynamic allocation.
 *
 * Program model:
 * - Lines are stored as (line_number, text).
 * - Supported statements: "O<bit> = <expr>"
 * - <expr> supports: I<n>, O<n>, 0, 1, with operators: AND, OR, XOR, NOT
 * - Evaluation reads input port ONCE per statement (to avoid tearing).
 * - Output latch is preserved across statements; only targeted bit is updated.
 */

#include <stdio.h>
#include <stdlib.h>     /* inp(), outp() */
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#ifdef __SPECTRUM__
#include <spectrum.h>   /* zx_break()*/
#endif
#ifdef __ZX81__
#include <zx81.h>   /* zx_break()*/
#endif
#include <time.h>       /* clock(), time() */
#include <conio.h>      /* gotoxy() */
#include <graphics.h>


#define MAX_LINES     200
#define MAX_LINE_LEN  50

typedef struct {
    int16_t number;                 /* line number, -1 means empty slot */
    char    text[MAX_LINE_LEN];     /* raw statement text after the number */
} ProgLine;

static ProgLine program_mem[MAX_LINES+1];
static int   line_count = 0;

static int   flags = 0;
#define F_MONITOR  1

static uint8_t mem_reg = 0x00;   /* M0..M7 */


/* Software latches for simulation and for preserving last output. */
static uint8_t input_addr  = 251;
static uint8_t input_latch = 0x00;
static uint8_t output_addr = 251;
static uint8_t output_latch= 0x00;


// ---- Timers A..D (index 0..3) ----
static clock_t t_start[4] = {0,0,0,0};
static clock_t t_duration[4] = {0,0,0,0}; // in clock ticks (CLOCKS_PER_SEC based)


/* visual bit string */
char bbuf[9];
static const char* byte_to_bin(uint8_t v) {
    for (int i = 7; i >= 0; --i) {
        bbuf[7 - i] = (char)('0' + ((v >> i) & 1));
    }
    bbuf[8] = '\0';
    return bbuf;
}


/* -------------------- Graphics symbols-------------------- */
#ifdef GRAPHICS
char gfx_blank[] = { 
        6,6,
        0xFC,0xFC,0xFC,0xFC,0xFC,0xFC
};
char gfx_bits[] = { 
        4,4,
        0x60,0x90,0x90,0x60,
        4,4,
        0x60,0xF0,0xF0,0x60
};


static void monitor_panel() {
    for (int p = 7; p >= 0; --p) {
      putsprite (spr_and, 205+p*6, 4,  gfx_blank);  //blank
      putsprite (spr_or, 205+p*6+1, 5,  gfx_bits+(6*(bbuf[p]-'0')));
    }
}
#endif



/* -------------------- Hardware I/O -------------------- */
static uint8_t read_input_port(void) {
    input_latch = inp(input_addr);
    if (flags & F_MONITOR) {
        byte_to_bin(input_latch);
//#ifdef GRAPHICS
//        for (int i = 7; i >= 0; --i) {
//          putsprite (spr_and, 205+i*6, 2,  gfx_blank);  //blank
//          putsprite (spr_or, 205+i*6+1, 2,  gfx_bits+(6*(bbuf[i]-'0')));
//        }
//#else
//        printf("[%s]\n", bbuf);
//#endif
    }
    return input_latch;
}
static void write_output_port(uint8_t v) {
    output_latch = v;                     /* retain last written value */
    outp(output_addr, output_latch);
    if (flags & F_MONITOR) {
        byte_to_bin(v);
#ifdef GRAPHICS
        monitor_panel();
#else
        printf("[%s]\n", bbuf);
#endif
    }
}

/* Update one output bit while preserving the others. */
static void set_output_bit(uint8_t bit, uint8_t val) {
    if (bit > 7) return;
    uint8_t mask = (uint8_t)(1u << bit);
    uint8_t outv = output_latch;
    if (val) outv |= mask;
    else     outv &= (uint8_t)~mask;
    write_output_port(outv);
}


/* -------------------- Utils -------------------- */

static void set_memory_bit(uint8_t bit, uint8_t val) {
    if (bit > 7) return;
    uint8_t mask = (uint8_t)(1u << bit);
    if (val) mem_reg |= mask;
    else     mem_reg &= (uint8_t)~mask;
}


// Convert milliseconds to clock ticks using CLOCKS_PER_SEC.
static clock_t ms_to_ticks(long ms) {
    if (ms <= 0) return 0;
    // Avoid overflow by doing (ms * CLOCKS_PER_SEC) / 1000 with 64-bit widen
    long ticks = (long)(( (long long)ms * (long long)CLOCKS_PER_SEC ) / 1000LL);
    if (ticks < 0) ticks = 0;
    return (clock_t)ticks;
}

// Check if timer idx (0..3) is still running (not expired).
static int timer_active(int idx) {
    clock_t dur = t_duration[idx];
    if (dur == 0) return 0; // not configured -> treat as expired
    clock_t now = clock();
    // Safe even on wrap-around due to unsigned arithmetic semantics of clock_t on most libs,
    // but we keep it simple for this target.
    return (clock_t)(now - t_start[idx]) < dur;
}

static void trim(char *s) {
    char *p = s;
    while (*p && isspace((unsigned char)*p)) p++;
    if (p != s) memmove(s, p, strlen(p) + 1);
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) {
        s[n - 1] = '\0';
        n--;
    }
}
static void str_to_upper(char *s) {
    for (; *s; ++s) *s = (char)toupper((unsigned char)*s);
}
/* Safe copy with NUL termination */
static void safe_copy(char *dst, const char *src, size_t maxlen) {
    if (maxlen == 0) return;
    strncpy(dst, src, maxlen - 1);
    dst[maxlen - 1] = '\0';
}

/* -------------------- Line editor storage -------------------- */
static int find_line_index(int16_t number) {
    for (int i = 0; i < line_count; ++i) {
        if (program_mem[i].number == number) return i;
    }
    return -1;
}
static void insert_or_replace_line(int16_t number, const char *text) {
    int idx = find_line_index(number);
    if (idx >= 0) {
        /* Replace */
        safe_copy(program_mem[idx].text, text, MAX_LINE_LEN);
        return;
    }
    if (line_count >= MAX_LINES) {
        printf("ERR: program full.\n");
        return;
    }
    /* Insert sorted by line number */
    int pos = line_count;
    for (int i = 0; i < line_count; ++i) {
        if (number < program_mem[i].number) { pos = i; break; }
    }
    /* Shift down */
    for (int j = line_count; j > pos; --j) {
        program_mem[j] = program_mem[j - 1];
    }
    program_mem[pos].number = number;
    safe_copy(program_mem[pos].text, text, MAX_LINE_LEN);
    line_count++;
}
static void delete_line(int16_t number) {
    int idx = find_line_index(number);
    if (idx < 0) return;
    for (int i = idx; i < line_count - 1; ++i) {
        program_mem[i] = program_mem[i + 1];
    }
    line_count--;
}

/* -------------------- Parser / Evaluator -------------------- */
/* Simple tokenizer helpers on an in-place mutable cursor */
typedef struct {
    const char *start;
    const char *cur;
} Scanner;

static void sc_init(Scanner *sc, const char *s) { sc->start = s; sc->cur = s; }
static void sc_skip_ws(Scanner *sc) { while (*sc->cur && isspace((unsigned char)*sc->cur)) sc->cur++; }

// Get 8 '0'/'1' characters to retrieve a byte value,
// return '-1' on error
static int sc_read_bin8(Scanner *sc) {
    const char *p = sc->cur;
    uint8_t v = 0;
    // salta spazi
    while (*p && isspace((unsigned char)*p)) p++;
    // ust be exactly 8 binary digits
    for (int i = 0; i < 8; ++i) {
        char c = p[i];
        if (c != '0' && c != '1') {
            return -1; // Not a valid binary sequence
        }
        v = (uint8_t)((v << 1) | (uint8_t)(c - '0'));
    }
    // the number must finish here
    char next = p[8];
    if (isdigit((unsigned char)next)) {
        return -1;
    }
    // OK: eat characters and provide the value
    sc->cur = p + 8;
    return v;
}

// Read a seconds value with optional fractional part (e.g., "2.6", "0.250")
// and convert it to milliseconds (integer). Returns 1 on success, 0 on failure.
// No FP math at runtime: we parse digits and scale to ms (3 decimals).
static int sc_read_seconds_as_ms(Scanner *sc, long *out_ms) {
    sc_skip_ws(sc);
    const char *p = sc->cur;

    // Must start with a digit
    if (!isdigit((unsigned char)*p)) return 0;

    long sec = 0;
    while (isdigit((unsigned char)*p)) { sec = sec * 10 + (*p - '0'); p++; }

    long ms_frac = 0;    // 0..999
    int seen_dot = 0;
    if (*p == '.') {
        seen_dot = 1;
        p++;
        int dig = 0;
        // capture up to 3 fractional digits
        while (dig < 3 && isdigit((unsigned char)*p)) {
            ms_frac = ms_frac * 10 + (*p - '0');
            p++; dig++;
        }
        // if fewer than 3 digits, scale (e.g., ".2" -> 200ms, ".25" -> 250ms)
        while (dig < 3) { ms_frac *= 10; dig++; }
    }

    sc->cur = p;
    // sanity: avoid overflow at absurd values
    if (sec > 2147483L) sec = 2147483L; // ~24 days in seconds
    long total_ms = sec * 1000L + (seen_dot ? ms_frac : 0L);
    if (total_ms < 0) total_ms = 0;
    *out_ms = total_ms;
    return 1;
}

static int  sc_match_char_ci(Scanner *sc, char ch) {
    sc_skip_ws(sc);
    char c = *sc->cur;
    if (toupper((unsigned char)c) == toupper((unsigned char)ch)) { sc->cur++; return 1; }
    return 0;
}
/* Match keyword case-insensitively with token boundary (non-alnum after kw) */
static int  sc_match_kw(Scanner *sc, const char *kw) {
    sc_skip_ws(sc);
    size_t n = strlen(kw);
    for (size_t i = 0; i < n; ++i) {
        char c1 = (char)toupper((unsigned char)sc->cur[i]);
        char c2 = (char)toupper((unsigned char)kw[i]);
        if (c1 != c2) return 0;
    }
    char next = sc->cur[n];
    if (isalnum((unsigned char)next)) return 0;
    sc->cur += n;
    return 1;
}
static int  sc_consume_char(Scanner *sc, char ch) {
    sc_skip_ws(sc);
    if (*sc->cur == ch) { sc->cur++; return 1; }
    return 0;
}
static int  sc_read_uint(Scanner *sc, int *out) {
    sc_skip_ws(sc);
    if (!isdigit((unsigned char)*sc->cur)) return 0;
    int val = 0;
    while (isdigit((unsigned char)*sc->cur)) {
        val = val * 10 + (*sc->cur - '0');
        sc->cur++;
    }
    *out = val;
    return 1;
}

/* Parser for statements with a single argument; returns:
 *  >0 -> parsed ok, writes *target
 *   0 -> Command not found (caller may parse other statements)
 *  -1 -> syntax error
 */
static int parse_stmt_one_arg(Scanner *sc, int *target, const char *stmt) {
    sc_skip_ws(sc);
    if (!sc_match_kw(sc, stmt)) return 0;
    int ln = -1;
    if (!sc_read_uint(sc, &ln)) return -1;
    sc_skip_ws(sc);
    if (*sc->cur != '\0') return -1;
    *target = ln;
    return 1;
}


// Try to match a specific bit-source designator and its bit number.
// - expected: 'I', 'O' or 'M' (case-insensitive)
// - On success: returns 1 and writes the parsed bit (0..7) to *out_bit.
// - On failure: returns 0 and leaves scanner in a consistent state.
static int sc_match_bit_index(Scanner *sc, char expected) {
    // Attempt to match the expected designator (case-insensitive)
    if (!sc_match_char_ci(sc, expected)) return -1;

    // Read the bit number (must be 0..7)
    int bit = -1;
    if (!sc_read_uint(sc, &bit) || bit < 0 || bit > 7) return -1;

    return bit;
}


/* Expr grammar (boolean, returns 0/1):
 * expr  -> term { (AND | OR | XOR) term }*
 * term  -> NOT? factor
 * factor-> I<number> | O<number> | 0 | 1
 * All operators are left-associative. No parentheses for simplicity.
 * Input byte is captured once before evaluating the expression.
 */

static uint8_t eval_factor(Scanner *sc, uint8_t in_byte, int *ok) {
    sc_skip_ws(sc);

    int bit;
    // I<n>
    if ((bit = sc_match_bit_index(sc, 'I')) >= 0)
        return (uint8_t)((in_byte >> bit) & 1);

    // O<n> (read from output latch)
    if ((bit = sc_match_bit_index(sc, 'O')) >= 0)
        return (uint8_t)((output_latch >> bit) & 1);

    // M<n> (read from internal memory register)
    if ((bit = sc_match_bit_index(sc, 'M')) >= 0)
        return (uint8_t)((mem_reg >> bit) & 1);

    // T<n> (timer), read a specific bit from clock()
    if ((bit = sc_match_bit_index(sc, 'T')) >= 0) {
        uint8_t t = (uint8_t)(clock() & 0xFF);
        return (uint8_t)((t >> bit) & 1);
    }


    // Constant 0 or 1
    if (isdigit((unsigned char)*sc->cur)) {
        int v = -1;
        if (!sc_read_uint(sc, &v) || !(v == 0 || v == 1)) { *ok = 0; return 0; }
        return (uint8_t)(v & 1);
    }

    *ok = 0;
    return 0;
}


static uint8_t eval_term(Scanner *sc, uint8_t in_byte, int *ok) {
    int neg = 0;
    if (sc_match_kw(sc, "NOT")) neg = 1;
    uint8_t v = eval_factor(sc, in_byte, ok);
    if (!*ok) return 0;
    return (uint8_t)(neg ? (v ? 0 : 1) : v);
}

static uint8_t eval_expr(Scanner *sc, uint8_t in_byte, int *ok) {
    uint8_t acc = eval_term(sc, in_byte, ok);
    if (!*ok) return 0;
    for (;;) {
        if (sc_match_kw(sc, "AND")) {
            uint8_t rhs = eval_term(sc, in_byte, ok);
            if (!*ok) return 0;
            acc = (uint8_t)((acc & rhs) ? 1 : 0);
        } else if (sc_match_kw(sc, "OR")) {
            uint8_t rhs = eval_term(sc, in_byte, ok);
            if (!*ok) return 0;
            acc = (uint8_t)((acc | rhs) ? 1 : 0);
        } else if (sc_match_kw(sc, "XOR")) {
            uint8_t rhs = eval_term(sc, in_byte, ok);
            if (!*ok) return 0;
            acc = (uint8_t)((acc ^ rhs) ? 1 : 0);
        } else {
            break;
        }
    }
    return (uint8_t)(acc ? 1 : 0);
}

// Evaluate n-ary prefix boolean functions: AND/OR/XOR/NAND/NOR/XNOR and unary NOT.
// Syntax after '=':  FUNC term { term }*
// Returns via *ok; on error *ok=0.
static uint8_t eval_prefix_expr(Scanner *sc, uint8_t in_byte, int *ok) {
    sc_skip_ws(sc);

    // Detect function keyword
    enum { F_NONE, F_NOT, F_AND, F_OR, F_XOR, F_NAND, F_NOR, F_XNOR } fn = F_NONE;

    if (sc_match_kw(sc, "AND"))  fn = F_AND;
    else if (sc_match_kw(sc, "OR"))   fn = F_OR;
    else if (sc_match_kw(sc, "XOR"))  fn = F_XOR;
    else if (sc_match_kw(sc, "NAND")) fn = F_NAND;
    else if (sc_match_kw(sc, "NOR"))  fn = F_NOR;
    else if (sc_match_kw(sc, "XNOR")) fn = F_XNOR;
    else {
        *ok = 0; // not a prefix form
        return 0;
    }

    // Read at least one term for NOT, at least two for n-ary
    uint8_t acc = 0;
    int term_count = 0;

    // Read first two terms
    uint8_t t1 = eval_term(sc, in_byte, ok);
    if (!*ok) return 0;
    term_count++;
    uint8_t t2 = eval_term(sc, in_byte, ok);
    if (!*ok) return 0;
    term_count++;

    // Initialize accumulator according to op
    switch (fn) {
        case F_AND:  acc = (uint8_t)((t1 & t2) ? 1 : 0); break;
        case F_OR:   acc = (uint8_t)((t1 | t2) ? 1 : 0); break;
        case F_XOR:  acc = (uint8_t)((t1 ^ t2) ? 1 : 0); break;
        case F_NAND: acc = (uint8_t)(!((t1 & t2) ? 1 : 0)); break;
        case F_NOR:  acc = (uint8_t)(!((t1 | t2) ? 1 : 0)); break;
        case F_XNOR: acc = (uint8_t)(!((t1 ^ t2) ? 1 : 0)); break;
        default: *ok = 0; return 0;
    }

    // Consume remaining terms, if any
    for (;;) {
        uint8_t v;
        const char *save = sc->cur;
        v = eval_term(sc, in_byte, ok);
        if (!*ok) { // if we failed because there is no more term, restore and finish
            *ok = 1;
            sc->cur = save;
            break;
        }
        term_count++;
        switch (fn) {
            case F_AND:  acc = (uint8_t)((acc & v) ? 1 : 0); break;
            case F_OR:   acc = (uint8_t)((acc | v) ? 1 : 0); break;
            case F_XOR:  acc = (uint8_t)((acc ^ v) ? 1 : 0); break;
            case F_NAND: acc = (uint8_t)(!((acc & v) ? 1 : 0)); break;
            case F_NOR:  acc = (uint8_t)(!((acc | v) ? 1 : 0)); break;
            case F_XNOR: acc = (uint8_t)(!((acc ^ v) ? 1 : 0)); break;
            default: *ok = 0; return 0;
        }
    }

    sc_skip_ws(sc);
    if (*sc->cur != '\0') { *ok = 0; return 0; }

    // We enforced >=2 terms above
    (void)term_count;
    return (uint8_t)(acc ? 1 : 0);
}


/* Interpreting Statements */
static int exec_statement(const char *stmt_raw, int *out_goto) {
    /* Work on an uppercase copy (parser is case-insensitive) */
    char buf[MAX_LINE_LEN];
    safe_copy(buf, stmt_raw, sizeof(buf));
    trim(buf);
    str_to_upper(buf);

    Scanner sc;
    sc_init(&sc, buf);

    // ---- ATIME/BTIME/CTIME/DTIME <seconds> ----
    if (sc_match_kw(&sc, "ATIME") || sc_match_kw(&sc, "BTIME") ||
        sc_match_kw(&sc, "CTIME") || sc_match_kw(&sc, "DTIME")) {

        // Determine which timer we matched: A=0,B=1,C=2,D=3
        int t_idx = -1;
        // sc.cur now points just after the matched keyword. We can detect by stepping back.
        // Easier: re-scan from buf head for the first char (A..D)
        {
            // Find which keyword by reading the first letter from the buffer copy
            const char *kwp = buf;
            while (*kwp && isspace((unsigned char)*kwp)) kwp++;
            // kwp should be 'A'/'B'/'C'/'D' of ATIME/BTIME/CTIME/DTIME
            if      (kwp[0]=='A') t_idx = 0;
            else if (kwp[0]=='B') t_idx = 1;
            else if (kwp[0]=='C') t_idx = 2;
            else                   t_idx = 3; // 'D'
        }

        long ms = -1;
        if (!sc_read_seconds_as_ms(&sc, &ms) || ms < 0) {
            printf("ERR: *TIME requires seconds (e.g., 2 or 2.6)\n");
            return 0;
        }
        sc_skip_ws(&sc);
        if (*sc.cur != '\0') { printf("ERR: trailing garbage after *TIME\n"); return 0; }

        t_duration[t_idx] = ms_to_ticks(ms);
        t_start[t_idx] = clock(); // (re)start now
        return 1;
    }

    // ---- ALOOP/BLOOP/CLOOP/DLOOP <line> ----
    if (sc_match_kw(&sc, "ALOOP") || sc_match_kw(&sc, "BLOOP") ||
        sc_match_kw(&sc, "CLOOP") || sc_match_kw(&sc, "DLOOP")) {

        int t_idx = -1;
        {
            const char *kwp = buf;
            while (*kwp && isspace((unsigned char)*kwp)) kwp++;
            // kwp should be 'A'/'B'/'C'/'D' of ALOOP..DLOOP
            if      (kwp[0]=='A') t_idx = 0;
            else if (kwp[0]=='B') t_idx = 1;
            else if (kwp[0]=='C') t_idx = 2;
            else                   t_idx = 3; // 'D'
        }

        int ln = -1;
        if (!sc_read_uint(&sc, &ln)) {
            printf("ERR: *LOOP requires line number\n");
            return 0;
        }
        sc_skip_ws(&sc);
        if (*sc.cur != '\0') { printf("ERR: trailing garbage after *LOOP\n"); return 0; }

        if (timer_active(t_idx)) {
            *out_goto = ln;  // loop until timer expires
        }
        return 1;
    }

    /* --- GOTO with optional conditional bit test, or its negation (!Xn) --- */
    if (sc_match_kw(&sc, "GOTO")) {
        int target = -1;
        if (!sc_read_uint(&sc, &target)) {
            printf("ERR: expected line number after GOTO\n");
            return 0;
        }

        sc_skip_ws(&sc);

        /* No condition → plain jump */
        if (*sc.cur == '\0') {
            *out_goto = target;
            return 1;
        }

        /* Optional NOT prefix: !I<n> / !O<n> / !M<n> / !T<n> */
        int negate = 0;
        if (*sc.cur == '!') {
            negate = 1;
            sc.cur++;
            sc_skip_ws(&sc);
        }

        /* Next must be I/O/M/T */
        char typ = *sc.cur;
        if (typ=='I' || typ=='O' || typ=='M' || typ=='T') {
            sc.cur++; /* consume designator */

            int bit = -1;
            if (!sc_read_uint(&sc, &bit) || bit < 0 || bit > 7) {
                printf("ERR: bad bit after GOTO\n");
                return 0;
            }

            sc_skip_ws(&sc);
            if (*sc.cur != '\0') {
                printf("ERR: trailing garbage after GOTO condition\n");
                return 0;
            }

            /* Evaluate single bit */
            uint8_t inb = read_input_port();
            uint8_t v = 0;

            switch (typ) {
                case 'I': v = (inb >> bit) & 1; break;
                case 'O': v = (output_latch >> bit) & 1; break;
                case 'M': v = (mem_reg >> bit) & 1; break;
                case 'T': v = ((uint8_t)(clock() & 0xFF) >> bit) & 1; break;
            }

            if (negate) v = v ? 0 : 1;

            *out_goto = v ? target : -1;
            return 1;
        }

        printf("ERR: expected I<n>, O<n>, M<n> or T<n> after GOTO\n");
        return 0;
    }

    /* --- PIN <address> as runtime statement --- */
    if (sc_match_kw(&sc, "PIN")) {
        int addr = -1;
        if (!sc_read_uint(&sc, &addr) || addr < 0 || addr > 255) {
            printf("ERR: PIN requires 0..255\n");
            return 0;
        }
        sc_skip_ws(&sc);
        if (*sc.cur != '\0') {
            printf("ERR: trailing garbage after PIN\n");
            return 0;
        }
        input_addr = (uint8_t)addr;   /* change only the address */
        return 1;
    }


    /* Recognize O or M */

    int target_is_output = 0;

    if (sc_match_char_ci(&sc, 'O')) {
        target_is_output = 1;
    } else if (!sc_match_char_ci(&sc, 'M')) {
        printf("ERR: expected O or M assignment\n");
        return 0;
    }

    /* whole-register assignment form (Syntax:  M/O = (I|O|M|T|0..255) */
    sc_skip_ws(&sc);
    if (sc_consume_char(&sc, '=')) {
        uint8_t rhs = 0;
        /* parse RHS source */
        sc_skip_ws(&sc);

        uint8_t src = target_is_output ? output_latch : mem_reg;

        /* whole-register assignment: O = ..., M = ... */
        if (sc_match_char_ci(&sc, 'I')) {
            rhs = read_input_port();
        }
        else if (sc_match_char_ci(&sc, 'O')) {
            rhs = output_latch;
        }
        else if (sc_match_char_ci(&sc, 'M')) {
            rhs = mem_reg;
        }
        else if (sc_match_char_ci(&sc, 'T')) {
            rhs = (uint8_t)(clock() & 0xFF);
        }
        /* ---- Byte-level ops ---- */

        else if (sc_match_kw(&sc, "NEG")) {
            rhs = (uint8_t)(~src);
        }
        else if (sc_match_kw(&sc, "RL")) {
            rhs = (uint8_t)((src << 1) | (src >> 7));
        }
        else if (sc_match_kw(&sc, "RR")) {
            rhs = (uint8_t)((src >> 1) | (src << 7));
        }
        else if (sc_match_kw(&sc, "REV")) {
            uint8_t b = src;
            b = (uint8_t)(((b & 0xF0) >> 4) | ((b & 0x0F) << 4));
            b = (uint8_t)(((b & 0xCC) >> 2) | ((b & 0x33) << 2));
            b = (uint8_t)(((b & 0xAA) >> 1) | ((b & 0x55) << 1));
            rhs = b;
        }   /* binary literal */
        else {
            int v;
            if ((v = sc_read_bin8(&sc)) < 0) {
                printf("ERR: expected I/O/M/T, RL/RR/INV/REV or 'xxxxxxxx' after '='\n");
                return 0;
            }
            rhs = (uint8_t)v;
        }

        sc_skip_ws(&sc);
        if (*sc.cur != '\0') {
            printf("ERR: unexpected trailing characters\n");
            return 0;
        }
        if (target_is_output) {
            write_output_port(rhs);
        } else {
            mem_reg = rhs;
        }
        return 1;
    }

    /* --- bit-wise path: O<n> = <expr> / M<n> = <expr> --- */
    int bit = -1;
    if (!sc_read_uint(&sc, &bit) || bit < 0 || bit > 7) {
        printf("ERR: bad bit (0..7)\n");
        return 0;
    }

    /* Expect '=' */
    if (!sc_consume_char(&sc, '=')) {
        printf("ERR: expected '='\n");
        return 0;
    }

    /* Evaluate expression */
    uint8_t in_byte = read_input_port();
    int ok = 1;

    // n-ary operators with function in prefix
    const char *save = sc.cur;
    uint8_t val = eval_prefix_expr(&sc, in_byte, &ok);
    if (ok) {
        // ok prefix path
    } else {
        // fallback alla forma infissa esistente
        sc.cur = save;
        ok = 1;
        val = eval_expr(&sc, in_byte, &ok);
        if (!ok) { printf("ERR: bad expression\n"); return 0; }
    }

    sc_skip_ws(&sc);
    if (*sc.cur != '\0') {
        printf("ERR: unexpected trailing characters\n");
        return 0;
    }

    /* Write destination bit */
    if (target_is_output)
        set_output_bit((uint8_t)bit, val);
    else
        set_memory_bit((uint8_t)bit, val);

    return 1;
}

/* Run once from the first line to the end, honoring GOTO and user BREAK.
 * Stops on:
 * - error in a statement (prints line number)
 * - GOTO to a non-existent line
 * - user BREAK (CapsShift+Space on Spectrum)
 */
static void run_once(void) {
    int pc = 0; /* index into program_mem[] */
    while (pc < line_count) {
        /* Allow user to force a break */
        if (zx_break()) {
            puts("** BREAK **");
            return;
        }
        const char *t = program_mem[pc].text;
        if (t[0] == '\0') { pc++; continue; }

        int jump_to = -1;
        if (!exec_statement(t, &jump_to)) {
            printf("STOP at line %d\n", program_mem[pc].number);
            return;
        }
        if (jump_to >= 0) {
            int idx = find_line_index((int16_t)jump_to);
            if (idx < 0) {
                printf("ERR: GOTO to missing line %d\n", jump_to);
                return;
            }
            pc = idx;
        } else {
            pc++;
        }
    }
}



/* -------------------- Command dispatcher -------------------- */
typedef int (*cmd_func)(const char *args);

typedef struct {
    const char *name;  /* UPPERCASE command keyword */
    cmd_func    func;
} Command;


/* --- Individual commands --- */

static int cmd_list(const char *args) {
    for (int i = 0; i < line_count; ++i) {
        printf("%d %s\n", program_mem[i].number, program_mem[i].text);
    }
    return 1;
}

static int cmd_new(const char *args) {
    line_count = 0;
    return 1;
}

static int cmd_monitor(const char *args) {
    flags = flags ^ F_MONITOR;
#ifdef GRAPHICS
    if (flags & F_MONITOR) {
        
        byte_to_bin(output_latch);
        monitor_panel();
    } else {
        for (int i = 7; i >= 0; --i) {
          putsprite (spr_and, 205+i*6, 4,  gfx_blank);  //blank
        }
    }
#else
    byte_to_bin(output_latch);
    if (flags & F_MONITOR)
        printf("[%s]\n", bbuf);
    else
        printf ("Monitor OFF\n\n");
#endif
    return 1;
}


/* -------------------- Help -------------------- */
static int cmd_help(const char *args) {
    puts("BASIC-LIKE instructions:");
    puts("  <num> <stmt>  - insert/replace program line (e.g., 10 O2 = 1)");
    puts("  LIST, RUN [times], SAVE [+notes], LOAD");
    puts("");
    puts("BIO-BIT Specific Commands:");
    puts("  RUN <k>       - run k times");
    puts("  PIN <val>     - set/read input port");
    puts("  POUT <val>    - set/show output port/value");
    puts("  HELP          - this help");
    puts("  MONITOR       - toggle I/O monitor");
    puts("  QUIT          - exit");
    puts("");
    puts("Statement examples:");
    puts("  O2 = I3 AND I4 XOR I2");
    puts("  M0 = NOT I1");
    puts("  O7 = 1");
    puts("  O0 = AND I0 I1 I2 I3");
    return 1;
}

static int parse_uint_from_args(const char *args, int *out) {
    while (*args && isspace((unsigned char)*args)) args++;
    if (!*args) return 0;
    if (!isdigit((unsigned char)*args)) return 0;
    int v = 0;
    while (*args && isdigit((unsigned char)*args)) {
        v = v * 10 + (*args - '0');
        args++;
    }
    *out = v;
    return 1;
}

static int cmd_run(const char *args) {
    int k;
    if (parse_uint_from_args(args, &k)) {
        if (k <= 0) k = 1;
    } else {
        k = 1;
    }
    for (int i = 0; i < k; ++i) {
        run_once();
        if (zx_break()) break; /* if user held BREAK during/after run */
    }
    return 1;
}

static int cmd_save(const char *args) {
    program_mem[MAX_LINES].number=line_count;
    program_mem[MAX_LINES].text[MAX_LINE_LEN-1]=input_addr;
    program_mem[MAX_LINES].text[MAX_LINE_LEN-2]=output_addr;

    // Save also an optional comment
    while (isspace((unsigned char)*args)) args++;

    if (*args) {
        safe_copy(program_mem[MAX_LINES].text, args, MAX_LINE_LEN-5);
    } else {
        // No comment
        program_mem[MAX_LINES].text[0] = '\0';
    }


    tape_save_block(program_mem, sizeof(program_mem), 1);
	//bit_save_msx("BIOBIT",program_mem,program_mem,program_mem,sizeof(program_mem));

    return 1;
}

static int cmd_load(const char *args) {
    tape_load_block(program_mem, sizeof(program_mem), 1);
	//bit_load_msx("BIOBIT",program_mem,sizeof(program_mem));

    line_count=program_mem[MAX_LINES].number;
    input_addr=program_mem[MAX_LINES].text[MAX_LINE_LEN-1];
    output_addr=program_mem[MAX_LINES].text[MAX_LINE_LEN-2];

    if (program_mem[MAX_LINES].text[0]) {
        printf("Loaded %u program lines - %s\n", line_count, program_mem[MAX_LINES].text);
    }
    return 1;
}



static int cmd_pin(const char *args) {
    while (*args && isspace((unsigned char)*args)) args++;
    if (*args && isdigit((unsigned char)*args)) {
        int v = 0;
        while (*args && isdigit((unsigned char)*args)) { v = v * 10 + (*args - '0'); args++; }
        if (v < 0)   v = 0;
        if (v > 255) v = 255;
        input_addr = (uint8_t)v;
        printf("PIN = %u\n", (unsigned)input_addr);
    } else {
        read_input_port();
        byte_to_bin(input_latch);
        printf("PIN = %u, value: [%s]\n", (unsigned)input_addr,bbuf);
    }
    return 1;
}

static int cmd_pout(const char *args) {
    while (*args && isspace((unsigned char)*args)) args++;
    if (*args && isdigit((unsigned char)*args)) {
        int v = 0;
        while (*args && isdigit((unsigned char)*args)) { v = v * 10 + (*args - '0'); args++; }
        if (v < 0)   v = 0;
        if (v > 255) v = 255;
        output_addr = (uint8_t)v;
        printf("POUT = %u\n", (unsigned)output_addr);
    } else {
        byte_to_bin(output_latch);
        printf("POUT = %u, value: [%s]\n", (unsigned)output_addr,bbuf);
    }
    return 1;
}

/* Command table */
static const Command commands[] = {
    {"LIST", cmd_list},
    {"NEW",  cmd_new},
    {"RUN",  cmd_run},
    {"PIN",  cmd_pin},
    {"POUT", cmd_pout},
    {"SAVE", cmd_save},
    {"LOAD", cmd_load},
    {"MONITOR",  cmd_monitor},
    {"HELP", cmd_help},
    {"QUIT", NULL},      /* handled specially in the dispatcher */
};

static const int num_commands = 11;

/* Try to dispatch a command.
 * Returns: 2 -> QUIT requested, 1 -> handled, 0 -> not a command
 */
static int dispatch_command(const char *work, const char *up) {
    for (int i = 0; i < num_commands; ++i) {
        size_t len = strlen(commands[i].name);
        if (strncmp(up, commands[i].name, len) == 0 &&
            (up[len] == '\0' || isspace((unsigned char)up[len]))) {

            if (strcmp(commands[i].name, "QUIT") == 0) {
                return 2; /* signal exit */
            }
            const char *args = work + (int)len; /* original (already trimmed & uppercased) */
            commands[i].func(args);
            return 1;
        }
    }
    return 0;
}

/* -------------------- REPL -------------------- */
int main(void) {

    clg();
    fputc_cons(12);

    /* Initialize output latch to 0 on start */
    write_output_port(0x00);

    char line[128];

    printf("## BIO-BIT v1.2 ##\n\n");
    printf("Mini BASIC-like I/O Interpreter\n");
    printf("Type HELP for help.\n");

    for (;;) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        /* Strip single trailing newline or CR */
        size_t n = strlen(line);
        if (n && (line[n - 1] == '\n' || line[n - 1] == '\r')) line[n - 1] = '\0';

        char work[128];
        safe_copy(work, line, sizeof(work));
        trim(work);
        if (work[0] == '\0') continue;

        /* Work in uppercase for everything below */
        str_to_upper(work);

        /* If starts with a number -> program line insertion/deletion */
        {
            const char *p = work;
            while (*p && isspace((unsigned char)*p)) p++;
            if (isdigit((unsigned char)*p)) {
                /* Read line number */
                int ln = 0;
                while (*p && isdigit((unsigned char)*p)) { ln = ln * 10 + (*p - '0'); p++; }
                if (ln < 0 || ln > 32700) {
                    printf("ERR: bad line number\n");
                    continue;
                }
                while (*p && isspace((unsigned char)*p)) p++;
                if (*p) {
                    /* Remaining is the statement text (already uppercase) */
                    insert_or_replace_line((int16_t)ln, p);
                } else {
                    delete_line((int16_t)ln);
                }
                continue;
            }
        }

        /* Otherwise: dispatch commands, then (if not a command) execute inline statement */
        {
            char up[128];
            safe_copy(up, work, sizeof(up)); /* already uppercased */

            int disp = dispatch_command(work, up);
            if (disp == 2) break;        /* QUIT */
            if (disp == 1) continue;     /* handled */

            /* If not a command, try to execute a single inline statement directly */
            int g = -1;
            if (!exec_statement(work, &g)) {
                printf("ERR: unknown command or bad statement. Type HELP.\n");
            }
        }
    }

    printf("Bye.\n");
    return 0;
}
