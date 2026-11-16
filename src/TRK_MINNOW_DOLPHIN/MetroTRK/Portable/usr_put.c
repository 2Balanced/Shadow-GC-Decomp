#include <TRK_MINNOW_DOLPHIN/trk.h>

/* OSReport, as this file declares it: called with the finished string only. */
extern void OSReport(const char *text);

/* Prints one character on the console, with the debugger connection marked closed meanwhile so
 * the output is not routed back to the host. */
static inline BOOL usr_putchar_serial(char c) {
    char text[2];
    BOOL connected = GetTRKConnected();

    text[0] = c;
    text[1] = '\0';
    SetTRKConnected(FALSE);
    OSReport(text);
    SetTRKConnected(connected);
    return FALSE;
}

BOOL usr_puts_serial(const char *text) {
    BOOL failed = FALSE;
    char c;

    while (!failed && (c = *text++) != '\0') {
        failed = usr_putchar_serial(c);
    }
    return failed;
}

void usr_put_initialize(void) {
}
