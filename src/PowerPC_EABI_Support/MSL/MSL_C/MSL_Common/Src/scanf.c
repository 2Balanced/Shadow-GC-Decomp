/* The scanf family's formatted-input engine. Characters come from a reader callback, so the same
 * engine serves strings (sscanf) and files. */

/* Sources elsewhere in the project still call this function by its address-derived name. */
#define sscanf fn_803AB5B8

typedef unsigned long size_t;
typedef unsigned short wchar_t;

typedef struct __va_list_struct {
    char gpr;
    char fpr;
    char reserved[2];
    char *input_arg_area;
    char *reg_save_area;
} va_list[1];

void *__va_arg(void *list, int type);
#define va_start(list, last) ((void)(last), __builtin_va_info(&(list)))
#define va_arg(list, type) (*(type *)__va_arg(list, _var_arg_typeof(type)))

#define INT_MAX 0x7FFFFFFF
#define EOF (-1)

/* What a reader callback is asked to do. */
enum { READ_GET_CHAR, READ_UNGET_CHAR, READ_TEST_ERROR };

typedef int (*ReadFn)(void *context, int ch, int action);

extern unsigned char __ctype_map[256];
#define CTYPE_SPACE 0x06 /* blank or motion control */
#define CTYPE_DIGIT 0x10
static inline int isspace(int c) { return __ctype_map[(unsigned char)c] & CTYPE_SPACE; }
static inline int isdigit(int c) { return __ctype_map[(unsigned char)c] & CTYPE_DIGIT; }

int mbtowc(wchar_t *dest, const char *src, size_t count);
unsigned long __strtoul(int base, int maxWidth, ReadFn read, void *context, int *charsScanned,
                        int *negative, int *overflow);
unsigned long long __strtoull(int base, int maxWidth, ReadFn read, void *context,
                              int *charsScanned, int *negative, int *overflow);
long double __strtold(int maxWidth, ReadFn read, void *context, int *charsScanned, int *overflow);

/* Length modifiers. */
enum {
    ARG_NORMAL,
    ARG_CHAR,        /* hh */
    ARG_SHORT,       /* h */
    ARG_LONG,        /* l */
    ARG_LONG_LONG,   /* ll */
    ARG_DOUBLE,      /* l with a floating conversion */
    ARG_LONG_DOUBLE, /* L */
    ARG_WCHAR        /* l with c, s or [ */
};

#define BAD_CONVERSION 0xFF

/* One bit per character: the set a %[ or %s conversion accepts. */
typedef unsigned char CharSet[32];
#define CharSetAdd(set, c) ((set)[(unsigned char)(c) >> 3] |= (1 << ((c) & 7)))
#define CharSetHas(set, c) ((set)[(unsigned char)(c) >> 3] & (1 << ((c) & 7)))

typedef struct ScanFormat {
    unsigned char suppressAssignment;
    unsigned char widthSpecified;
    unsigned char argumentOptions;
    unsigned char conversion;
    int width;
    CharSet charSet;
} ScanFormat;

/* Parse one conversion specification. `spec` points at its '%'; the result points past it. */
static const char *parse_format(const char *spec, ScanFormat *out) {
    const char *s = spec;
    int c;
    int hasModifier;
    int invert;
    ScanFormat format = {0, 0, ARG_NORMAL, 0, INT_MAX, {0}};

    if ((c = *++s) == '%') {
        format.conversion = c;
        *out = format;
        return s + 1;
    }

    if (c == '*') {
        format.suppressAssignment = 1;
        c = *++s;
    }

    if (isdigit(c)) {
        format.width = 0;
        do {
            format.width = format.width * 10 + (c - '0');
            c = *++s;
        } while (isdigit(c));
        if (format.width == 0) {
            format.conversion = BAD_CONVERSION;
            *out = format;
            return s + 1;
        }
        format.widthSpecified = 1;
    }

    hasModifier = 1;
    switch (c) {
    case 'h':
        format.argumentOptions = ARG_SHORT;
        if (s[1] == 'h') {
            format.argumentOptions = ARG_CHAR;
            c = *++s;
        }
        break;
    case 'l':
        format.argumentOptions = ARG_LONG;
        if (s[1] == 'l') {
            format.argumentOptions = ARG_LONG_LONG;
            c = *++s;
        }
        break;
    case 'L':
        format.argumentOptions = ARG_LONG_DOUBLE;
        break;
    default:
        hasModifier = 0;
        break;
    }
    if (hasModifier) {
        c = *++s;
    }

    format.conversion = c;
    switch (c) {
    case 'd':
    case 'i':
    case 'u':
    case 'o':
    case 'x':
    case 'X':
        if (format.argumentOptions == ARG_LONG_DOUBLE) {
            format.conversion = BAD_CONVERSION;
        }
        break;

    case 'a':
    case 'f':
    case 'e':
    case 'E':
    case 'g':
    case 'G':
        if (format.argumentOptions == ARG_CHAR || format.argumentOptions == ARG_SHORT ||
            format.argumentOptions == ARG_LONG_LONG) {
            format.conversion = BAD_CONVERSION;
        } else if (format.argumentOptions == ARG_LONG) {
            format.argumentOptions = ARG_DOUBLE;
        }
        break;

    case 'p':
        format.argumentOptions = ARG_LONG;
        format.conversion = 'x';
        break;

    case 'c':
        if (format.argumentOptions == ARG_LONG) {
            format.argumentOptions = ARG_WCHAR;
        } else if (format.argumentOptions != ARG_NORMAL) {
            format.conversion = BAD_CONVERSION;
        }
        break;

    case 's':
        if (format.argumentOptions == ARG_LONG) {
            format.argumentOptions = ARG_WCHAR;
        } else if (format.argumentOptions != ARG_NORMAL) {
            format.conversion = BAD_CONVERSION;
        }
        {
            /* Everything but white space: tab to carriage return, and the blank. */
            int i;
            unsigned char *p;
            for (i = sizeof(format.charSet), p = format.charSet; i; --i) {
                *p++ = 0xFF;
            }
            format.charSet[1] = 0xC1;
            format.charSet[4] = 0xFE;
        }
        break;

    case 'n':
        break;

    case '[':
        if (format.argumentOptions == ARG_LONG) {
            format.argumentOptions = ARG_WCHAR;
        } else if (format.argumentOptions != ARG_NORMAL) {
            format.conversion = BAD_CONVERSION;
        }
        c = *++s;
        invert = 0;
        if (c == '^') {
            invert = 1;
            c = *++s;
        }
        if (c == ']') {
            /* A ']' right after the opening bracket is a member of the set. */
            CharSetAdd(format.charSet, ']');
            c = *++s;
        }
        while (c != 0 && c != ']') {
            int last;
            CharSetAdd(format.charSet, c);
            if (s[1] == '-' && (last = s[2]) != 0 && last != ']') {
                while (++c <= last) {
                    CharSetAdd(format.charSet, c);
                }
                c = *(s += 3);
            } else {
                c = *++s;
            }
        }
        if (c == 0) {
            format.conversion = BAD_CONVERSION;
            break;
        }
        if (invert) {
            int i;
            unsigned char *p;
            for (i = sizeof(format.charSet), p = format.charSet; i; --i, ++p) {
                *p = ~*p;
            }
        }
        break;

    default:
        format.conversion = BAD_CONVERSION;
        break;
    }

    *out = format;
    return s + 1;
}

static int __sformatter(ReadFn read, void *context, const char *formatString, va_list args) {
    int charsRead = 0;
    int assigned = 0;
    int conversions = 0;
    const char *spec = formatString;
    unsigned long unsignedValue;
    unsigned long long unsignedLongValue;
    char *dest;
    long signedValue;
    long long signedLongValue;
    long double floatValue;
    int specChar;
    int base;
    int charsScanned;
    int negative;
    int overflow;
    ScanFormat format;
    char c;
    int ch;

    while ((specChar = (char)*spec) != 0) {
        if (isspace(specChar)) {
            /* White space in the format skips any amount of white space in the input. */
            do {
                specChar = *++spec;
            } while (isspace(specChar));
            while (isspace(c = (*read)(context, 0, READ_GET_CHAR))) {
                charsRead++;
            }
            (*read)(context, c, READ_UNGET_CHAR);
            continue;
        }

        if (specChar != '%') {
            /* An ordinary character must match the input. */
            if ((unsigned char)specChar != (c = (*read)(context, 0, READ_GET_CHAR))) {
                (*read)(context, c, READ_UNGET_CHAR);
                goto done;
            }
            charsRead++;
            spec++;
            continue;
        }

        spec = parse_format(spec, &format);

        if (!format.suppressAssignment && format.conversion != '%') {
            dest = va_arg(args, char *);
        } else {
            dest = 0;
        }

        if (format.conversion != 'n' && (*read)(context, 0, READ_TEST_ERROR)) {
            goto done;
        }

        switch (format.conversion) {
        case 'd':
            base = 10;
            goto signedInteger;

        case 'i':
            base = 0;

        signedInteger:
            if (format.argumentOptions == ARG_LONG_LONG) {
                unsignedLongValue = __strtoull(base, format.width, read, context, &charsScanned,
                                               &negative, &overflow);
            }
            if (format.argumentOptions != ARG_LONG_LONG) {
                unsignedValue = __strtoul(base, format.width, read, context, &charsScanned,
                                          &negative, &overflow);
            }
            if (!charsScanned) {
                goto done;
            }
            charsRead += charsScanned;

            if (format.argumentOptions == ARG_LONG_LONG) {
                signedLongValue = negative ? -unsignedLongValue : unsignedLongValue;
            } else {
                signedValue = negative ? -unsignedValue : unsignedValue;
            }

            if (dest) {
                switch (format.argumentOptions) {
                case ARG_NORMAL:
                    *(int *)dest = signedValue;
                    break;
                case ARG_CHAR:
                    *(signed char *)dest = signedValue;
                    break;
                case ARG_SHORT:
                    *(short *)dest = signedValue;
                    break;
                case ARG_LONG:
                    *(long *)dest = signedValue;
                    break;
                case ARG_LONG_LONG:
                    *(long long *)dest = signedLongValue;
                    break;
                }
                assigned++;
            }
            conversions++;
            break;

        case 'o':
            base = 8;
            goto unsignedInteger;

        case 'u':
            base = 10;
            goto unsignedInteger;

        case 'x':
        case 'X':
            base = 16;

        unsignedInteger:
            if (format.argumentOptions == ARG_LONG_LONG) {
                unsignedLongValue = __strtoull(base, format.width, read, context, &charsScanned,
                                               &negative, &overflow);
            }
            if (format.argumentOptions != ARG_LONG_LONG) {
                unsignedValue = __strtoul(base, format.width, read, context, &charsScanned,
                                          &negative, &overflow);
            }
            if (!charsScanned) {
                goto done;
            }
            charsRead += charsScanned;

            if (negative) {
                if (format.argumentOptions == ARG_LONG_LONG) {
                    unsignedLongValue = -unsignedLongValue;
                } else {
                    unsignedValue = -unsignedValue;
                }
            }

            if (dest) {
                switch (format.argumentOptions) {
                case ARG_NORMAL:
                    *(unsigned int *)dest = unsignedValue;
                    break;
                case ARG_CHAR:
                    *(unsigned char *)dest = unsignedValue;
                    break;
                case ARG_SHORT:
                    *(unsigned short *)dest = unsignedValue;
                    break;
                case ARG_LONG:
                    *(unsigned long *)dest = unsignedValue;
                    break;
                case ARG_LONG_LONG:
                    *(unsigned long long *)dest = unsignedLongValue;
                    break;
                }
                assigned++;
            }
            conversions++;
            break;

        case 'a':
        case 'f':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
            floatValue = __strtold(format.width, read, context, &charsScanned, &overflow);
            if (!charsScanned) {
                goto done;
            }
            charsRead += charsScanned;

            if (dest) {
                switch (format.argumentOptions) {
                case ARG_NORMAL:
                    *(float *)dest = floatValue;
                    break;
                case ARG_DOUBLE:
                    *(double *)dest = floatValue;
                    break;
                case ARG_LONG_DOUBLE:
                    *(long double *)dest = floatValue;
                    break;
                }
                assigned++;
            }
            conversions++;
            break;

        case 'c':
            if (!format.widthSpecified) {
                format.width = 1;
            }
            if (dest) {
                charsScanned = 0;
                while (format.width-- && (ch = (*read)(context, 0, READ_GET_CHAR)) != EOF) {
                    c = ch;
                    if (format.argumentOptions == ARG_WCHAR) {
                        mbtowc((wchar_t *)dest, &c, 1);
                        /* The library steps one byte here, not one wide character. */
                        dest++;
                    } else {
                        *dest++ = ch;
                    }
                    charsScanned++;
                }
                if (!charsScanned) {
                    goto done;
                }
                charsRead += charsScanned;
                assigned++;
            } else {
                charsScanned = 0;
                while (format.width-- && (c = (*read)(context, 0, READ_GET_CHAR)) != EOF) {
                    charsScanned++;
                }
                if (!charsScanned) {
                    goto done;
                }
            }
            conversions++;
            break;

        case '%':
            while (isspace(c = (*read)(context, 0, READ_GET_CHAR))) {
                charsRead++;
            }
            if (c != '%') {
                (*read)(context, c, READ_UNGET_CHAR);
                goto done;
            }
            charsRead++;
            break;

        case 's':
            /* Skip leading white space, then scan like %[ with the non-space set. */
            c = (*read)(context, 0, READ_GET_CHAR);
            while (isspace(c)) {
                charsRead++;
                c = (*read)(context, 0, READ_GET_CHAR);
            }
            (*read)(context, c, READ_UNGET_CHAR);

        case '[':
            if (dest) {
                charsScanned = 0;
                while (format.width-- && (c = (*read)(context, 0, READ_GET_CHAR)) != EOF &&
                       CharSetHas(format.charSet, c)) {
                    if (format.argumentOptions == ARG_WCHAR) {
                        mbtowc((wchar_t *)dest, &c, 1);
                        dest = (char *)((wchar_t *)dest + 1);
                    } else {
                        *dest++ = c;
                    }
                    charsScanned++;
                }
                if (!charsScanned) {
                    (*read)(context, c, READ_UNGET_CHAR);
                    goto done;
                }
                charsRead += charsScanned;
                if (format.argumentOptions == ARG_WCHAR) {
                    *(wchar_t *)dest = 0;
                } else {
                    *dest = 0;
                }
                assigned++;
            } else {
                charsScanned = 0;
                while (format.width-- && (c = (*read)(context, 0, READ_GET_CHAR)) != EOF &&
                       CharSetHas(format.charSet, c)) {
                    charsScanned++;
                }
                if (!charsScanned) {
                    (*read)(context, c, READ_UNGET_CHAR);
                    break;
                }
                charsRead += charsScanned;
            }
            if (format.width >= 0) {
                (*read)(context, c, READ_UNGET_CHAR);
            }
            conversions++;
            break;

        case 'n':
            if (dest) {
                switch (format.argumentOptions) {
                case ARG_NORMAL:
                    *(int *)dest = charsRead;
                    break;
                case ARG_SHORT:
                    *(short *)dest = charsRead;
                    break;
                case ARG_LONG:
                    *(long *)dest = charsRead;
                    break;
                case ARG_CHAR:
                    *(char *)dest = charsRead;
                    break;
                case ARG_LONG_LONG:
                    *(long long *)dest = charsRead;
                    break;
                }
            }
            continue;

        case BAD_CONVERSION:
        default:
            goto done;
        }
    }

done:
    if ((*read)(context, 0, READ_TEST_ERROR) && conversions == 0) {
        return EOF;
    }
    return assigned;
}

/* The reader behind sscanf: a cursor into the string, and whether its end has been reached. */
typedef struct StringSource {
    char *next;
    int atEnd;
} StringSource;

int __StringRead(void *context, int ch, int action) {
    char c;
    StringSource *source = (StringSource *)context;

    switch (action) {
    case READ_GET_CHAR:
        c = *source->next;
        if (c == 0) {
            source->atEnd = 1;
            return EOF;
        } else {
            source->next++;
            return (unsigned char)c;
        }

    case READ_UNGET_CHAR:
        if (!source->atEnd) {
            source->next--;
        } else {
            source->atEnd = 0;
        }
        return ch;

    case READ_TEST_ERROR:
        return source->atEnd;
    }
    return 0;
}

inline int vsscanf(const char *s, const char *format, va_list args) {
    StringSource source;

    source.next = (char *)s;
    if (s == 0 || *source.next == 0) {
        return EOF;
    }
    source.atEnd = 0;
    return __sformatter(__StringRead, &source, format, args);
}

int sscanf(const char *s, const char *format, ...) {
    va_list args;

    va_start(args, format);
    return vsscanf(s, format, args);
}
