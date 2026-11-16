/* Console output for the GameCube: stdout and stderr go to the serial port of a development
 * unit, and always to the debugger (MetroTRK). */

typedef unsigned long size_t;
typedef void (*IdleProc)(void);

/* These three still carry address-derived names in this project. */
#define __TRK_write_console fn_8039F550

unsigned long OSGetConsoleType(void);
int InitializeUART(unsigned long baudRate);
int WriteUARTN(const void *buffer, unsigned long count);
int __TRK_write_console(unsigned long handle, unsigned char *buffer, size_t *count,
                        IdleProc idleProc);

#define OS_CONSOLE_DEVELOPMENT 0x10000000
#define OS_CONSOLE_DDH 0x20000000 /* development hardware without the serial port */

#define UART_BAUD_RATE 57600

static inline int __init_uart_console(void) {
    int error = 0;
    static int initialized = 0;

    if (initialized == 0) {
        error = InitializeUART(UART_BAUD_RATE);
        if (error == 0) {
            initialized = 1;
        }
    }
    return error;
}

int __write_console(unsigned long handle, unsigned char *buffer, size_t *count,
                    IdleProc idleProc) {
    if ((OSGetConsoleType() & OS_CONSOLE_DDH) == 0) {
        if (__init_uart_console() != 0) {
            return 1;
        }
        if (WriteUARTN(buffer, *count) != 0) {
            *count = 0;
            return 1;
        }
    }
    __TRK_write_console(handle, buffer, count, idleProc);
    return 0;
}
