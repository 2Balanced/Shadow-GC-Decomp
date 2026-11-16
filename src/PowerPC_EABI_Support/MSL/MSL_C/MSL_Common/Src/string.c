/* The <string.h> string functions. strcpy and strcmp work a word at a time once both pointers are
 * aligned; the rest are plain byte loops written with pre-increment so they compile to update-form
 * loads and stores. */

/* Sources elsewhere in the project still call these functions by their address-derived names. */
#define strncpy fn_803ACABC
#define strncmp fn_803AC8DC
#define strrchr fn_803AC864
#define strstr fn_803AC6C0

typedef unsigned long size_t;

/* (word + NO_BORROW) & HIGH_BITS is non-zero when one of the word's four bytes may be zero. */
#define HIGH_BITS 0x80808080
#define NO_BORROW 0xFEFEFEFF

int sprintf(char *dest, const char *format, ...);

/* The error numbers strerror knows. The linker dropped strerror from the game (nothing calls it),
 * so the binary keeps only its messages, in this order, in the unit's string pool; the numeric
 * values are therefore not recoverable from the game and are simply assigned in sequence here. */
enum {
    E2BIG,
    EACCES,
    EAGAIN,
    EBADF,
    EBUSY,
    ECHILD,
    EDEADLK,
    EDOM,
    EEXIST,
    EFAULT,
    EFBIG,
    EFPOS,
    EILSEQ,
    EINTR,
    EINVAL,
    EIO,
    EISDIR,
    EMFILE,
    EMLINK,
    ENAMETOOLONG,
    ENFILE,
    ENODEV,
    ENOENT,
    ENOERR,
    ENOEXEC,
    ENOLCK,
    ENOMEM,
    ENOSPC,
    ENOSYS,
    ENOTDIR,
    ENOTEMPTY,
    ENOTTY,
    ENXIO,
    EPERM,
    EPIPE,
    ERANGE,
    EROFS,
    ESIGPARM,
    ESPIPE,
    ESRCH,
    EUNKNOWN,
    EXDEV,
};

#define ERROR_TEXT_SIZE 64

/* One bit per character. */
typedef unsigned char CharSet[32];
#define CharSetAdd(set, c) ((set)[(unsigned char)(c) >> 3] |= (1 << ((c) & 7)))
#define CharSetHas(set, c) ((set)[(unsigned char)(c) >> 3] & (1 << ((c) & 7)))

size_t strlen(const char *str) {
    size_t length = -1;
    unsigned char *p = (unsigned char *)str - 1;

    do {
        length++;
    } while (*++p);
    return length;
}

char *strcpy(char *dest, const char *src) {
    register unsigned char *to;
    register unsigned char *from;
    register unsigned long word;
    register unsigned long zeroTest;
    register unsigned long align;

    from = (unsigned char *)src;
    to = (unsigned char *)dest;

    if ((align = ((int)from & 3)) != ((int)to & 3)) {
        goto byBytes;
    }

    if (align) {
        /* Copy up to the next word boundary. */
        if ((*to = *from) == 0) {
            return dest;
        }
        for (align = 3 - align; align; align--) {
            if ((*++to = *++from) == 0) {
                return dest;
            }
        }
        ++to;
        ++from;
    }

    word = *(int *)from;
    zeroTest = word + NO_BORROW;
    zeroTest &= HIGH_BITS;
    if (zeroTest) {
        goto byBytes;
    }

    /* (Casts as lvalues are a Metrowerks extension.) */
    --((int *)to);
    do {
        *(++((int *)to)) = word;
        word = *(++((int *)from));
        zeroTest = word + NO_BORROW;
        zeroTest &= HIGH_BITS;
        if (zeroTest) {
            goto lastWord;
        }
    } while (1);

lastWord:
    ++((int *)to);

byBytes:
    if ((*to = *from) == 0) {
        return dest;
    }
    do {
        if ((*++to = *++from) == 0) {
            return dest;
        }
    } while (1);

    return dest;
}

char *strncpy(char *dest, const char *src, size_t count) {
    const unsigned char *from = (const unsigned char *)src - 1;
    unsigned char *to = (unsigned char *)dest - 1;

    count++;
    while (--count) {
        if (!(*++to = *++from)) {
            /* Pad the rest of the destination with zeros. */
            while (--count) {
                *++to = 0;
            }
            break;
        }
    }
    return dest;
}

char *strcat(char *dest, const char *src) {
    const unsigned char *from = (unsigned char *)src - 1;
    unsigned char *to = (unsigned char *)dest - 1;

    while (*++to) {
    }
    to--;
    while (*++to = *++from) {
    }
    return dest;
}

char *strncat(char *dest, const char *src, size_t count) {
    const unsigned char *from = (unsigned char *)src - 1;
    unsigned char *to = (unsigned char *)dest - 1;

    while (*++to) {
    }
    to--;
    count++;
    while (--count) {
        if (!(*++to = *++from)) {
            to--;
            break;
        }
    }
    *++to = 0;
    return dest;
}

int strcmp(const char *str1, const char *str2) {
    register unsigned char *left = (unsigned char *)str1;
    register unsigned char *right = (unsigned char *)str2;
    unsigned long align;
    unsigned long leftWord;
    unsigned long rightWord;
    unsigned long zeroTest;

    leftWord = *left;
    rightWord = *right;
    if (leftWord - rightWord) {
        return leftWord - rightWord;
    }

    if ((align = ((int)left & 3)) != ((int)right & 3)) {
        goto byBytes;
    }

    if (align) {
        /* Compare up to the next word boundary. */
        if (leftWord == 0) {
            return 0;
        }
        for (align = 3 - align; align; align--) {
            leftWord = *++left;
            rightWord = *++right;
            if (leftWord - rightWord) {
                return leftWord - rightWord;
            }
            if (leftWord == 0) {
                return 0;
            }
        }
        left++;
        right++;
    }

    leftWord = *(int *)left;
    rightWord = *(int *)right;
    zeroTest = leftWord + NO_BORROW;
    if (zeroTest & HIGH_BITS) {
        goto lastWord;
    }
    while (leftWord == rightWord) {
        left += 4;
        leftWord = *(int *)left;
        right += 4;
        rightWord = *(int *)right;
        zeroTest = leftWord + NO_BORROW;
        if (zeroTest & HIGH_BITS) {
            goto lastWord;
        }
    }
    if (leftWord > rightWord) {
        return 1;
    }
    return -1;

lastWord:
    leftWord = *left;
    rightWord = *right;
    if (leftWord - rightWord) {
        return leftWord - rightWord;
    }

byBytes:
    if (leftWord == 0) {
        return 0;
    }
    do {
        leftWord = *++left;
        rightWord = *++right;
        if (leftWord - rightWord) {
            return leftWord - rightWord;
        }
        if (leftWord == 0) {
            return 0;
        }
    } while (1);
}

int strncmp(const char *str1, const char *str2, size_t count) {
    const unsigned char *p1 = (unsigned char *)str1 - 1;
    const unsigned char *p2 = (unsigned char *)str2 - 1;
    unsigned long c1;
    unsigned long c2;

    count++;
    while (--count) {
        if ((c1 = *++p1) != (c2 = *++p2)) {
            return c1 - c2;
        } else if (!c1) {
            break;
        }
    }
    return 0;
}

char *strchr(const char *str, int chr) {
    const unsigned char *p = (unsigned char *)str - 1;
    unsigned long wanted = chr & 0xFF;
    unsigned long c;

    while (c = *++p) {
        if (c == wanted) {
            return (char *)p;
        }
    }
    /* Searching for the terminator finds it. */
    return wanted ? 0 : (char *)p;
}

char *strrchr(const char *str, int chr) {
    const unsigned char *p = (unsigned char *)str - 1;
    const unsigned char *last = 0;
    unsigned long wanted = chr & 0xFF;
    unsigned long c;

    while (c = *++p) {
        if (c == wanted) {
            last = p;
        }
    }
    if (last) {
        return (char *)last;
    }
    return wanted ? 0 : (char *)p;
}

char *strtok(char *str, const char *delimiters) {
    static unsigned char *empty = (unsigned char *)"";
    static unsigned char *next = (unsigned char *)"";
    unsigned char *p;
    unsigned char *token;
    int c;
    CharSet set = {0};

    if (str) {
        next = (unsigned char *)str;
    }

    p = (unsigned char *)delimiters - 1;
    while (c = *++p) {
        CharSetAdd(set, c);
    }

    /* Skip leading delimiters. */
    p = next - 1;
    while (c = *++p) {
        if (!CharSetHas(set, c)) {
            break;
        }
    }
    if (!c) {
        next = empty;
        return 0;
    }

    token = p;
    while (c = *++p) {
        if (CharSetHas(set, c)) {
            break;
        }
    }
    if (!c) {
        next = empty;
    } else {
        next = p + 1;
        *p = 0;
    }
    return (char *)token;
}

char *strstr(const char *str, const char *pattern) {
    unsigned char *s = (unsigned char *)str - 1;
    unsigned char *p = (unsigned char *)pattern - 1;
    unsigned long first;
    unsigned long c1;
    unsigned long c2;

    if (pattern == 0 || !(first = *++p)) {
        return (char *)str;
    }

    while (c1 = *++s) {
        if (c1 == first) {
            const unsigned char *s2 = s - 1;
            const unsigned char *p2 = p - 1;

            while ((c1 = *++s2) == (c2 = *++p2) && c1) {
            }
            if (!c2) {
                return (char *)s;
            }
        }
    }
    return 0;
}

char *__strerror(int errnum, char *str) {
    switch (errnum) {
    case E2BIG:
        strcpy(str, "Argument list too long");
        break;
    case EACCES:
        strcpy(str, "Permission denied");
        break;
    case EAGAIN:
        strcpy(str, "Resource temporarily unavailable");
        break;
    case EBADF:
        strcpy(str, "Bad file descriptor");
        break;
    case EBUSY:
        strcpy(str, "Device busy");
        break;
    case ECHILD:
        strcpy(str, "No child processes");
        break;
    case EDEADLK:
        strcpy(str, "Resource deadlock avoided");
        break;
    case EDOM:
        strcpy(str, "Numerical argument out of domain");
        break;
    case EEXIST:
        strcpy(str, "File exists");
        break;
    case EFAULT:
        strcpy(str, "Bad address");
        break;
    case EFBIG:
        strcpy(str, "File too large");
        break;
    case EFPOS:
        strcpy(str, "File Position Error");
        break;
    case EILSEQ:
        strcpy(str, "Wide character encoding error");
        break;
    case EINTR:
        strcpy(str, "Interrupted system call");
        break;
    case EINVAL:
        strcpy(str, "Invalid argument");
        break;
    case EIO:
        strcpy(str, "Input/output error");
        break;
    case EISDIR:
        strcpy(str, "Is a directory");
        break;
    case EMFILE:
        strcpy(str, "Too many open files");
        break;
    case EMLINK:
        strcpy(str, "Too many links");
        break;
    case ENAMETOOLONG:
        strcpy(str, "File name too long");
        break;
    case ENFILE:
        strcpy(str, "Too many open files in system");
        break;
    case ENODEV:
        strcpy(str, "Operation not supported by device");
        break;
    case ENOENT:
        strcpy(str, "No such file or directory");
        break;
    case ENOERR:
        strcpy(str, "No error detected");
        break;
    case ENOEXEC:
        strcpy(str, "Exec format error");
        break;
    case ENOLCK:
        strcpy(str, "No locks available");
        break;
    case ENOMEM:
        strcpy(str, "Cannot allocate memory");
        break;
    case ENOSPC:
        strcpy(str, "No space left on device");
        break;
    case ENOSYS:
        strcpy(str, "Function not implemented");
        break;
    case ENOTDIR:
        strcpy(str, "Not a directory");
        break;
    case ENOTEMPTY:
        strcpy(str, "Directory not empty");
        break;
    case ENOTTY:
        strcpy(str, "Inappropriate ioctl for device");
        break;
    case ENXIO:
        strcpy(str, "Device not configured");
        break;
    case EPERM:
        strcpy(str, "Operation not permitted");
        break;
    case EPIPE:
        strcpy(str, "Broken pipe");
        break;
    case ERANGE:
        strcpy(str, "Result too large");
        break;
    case EROFS:
        strcpy(str, "Read-only file system");
        break;
    case ESIGPARM:
        strcpy(str, "Signal error");
        break;
    case ESPIPE:
        strcpy(str, "Illegal seek");
        break;
    case ESRCH:
        strcpy(str, "No such process");
        break;
    case EUNKNOWN:
        strcpy(str, "Unknown error");
        break;
    case EXDEV:
        strcpy(str, "Cross-device link");
        break;
    default:
        sprintf(str, "Unknown Error (%d)", errnum);
        break;
    }
    return str;
}

char *strerror(int errnum) {
    static char text[ERROR_TEXT_SIZE];

    return __strerror(errnum, text);
}
