/* Character output. */

#include "msl_internal.h"

#define __putc(c, file) \
    ((file)->buffer_len-- ? (*(file)->buffer_ptr++ = (unsigned char)(c)) : __put_char(c, file))

#define putc(c, file) ((fwide(file, -1) >= 0) ? EOF : __putc(c, file))

/* Stores one character when the buffer has no room left (or the stream is not yet writing),
 * flushing as the buffering mode demands. */
int __put_char(int c, FILE *file) {
    int kind = file->mode.file_kind;

    file->buffer_len = 0;

    if (file->state.error || kind == __closed_file) {
        return EOF;
    }

    if (kind == __console_file) {
        __stdio_atexit();
    }

    if (file->state.io_state == __neutral) {
        if (file->mode.io_mode & __write) {
            if (file->mode.io_mode & __append) {
                if (fseek(file, 0, SEEK_END) != 0) {
                    return 0;
                }
            }
            file->state.io_state = __writing;
            __prep_buffer(file);
        }
    }

    if (file->state.io_state != __writing) {
        set_error(file);
        return EOF;
    }

    if (file->mode.buffer_mode == _IOFBF ||
        file->buffer_size == (file->buffer_ptr - file->buffer)) {
        if (__flush_buffer(file, NULL) != __no_io_error) {
            set_error(file);
            return EOF;
        }
    }

    file->buffer_len--;
    *file->buffer_ptr++ = c;

    if (file->mode.buffer_mode != _IOFBF) {
        if (file->mode.buffer_mode == _IONBF || c == '\n') {
            if (__flush_buffer(file, NULL) != __no_io_error) {
                set_error(file);
                return EOF;
            }
        }
        file->buffer_len = 0;
    }

    return (unsigned char)c;
}

int puts(const char *s) {
    int c;
    int result = 0;
    FILE *file = stdout;

    __begin_critical_region(stdin_access);

    while ((c = *s++) != 0) {
        if (putc(c, file) == EOF) {
            result = EOF;
            break;
        }
    }

    if (result != EOF) {
        result = putc('\n', file);
    }

    __end_critical_region(stdin_access);

    return result;
}
