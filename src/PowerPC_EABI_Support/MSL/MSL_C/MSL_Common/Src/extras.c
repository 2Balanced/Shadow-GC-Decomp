/* Non-standard string helpers. */

/* Sources elsewhere in the project still call this function by its address-derived name. */
#define strupr fn_803B1F68

#define EOF (-1)

extern unsigned char __upper_map[256];
static inline int toupper(int c) { return c == EOF ? EOF : __upper_map[(unsigned char)c]; }

/* Convert a string to upper case in place. */
char *strupr(char *str) {
    char *p = str;

    while (*p) {
        *p = toupper(*p);
        p++;
    }
    return str;
}
