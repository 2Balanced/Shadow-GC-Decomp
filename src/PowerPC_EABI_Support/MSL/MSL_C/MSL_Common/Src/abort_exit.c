/* Program termination: exit() runs the static destructors and the registered exit handlers,
 * abort() skips the destructors. */

#define MAX_ATEXIT_FUNCS 64

extern void (*_dtors[])(void);

void __destroy_global_chain(void);
void __begin_critical_region(int region);
void __end_critical_region(int region);
void __kill_critical_regions(void);
void _ExitProcess(void);
int raise(int sig);

int __aborting = 0;

static void (*__atexit_funcs[MAX_ATEXIT_FUNCS])(void);
static long __atexit_curr_func = 0;

void (*__stdio_exit)(void) = 0;
void (*__console_exit)(void) = 0;

void __exit(int status);

void abort(void) {
    raise(1);
    __aborting = 1;
    __exit(1);
}

void exit(int status) {
    void (**dtor)(void);

    if (!__aborting) {
        __begin_critical_region(0);
        __end_critical_region(0);

        __destroy_global_chain();

        dtor = _dtors;
        while (*dtor != 0) {
            (**dtor)();
            dtor++;
        }

        if (__stdio_exit != 0) {
            (*__stdio_exit)();
            __stdio_exit = 0;
        }
    }

    __exit(status);
}

void __exit(int status) {
    __begin_critical_region(0);

    while (__atexit_curr_func > 0) {
        (*__atexit_funcs[--__atexit_curr_func])();
    }

    __end_critical_region(0);

    __kill_critical_regions();

    if (__console_exit != 0) {
        (*__console_exit)();
        __console_exit = 0;
    }

    _ExitProcess();
}
