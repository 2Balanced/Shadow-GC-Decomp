#ifndef RUNTIME_NMWEXCEPTION_H
#define RUNTIME_NMWEXCEPTION_H

/* The part of the CodeWarrior PowerPC EABI runtime that registers a module's exception tables.
 * The linker emits _eti_init_info, an array of these records terminated by a zeroed one; each
 * record tells the unwinder where a block of exception-table index entries is and which code
 * range it covers. */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ExceptionTableRange {
    void *indexBegin;     /* first exception-table index entry */
    void *indexEnd;       /* one past the last entry */
    void *textBegin;      /* first byte of the code those entries describe */
    unsigned long textLen; /* size of that code in bytes */
} ExceptionTableRange;

extern ExceptionTableRange _eti_init_info[];

/* Registers the tables (with the small-data base `toc`); returns a handle for unregistering. */
int __register_fragment(ExceptionTableRange *ranges, char *toc);
void __unregister_fragment(int handle);

/* Runs the destructors of constructed global objects, newest first. */
void __destroy_global_chain(void);

#ifdef __cplusplus
}
#endif

#endif
