/* raise: deliver a signal to the handler installed for it. */

typedef void (*SignalHandler)(int);

#define SIG_DFL ((SignalHandler)0)
#define SIG_IGN ((SignalHandler)1)

#define SIGABRT 1
#define SIGNAL_COUNT 6

/* Lock number of the signal table (see the critical-region functions). */
#define SIGNAL_ACCESS 4

void __begin_critical_region(int region);
void __end_critical_region(int region);
void exit(int status);

SignalHandler signal_funcs[SIGNAL_COUNT];

int raise(int signal) {
    SignalHandler handler;

    if (signal < 1 || signal > SIGNAL_COUNT) {
        return -1;
    }

    /* A handler is installed for one delivery only: taking it resets the entry to the default. */
    __begin_critical_region(SIGNAL_ACCESS);
    handler = signal_funcs[signal - 1];
    if (handler != SIG_IGN) {
        signal_funcs[signal - 1] = SIG_DFL;
    }
    __end_critical_region(SIGNAL_ACCESS);

    if (handler == SIG_IGN || (handler == SIG_DFL && signal == SIGABRT)) {
        return 0;
    }
    if (handler == SIG_DFL) {
        exit(0);
    }
    (*handler)(signal);
    return 0;
}
