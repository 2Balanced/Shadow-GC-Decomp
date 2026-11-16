/* Integer parsing: the scanners behind strtoul, strtol, atoi and the scanf integer conversions.
 * Characters come from a reader callback (see scanf.c), one at a time, with one character of
 * push-back. */

#define ULONG_MAX 0xFFFFFFFFUL
#define ULLONG_MAX 0xFFFFFFFFFFFFFFFFULL
#define LONG_MAX 0x7FFFFFFFL
#define LONG_MIN (-LONG_MAX - 1)
#define EOF (-1)
#define ERANGE 0x22

extern int errno;

enum { READ_GET_CHAR, READ_UNGET_CHAR, READ_TEST_ERROR };

typedef int (*ReadFn)(void *context, int ch, int action);

typedef struct StringSource {
    char *next;
    int atEnd;
} StringSource;

int __StringRead(void *context, int ch, int action);

extern unsigned char __ctype_map[256];
extern unsigned char __upper_map[256];
#define CTYPE_SPACE 0x06
#define CTYPE_DIGIT 0x10
#define CTYPE_ALPHA 0xC0
static inline int isspace(int c) { return __ctype_map[(unsigned char)c] & CTYPE_SPACE; }
static inline int isdigit(int c) { return __ctype_map[(unsigned char)c] & CTYPE_DIGIT; }
static inline int isalpha(int c) { return __ctype_map[(unsigned char)c] & CTYPE_ALPHA; }
static inline int toupper(int c) { return c == EOF ? EOF : __upper_map[(unsigned char)c]; }

/* Scanner states. They are bits so that groups of states can be tested with one mask. */
enum {
    STATE_START = 0x01,          /* skipping white space, then the sign */
    STATE_CHECK_FOR_ZERO = 0x02, /* a leading 0 may start a base prefix */
    STATE_LEADING_ZERO = 0x04,   /* seen 0: an x makes it hexadecimal */
    STATE_NEED_DIGIT = 0x08,     /* a digit must follow */
    STATE_DIGIT_LOOP = 0x10,     /* at least one digit seen */
    STATE_FINISHED = 0x20,
    STATE_FAILURE = 0x40
};

#define IsFinal(state) ((state) & (STATE_FINISHED | STATE_FAILURE))
#define IsSuccess(state) ((state) & (STATE_LEADING_ZERO | STATE_DIGIT_LOOP | STATE_FINISHED))

#define Fetch() (count++, (*read)(context, 0, READ_GET_CHAR))
#define Unfetch(c) (*read)(context, c, READ_UNGET_CHAR)

/* Scan an unsigned integer in the given base (0: decide from the prefix). The sign is reported
 * through `negative` and not applied; `overflow` is set when the value does not fit. */
unsigned long __strtoul(int base, int maxWidth, ReadFn read, void *context, int *charsScanned,
                        int *negative, int *overflow) {
    int state = STATE_START;
    int count = 0;
    int spaces = 0;
    unsigned long value = 0;
    unsigned long valueMax = 0;
    int c;

    *negative = *overflow = 0;

    if (base < 0 || base == 1 || base > 36 || maxWidth < 1) {
        state = STATE_FAILURE;
    } else {
        c = Fetch();
    }

    if (base != 0) {
        valueMax = ULONG_MAX / base;
    }

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
            if (c == '+') {
                c = Fetch();
            } else if (c == '-') {
                c = Fetch();
                *negative = 1;
            }
            state = STATE_CHECK_FOR_ZERO;
            break;

        case STATE_CHECK_FOR_ZERO:
            if (base == 0 || base == 16) {
                if (c == '0') {
                    state = STATE_LEADING_ZERO;
                    c = Fetch();
                    break;
                }
            }
            state = STATE_NEED_DIGIT;
            break;

        case STATE_LEADING_ZERO:
            if (c == 'X' || c == 'x') {
                base = 16;
                state = STATE_NEED_DIGIT;
                c = Fetch();
                break;
            }
            if (base == 0) {
                base = 8;
            }
            state = STATE_DIGIT_LOOP;
            break;

        case STATE_NEED_DIGIT:
        case STATE_DIGIT_LOOP:
            if (base == 0) {
                base = 10;
            }
            if (!valueMax) {
                valueMax = ULONG_MAX / base;
            }
            if (isdigit(c)) {
                if ((c -= '0') >= base) {
                    if (state == STATE_DIGIT_LOOP) {
                        state = STATE_FINISHED;
                    } else {
                        state = STATE_FAILURE;
                    }
                    c += '0';
                    break;
                }
            } else if (!isalpha(c) || (toupper(c) - 'A' + 10) >= base) {
                if (state == STATE_DIGIT_LOOP) {
                    state = STATE_FINISHED;
                } else {
                    state = STATE_FAILURE;
                }
                break;
            } else {
                c = toupper(c) - 'A' + 10;
            }
            if (value > valueMax) {
                *overflow = 1;
            }
            value *= base;
            if (c > ULONG_MAX - value) {
                *overflow = 1;
            }
            value += c;
            state = STATE_DIGIT_LOOP;
            c = Fetch();
            break;
        }
    }

    if (!IsSuccess(state)) {
        count = value = 0;
        *charsScanned = 0;
    } else {
        count--;
        *charsScanned = count + spaces;
    }

    Unfetch(c);
    return value;
}

unsigned long long __strtoull(int base, int maxWidth, ReadFn read, void *context,
                              int *charsScanned, int *negative, int *overflow) {
    int state = STATE_START;
    int count = 0;
    int spaces = 0;
    unsigned long long value = 0;
    unsigned long long valueMax = 0;
    int c;

    *negative = *overflow = 0;

    if (base < 0 || base == 1 || base > 36 || maxWidth < 1) {
        state = STATE_FAILURE;
    } else {
        c = Fetch();
    }

    if (base != 0) {
        valueMax = ULLONG_MAX / base;
    }

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
            if (c == '+') {
                c = Fetch();
            } else if (c == '-') {
                c = Fetch();
                *negative = 1;
            }
            state = STATE_CHECK_FOR_ZERO;
            break;

        case STATE_CHECK_FOR_ZERO:
            if (base == 0 || base == 16) {
                if (c == '0') {
                    state = STATE_LEADING_ZERO;
                    c = Fetch();
                    break;
                }
            }
            state = STATE_NEED_DIGIT;
            break;

        case STATE_LEADING_ZERO:
            if (c == 'X' || c == 'x') {
                base = 16;
                state = STATE_NEED_DIGIT;
                c = Fetch();
                break;
            }
            if (base == 0) {
                base = 8;
            }
            state = STATE_DIGIT_LOOP;
            break;

        case STATE_NEED_DIGIT:
        case STATE_DIGIT_LOOP:
            if (base == 0) {
                base = 10;
            }
            if (!valueMax) {
                valueMax = ULLONG_MAX / base;
            }
            if (isdigit(c)) {
                if ((c -= '0') >= base) {
                    if (state == STATE_DIGIT_LOOP) {
                        state = STATE_FINISHED;
                    } else {
                        state = STATE_FAILURE;
                    }
                    c += '0';
                    break;
                }
            } else if (!isalpha(c) || (toupper(c) - 'A' + 10) >= base) {
                if (state == STATE_DIGIT_LOOP) {
                    state = STATE_FINISHED;
                } else {
                    state = STATE_FAILURE;
                }
                break;
            } else {
                c = toupper(c) - 'A' + 10;
            }
            if (value > valueMax) {
                *overflow = 1;
            }
            value *= base;
            if (c > ULLONG_MAX - value) {
                *overflow = 1;
            }
            value += c;
            state = STATE_DIGIT_LOOP;
            c = Fetch();
            break;
        }
    }

    if (!IsSuccess(state)) {
        value = *charsScanned = 0;
    } else {
        count--;
        *charsScanned = count + spaces;
    }

    Unfetch(c);
    return value;
}

unsigned long strtoul(const char *str, char **end, int base) {
    unsigned long value;
    int count;
    int negative;
    int overflow;
    StringSource source;

    source.next = (char *)str;
    source.atEnd = 0;

    value = __strtoul(base, LONG_MAX, __StringRead, &source, &count, &negative, &overflow);

    if (end) {
        *end = (char *)str + count;
    }

    if (overflow) {
        value = ULONG_MAX;
        errno = ERANGE;
    } else if (negative) {
        value = -value;
    }
    return value;
}

long strtol(const char *str, char **end, int base) {
    unsigned long magnitude;
    long value;
    int count;
    int negative;
    int overflow;
    StringSource source;

    source.next = (char *)str;
    source.atEnd = 0;

    magnitude = __strtoul(base, LONG_MAX, __StringRead, &source, &count, &negative, &overflow);

    if (end) {
        *end = (char *)str + count;
    }

    if (overflow || (!negative && magnitude > LONG_MAX) ||
        (negative && magnitude > (unsigned long)LONG_MIN)) {
        value = negative ? LONG_MIN : LONG_MAX;
        errno = ERANGE;
    } else {
        value = negative ? -magnitude : magnitude;
    }
    return value;
}

int atoi(const char *str) {
    return strtol(str, 0, 10);
}
