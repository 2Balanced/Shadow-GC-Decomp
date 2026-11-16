#ifndef MSL_ANSI_FP_H_
#define MSL_ANSI_FP_H_

/* Decimal floating-point conversion records (ansi_fp.c), also used by printf.c. */

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

typedef struct decform {
    char style;
    char unused;
    short digits;
} decform;

void __num2dec(const decform *form, double x, decimal *d);
double __dec2num(const decimal *d);

#endif
