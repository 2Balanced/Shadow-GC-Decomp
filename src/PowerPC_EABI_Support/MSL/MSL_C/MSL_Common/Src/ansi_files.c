/* The standard streams and the list of open streams. */

#include "msl_internal.h"

#define console_buff_size 256

/* The console's transfer functions, outside this library's lower half. */
#define __read_console fn_8039F60C
#define __write_console fn_803AE558
#define __close_console fn_8039F4CC

int __read_console(__file_handle file, unsigned char *buff, size_t *count, __ref_con ref_con);
int __write_console(__file_handle file, unsigned char *buff, size_t *count, __ref_con ref_con);
int __close_console(__file_handle file);

static unsigned char stdin_buff[console_buff_size];
static unsigned char stdout_buff[console_buff_size];
static unsigned char stderr_buff[console_buff_size];

/* stdin, stdout, stderr, and one spare record that ends the static list. */
FILE __files[4] = {
    {
        0,
        { __must_exist, __read, _IOLBF, __console_file, __unoriented, 0 },
        { __neutral, 0, 0, 0 },
        0,
        0,
        0,
        { 0, 0 },
        { 0, 0 },
        0,
        stdin_buff,
        console_buff_size,
        stdin_buff,
        0,
        0,
        0,
        0,
        NULL,
        __read_console,
        __write_console,
        __close_console,
        NULL,
        &__files[1],
    },
    {
        1,
        { __must_exist, __write, _IOLBF, __console_file, __unoriented, 0 },
        { __neutral, 0, 0, 0 },
        0,
        0,
        0,
        { 0, 0 },
        { 0, 0 },
        0,
        stdout_buff,
        console_buff_size,
        stdout_buff,
        0,
        0,
        0,
        0,
        NULL,
        __read_console,
        __write_console,
        __close_console,
        NULL,
        &__files[2],
    },
    {
        2,
        { __must_exist, __write, _IONBF, __console_file, __unoriented, 0 },
        { __neutral, 0, 0, 0 },
        0,
        0,
        0,
        { 0, 0 },
        { 0, 0 },
        0,
        stderr_buff,
        console_buff_size,
        stderr_buff,
        0,
        0,
        0,
        0,
        NULL,
        __read_console,
        __write_console,
        __close_console,
        NULL,
        &__files[3],
    },
};

/* Closes every stream. Records allocated by fopen are freed; the static ones are marked
 * unavailable, and the static list is cut off where the allocated records begin. */
void __close_all(void) {
    FILE *file = &__files[0];
    FILE *closed;

    __begin_critical_region(stdin_access);

    while (file != NULL) {
        if (file->mode.file_kind != __closed_file) {
            fclose(file);
        }

        closed = file;
        file = file->next_file_struct;

        if (closed->is_dynamically_allocated) {
            free(closed);
        } else {
            closed->mode.file_kind = __unavailable_file;
            if (file != NULL && file->is_dynamically_allocated) {
                closed->next_file_struct = NULL;
            }
        }
    }

    __end_critical_region(stdin_access);
}

/* Flushes every open stream; EOF if any of them failed. */
int __flush_all(void) {
    int result = 0;
    FILE *file = &__files[0];

    while (file != NULL) {
        if (file->mode.file_kind != __closed_file) {
            if (fflush(file) != 0) {
                result = EOF;
            }
        }
        file = file->next_file_struct;
    }

    return result;
}
