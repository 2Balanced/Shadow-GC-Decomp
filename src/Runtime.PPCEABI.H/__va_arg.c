/* The compiler's helper behind va_arg on the PowerPC EABI. A variadic function's prologue saves
 * r3-r10 and f1-f8 to a register save area; the va_list records how many of each class have been
 * consumed and where the next stack-passed argument is. __va_arg returns the address of the next
 * argument of the given class and advances the list past it. */

/* The EABI va_list (one element of va_list's array type). */
typedef struct __va_list_struct {
    char gpr;             /* general-purpose argument registers used so far (0-8) */
    char fpr;             /* floating-point argument registers used so far (0-8) */
    char reserved[2];
    char *input_arg_area; /* next argument passed on the stack */
    char *reg_save_area;  /* r3-r10 (4 bytes each), then f1-f8 (8 bytes each) */
} __va_list_struct;

/* Argument classes the compiler passes as `type`. */
enum {
    ARG_AGGREGATE = 0, /* passed by reference: the slot holds a pointer to the object */
    ARG_INT = 1,
    ARG_LONG_LONG = 2, /* takes an aligned pair of registers */
    ARG_DOUBLE = 3
};

void *__va_arg(__va_list_struct *list, int type) {
    char *address;
    char *usedRegs = &list->gpr;
    int index = list->gpr;
    int maxIndex = 8;
    int size = 4;
    int increment = 1;
    int pairAlign = 0;
    int saveOffset = 0;
    int regSize = 4;

    if (type == ARG_DOUBLE) {
        usedRegs = &list->fpr;
        index = list->fpr;
        size = 8;
        saveOffset = 32; /* the FPRs come after the eight saved GPRs */
        regSize = 8;
    }
    if (type == ARG_LONG_LONG) {
        size = 8;
        maxIndex--;
        if (index & 1) {
            pairAlign = 1; /* a pair starts at an odd register (r3, r5, r7, r9) */
        }
        increment = 2;
    }

    if (index < maxIndex) {
        index += pairAlign;
        address = list->reg_save_area + saveOffset + index * regSize;
        *usedRegs = index + increment;
    } else {
        /* Registers of this class are exhausted: take it from the stack, aligned to its size. */
        *usedRegs = 8;
        address = list->input_arg_area;
        address = (char *)(((unsigned long)address + (size - 1)) & ~(size - 1));
        list->input_arg_area = address + size;
    }

    if (type == ARG_AGGREGATE) {
        address = *(char **)address;
    }
    return address;
}
