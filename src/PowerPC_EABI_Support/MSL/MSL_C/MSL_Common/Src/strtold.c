/* Floating-point parsing: the scanner behind the scanf floating conversions. It accepts decimal
 * numbers, hexadecimal floating constants (0x1.8p3), INF/INFINITY and NAN/NAN(...). Decimal digits
 * are collected into a `decimal` record and converted by __dec2num; hexadecimal digits are packed
 * straight into the double. Characters come from a reader callback (see scanf.c). */

#define EOF (-1)
#define SHRT_MAX 0x7FFF
#define SHRT_MIN (-0x8000)

enum { READ_GET_CHAR, READ_UNGET_CHAR, READ_TEST_ERROR };

typedef int (*ReadFn)(void *context, int ch, int action);

extern unsigned char __ctype_map[256];
extern unsigned char __upper_map[256];
#define CTYPE_SPACE 0x06
#define CTYPE_DIGIT 0x10
#define CTYPE_XDIGIT 0x20
#define CTYPE_ALPHA 0xC0
static inline int isspace(int c) { return __ctype_map[(unsigned char)c] & CTYPE_SPACE; }
static inline int isdigit(int c) { return __ctype_map[(unsigned char)c] & CTYPE_DIGIT; }
static inline int isxdigit(int c) { return __ctype_map[(unsigned char)c] & CTYPE_XDIGIT; }
static inline int isalpha(int c) { return __ctype_map[(unsigned char)c] & CTYPE_ALPHA; }
static inline int toupper(int c) { return c == EOF ? EOF : __upper_map[(unsigned char)c]; }

/* The float.h limits live in float.c as raw bit patterns. (Game sources still refer to the NaN by
 * its address-derived name.) */
#define __float_nan lbl_805EDCC8
extern unsigned long __float_nan[];
extern unsigned long __float_huge[];
extern unsigned long __double_huge[];
extern unsigned long __extended_min[];
extern unsigned long __extended_max[];
#define NAN (*(float *)__float_nan)
#define INFINITY (*(float *)__float_huge)
#define HUGE_VAL (*(double *)__double_huge)
#define LDBL_MIN (*(long double *)__extended_min)
#define LDBL_MAX (*(long double *)__extended_max)

/* The current locale's numeric conventions (locale.c); only the first field is needed here. */
struct lconv {
    char *decimal_point;
    char rest[0x34];
};
extern struct lconv __lconv;

/* A number as decimal digits: value = 0.<digits> * 10^(exp + length), sign aside. */
#define SIGDIGLEN 36
typedef struct decimal {
    char sign;
    char unused;
    short exp;
    struct {
        unsigned char length;
        unsigned char text[SIGDIGLEN];
        unsigned char unused;
    } sig;
} decimal;

double __dec2num(const decimal *d);

/* Scanner states: bits, so that groups can be tested with one mask. */
enum {
    STATE_START = 0x0001,
    STATE_SIG_START = 0x0002,
    STATE_LEADING_SIG_ZEROES = 0x0004,
    STATE_INT_DIGIT_LOOP = 0x0008,
    STATE_FRAC_START = 0x0010,
    STATE_FRAC_DIGIT_LOOP = 0x0020,
    STATE_SIG_END = 0x0040,
    STATE_EXP_START = 0x0080,
    STATE_LEADING_EXP_DIGIT = 0x0100,
    STATE_LEADING_EXP_ZEROES = 0x0200,
    STATE_EXP_DIGIT_LOOP = 0x0400,
    STATE_FINISHED = 0x0800,
    STATE_FAILURE = 0x1000,
    STATE_NAN = 0x2000,
    STATE_INFINITY = 0x4000,
    STATE_HEX = 0x8000
};

/* Sub-states while scanning a hexadecimal constant. */
enum {
    HEX_NONE = 0x0000,
    HEX_START = 0x0001,
    HEX_LEADING_SIG_ZEROES = 0x0002,
    HEX_INT_DIGIT_LOOP = 0x0004,
    HEX_FRAC_DIGIT_LOOP = 0x0008,
    HEX_SIG_END = 0x0010,
    HEX_EXP_START = 0x0020,
    HEX_LEADING_EXP_DIGIT = 0x0040,
    HEX_LEADING_EXP_ZEROES = 0x0080,
    HEX_EXP_DIGIT_LOOP = 0x0100
};

#define IsFinal(state) ((state) & (STATE_FINISHED | STATE_FAILURE))
#define IsSuccess(state)                                                                           \
    ((state) & (STATE_LEADING_SIG_ZEROES | STATE_INT_DIGIT_LOOP | STATE_FRAC_DIGIT_LOOP |          \
                STATE_LEADING_EXP_ZEROES | STATE_EXP_DIGIT_LOOP | STATE_FINISHED))

#define Fetch() (count++, (*read)(context, 0, READ_GET_CHAR))
#define Unfetch(c) (*read)(context, c, READ_UNGET_CHAR)

/* NonMatching: the function has retail's size, instruction sequence, stack layout and constants;
 * what differs is which callee-saved register holds which variable (251 of its 1027 instructions
 * name a different register). Retail gives r31 downward to hexState, count, spaces, expValue,
 * expAdjust, resultBytes, nibbleIndex, hexExpChars, hexIntDigits, hexExponent and radixMarker, then
 * the block temporaries, then overflow, context and read (r18-r16), state (r15) and negative (r14);
 * this source gets overflow, context and read in r31-r29 and the locals below them. Declaration
 * order, moving the block locals, copying the parameters and the count-- idiom that settled
 * strtoul.c do not move the parameters. */

/* The most significant digits kept; further integer digits only scale the exponent. */
#define MAX_SIG_DIGITS 20

long double __strtold(int maxWidth, ReadFn read, void *context, int *charsScanned, int *overflow) {
    int state = STATE_START;
    int hexState = HEX_NONE;
    int count = 0;
    int spaces = 0;
    int c;
    decimal d = {0, 0, 0, {0, ""}};
    int negative = 0;
    int expNegative = 0;
    long expValue = 0;
    int expAdjust = 0;
    long double result;
    int signDetected = 0;
    int radixMarker = *(unsigned char *)__lconv.decimal_point;

    /* Hexadecimal constants. */
    unsigned char *resultBytes = (unsigned char *)&result;
    unsigned char byte;
    unsigned char nibble;
    int digit;
    int i;
    int nibbleIndex; /* next mantissa nibble; nibbles 0-1 are left for the sign and exponent */
    int hexExpNegative = 0;
    int hexExpChars = 0;
    int hexIntDigits = 0;
    short hexExponent = 0;

    *overflow = 0;
    c = Fetch();

    while (count <= maxWidth && c != EOF && !IsFinal(state)) {
        switch (state) {
        case STATE_START:
            if (isspace(c)) {
                /* White space is not counted against the field width. */
                c = Fetch();
                count--;
                spaces++;
                break;
            }

            switch (toupper(c)) {
            case '-':
                negative = 1;
            case '+':
                c = Fetch();
                signDetected = 1;
                break;
            case 'I':
                c = Fetch();
                state = STATE_INFINITY;
                break;
            case 'N':
                c = Fetch();
                state = STATE_NAN;
                break;
            default:
                state = STATE_SIG_START;
                break;
            }
            break;

        case STATE_INFINITY: {
            /* The I is consumed: match the rest of INF or INFINITY. */
            int matched = 1;
            char model[] = "INFINITY";
            char *expected = model + 1;

            while (matched < 8 && toupper(c) == *expected) {
                expected++;
                matched++;
                c = Fetch();
            }
            if (matched == 3 || matched == 8) {
                if (negative) {
                    result = -INFINITY;
                } else {
                    result = INFINITY;
                }
                *charsScanned = spaces + matched + signDetected;
                return result;
            } else {
                state = STATE_FAILURE;
            }
            break;
        }

        case STATE_NAN: {
            /* The N is consumed: match NAN, or NAN( followed by a tag and ). */
            int matched = 1;
            int tagChars = 0;
            char model[] = "NAN(";
            char *expected = model + 1;

            while (matched < 4 && toupper(c) == *expected) {
                expected++;
                matched++;
                c = Fetch();
            }
            if (matched == 3 || matched == 4) {
                if (matched == 4) {
                    while (tagChars < 32 && (isdigit(c) || isalpha(c))) {
                        tagChars++;
                        c = Fetch();
                    }
                    if (c != ')') {
                        state = STATE_FAILURE;
                        break;
                    } else {
                        tagChars++;
                    }
                }
                if (negative) {
                    result = -NAN;
                } else {
                    result = NAN;
                }
                *charsScanned = spaces + matched + tagChars + signDetected;
                return result;
            } else {
                state = STATE_FAILURE;
            }
            break;
        }

        case STATE_SIG_START:
            if (c == radixMarker) {
                state = STATE_FRAC_START;
                c = Fetch();
                break;
            }
            if (!isdigit(c)) {
                state = STATE_FAILURE;
                break;
            }
            if (c == '0') {
                c = Fetch();
                if (toupper(c) == 'X') {
                    state = STATE_HEX;
                    hexState = HEX_START;
                } else {
                    state = STATE_LEADING_SIG_ZEROES;
                }
                break;
            }
            state = STATE_INT_DIGIT_LOOP;
            break;

        case STATE_LEADING_SIG_ZEROES:
            if (c == '0') {
                c = Fetch();
                break;
            }
            state = STATE_INT_DIGIT_LOOP;
            break;

        case STATE_INT_DIGIT_LOOP:
            if (!isdigit(c)) {
                if (c == radixMarker) {
                    state = STATE_FRAC_DIGIT_LOOP;
                    c = Fetch();
                } else {
                    state = STATE_SIG_END;
                }
                break;
            }
            if (d.sig.length < MAX_SIG_DIGITS) {
                d.sig.text[d.sig.length++] = c;
            } else {
                expAdjust++;
            }
            c = Fetch();
            break;

        case STATE_FRAC_START:
            if (!isdigit(c)) {
                state = STATE_FAILURE;
                break;
            }
            state = STATE_FRAC_DIGIT_LOOP;
            break;

        case STATE_FRAC_DIGIT_LOOP:
            if (!isdigit(c)) {
                state = STATE_SIG_END;
                break;
            }
            if (d.sig.length < MAX_SIG_DIGITS) {
                /* Leading zeros of a pure fraction only scale the exponent. */
                if (c != '0' || d.sig.length) {
                    d.sig.text[d.sig.length++] = c;
                }
                expAdjust--;
            }
            c = Fetch();
            break;

        case STATE_SIG_END:
            if (toupper(c) == 'E') {
                state = STATE_EXP_START;
                c = Fetch();
                break;
            }
            state = STATE_FINISHED;
            break;

        case STATE_EXP_START:
            if (c == '+') {
                c = Fetch();
            } else if (c == '-') {
                c = Fetch();
                expNegative = 1;
            }
            state = STATE_LEADING_EXP_DIGIT;
            break;

        case STATE_LEADING_EXP_DIGIT:
            if (!isdigit(c)) {
                state = STATE_FAILURE;
                break;
            }
            if (c == '0') {
                state = STATE_LEADING_EXP_ZEROES;
                c = Fetch();
                break;
            }
            state = STATE_EXP_DIGIT_LOOP;
            break;

        case STATE_LEADING_EXP_ZEROES:
            if (c == '0') {
                c = Fetch();
                break;
            }
            state = STATE_EXP_DIGIT_LOOP;
            break;

        case STATE_EXP_DIGIT_LOOP:
            if (!isdigit(c)) {
                state = STATE_FINISHED;
                break;
            }
            expValue = expValue * 10 + (c - '0');
            if (expValue > SHRT_MAX) {
                *overflow = 1;
            }
            c = Fetch();
            break;

        case STATE_HEX:
            switch (hexState) {
            case HEX_START:
                for (i = 0; i < 8; i++) {
                    *(resultBytes + i) = 0;
                }
                nibbleIndex = 2;
                hexState = HEX_LEADING_SIG_ZEROES;
                c = Fetch();
                break;

            case HEX_LEADING_SIG_ZEROES:
                if (c == '0') {
                    c = Fetch();
                    break;
                }
                hexState = HEX_INT_DIGIT_LOOP;
                break;

            case HEX_INT_DIGIT_LOOP:
                if (!isxdigit(c)) {
                    if (c == radixMarker) {
                        hexState = HEX_FRAC_DIGIT_LOOP;
                        c = Fetch();
                    } else {
                        hexState = HEX_SIG_END;
                    }
                    break;
                }
                if (nibbleIndex < 17) {
                    hexIntDigits++;
                    byte = *(resultBytes + nibbleIndex / 2);
                    digit = toupper(c);
                    if (digit >= 'A') {
                        digit = digit - 'A' + 10;
                    } else {
                        digit -= '0';
                    }
                    nibble = digit;
                    if ((nibbleIndex % 2) != 0) {
                        byte |= nibble;
                    } else {
                        byte |= nibble << 4;
                    }
                    *(resultBytes + nibbleIndex++ / 2) = byte;
                    c = Fetch();
                } else {
                    c = Fetch();
                }
                break;

            case HEX_FRAC_DIGIT_LOOP:
                if (!isxdigit(c)) {
                    hexState = HEX_SIG_END;
                    break;
                }
                if (nibbleIndex < 17) {
                    byte = *(resultBytes + nibbleIndex / 2);
                    digit = toupper(c);
                    if (digit >= 'A') {
                        digit = digit - 'A' + 10;
                    } else {
                        digit -= '0';
                    }
                    nibble = digit;
                    if ((nibbleIndex % 2) != 0) {
                        byte |= nibble;
                    } else {
                        byte |= nibble << 4;
                    }
                    *(resultBytes + nibbleIndex++ / 2) = byte;
                    c = Fetch();
                } else {
                    c = Fetch();
                }
                break;

            case HEX_SIG_END:
                if (toupper(c) == 'P') {
                    hexState = HEX_EXP_START;
                    hexExpChars++;
                    c = Fetch();
                } else {
                    state = STATE_FINISHED;
                }
                break;

            case HEX_EXP_START:
                hexExpChars++;
                if (c == '-') {
                    hexExpNegative = 1;
                } else if (c != '+') {
                    Unfetch(c);
                    hexExpChars--;
                }
                hexState = HEX_LEADING_EXP_DIGIT;
                c = Fetch();
                break;

            case HEX_LEADING_EXP_DIGIT:
                if (!isdigit(c)) {
                    state = STATE_FAILURE;
                    break;
                }
                if (c == '0') {
                    /* (Nothing handles HEX_LEADING_EXP_ZEROES: the library has no such case.) */
                    hexState = HEX_LEADING_EXP_ZEROES;
                    hexExpChars++;
                    c = Fetch();
                    break;
                }
                hexState = HEX_EXP_DIGIT_LOOP;
                break;

            case HEX_EXP_DIGIT_LOOP:
                if (!isdigit(c)) {
                    state = STATE_FINISHED;
                    break;
                }
                hexExponent = hexExponent * 10 + (c - '0');
                if (expValue > SHRT_MAX) {
                    *overflow = 1;
                }
                hexExpChars++;
                c = Fetch();
                break;
            }
            break;
        }
    }

    if (!IsSuccess(state)) {
        count = 0;
        *charsScanned = 0;
    } else {
        count--;
        *charsScanned = count + spaces;
    }

    Unfetch(c);

    if (hexState == HEX_NONE) {
        if (expNegative) {
            expValue = -expValue;
        }

        /* Trailing zeros only scale the exponent. */
        {
            int length = d.sig.length;
            unsigned char *p = &d.sig.text[length];

            while (length-- && *--p == '0') {
                expAdjust++;
            }
            d.sig.length = length + 1;
            if (d.sig.length == 0) {
                d.sig.text[d.sig.length++] = '0';
            }
        }

        expValue += expAdjust;
        if (expValue < SHRT_MIN || expValue > SHRT_MAX) {
            *overflow = 1;
        }

        if (*overflow) {
            if (expNegative) {
                return 0.0;
            } else {
                return negative ? -HUGE_VAL : HUGE_VAL;
            }
        }

        d.exp = expValue;
        result = __dec2num(&d);

        if (result != 0.0 && result < LDBL_MIN) {
            *overflow = 1;
        } else if (result > LDBL_MAX) {
            *overflow = 1;
            result = HUGE_VAL;
        }

        if (negative && IsSuccess(state)) {
            result = -result;
        }
        return result;
    } else {
        unsigned long long *resultBits = (unsigned long long *)resultBytes;

        if ((double)result != 0.0) {
            if (hexExpNegative) {
                hexExponent = -hexExponent;
            }

            /* Shift the digits down until the leading 1 sits in the implicit-bit position. */
            while ((*(short *)resultBytes & 0x00F0) != 0x0010) {
                *resultBits >>= 1;
                hexExponent++;
            }

            hexExponent += 4 * (hexIntDigits - 1);
            *(short *)resultBytes &= 0x000F;
            *(short *)resultBytes |= (hexExponent + 1023) << 4;

            *charsScanned = spaces + signDetected + nibbleIndex + 1 + hexExpChars;

            if (result != 0.0 && result < LDBL_MIN) {
                *overflow = 1;
                result = 0.0;
            } else if (result > LDBL_MAX) {
                *overflow = 1;
                result = HUGE_VAL;
            }

            if (negative) {
                *(short *)resultBytes |= 0x8000;
            }
        } else {
            result = 0.0;
        }
        return result;
    }
}
