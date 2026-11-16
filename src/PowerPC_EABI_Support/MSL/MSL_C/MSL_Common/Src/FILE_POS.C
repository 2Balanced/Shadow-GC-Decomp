/* Stream positions. */

#include "msl_internal.h"

/* The position of the next character to transfer. Text streams count each buffered newline
 * twice, for the carriage return a text file stores in front of it. */
inline fpos_t _ftell(FILE *file) {
    int chars_in_undo_buffer = 0;
    fpos_t position;
    unsigned char kind = file->mode.file_kind;

    if (!(kind == __disk_file || kind == __console_file) || file->state.error) {
        errno = EFPOS;
        return -1L;
    }

    if (file->state.io_state == __neutral) {
        return file->position;
    }

    position = file->buffer_pos + (file->buffer_ptr - file->buffer);

    if (file->state.io_state >= __rereading) {
        chars_in_undo_buffer = file->state.io_state - __rereading + 1;
        position -= chars_in_undo_buffer;
    }

    if (!file->mode.binary_io) {
        int n = file->buffer_ptr - file->buffer - chars_in_undo_buffer;
        unsigned char *p = file->buffer;

        while (n--) {
            if (*p++ == '\n') {
                position++;
            }
        }
    }

    return position;
}

long ftell(FILE *file) {
    long result;

    __begin_critical_region(stdin_access);
    result = (long)_ftell(file);
    __end_critical_region(stdin_access);

    return result;
}

int _fseek(FILE *file, fpos_t offset, int whence) {
    __pos_proc position_proc;

    if (file->mode.file_kind != __disk_file || file->state.error) {
        errno = EFPOS;
        return -1;
    }

    if (file->state.io_state == __writing) {
        if (__flush_buffer(file, NULL) != __no_io_error) {
            set_error(file);
            errno = EFPOS;
            return -1;
        }
    }

    if (whence == SEEK_CUR) {
        whence = SEEK_SET;
        offset += _ftell(file);
    }

    /* A seek inside the data already read ahead only moves the buffer pointer. */
    if (whence != SEEK_END && file->mode.io_mode != __read_write &&
        (file->state.io_state == __reading || file->state.io_state == __rereading)) {
        if (offset >= file->position || offset < file->buffer_pos) {
            file->state.io_state = __neutral;
        } else {
            file->buffer_ptr = file->buffer + (offset - file->buffer_pos);
            file->buffer_len = file->position - offset;
            file->state.io_state = __reading;
        }
    } else {
        file->state.io_state = __neutral;
    }

    if (file->state.io_state == __neutral) {
        if ((position_proc = file->position_proc) != NULL &&
            (*position_proc)(file->handle, &offset, whence, file->ref_con) != 0) {
            set_error(file);
            errno = EFPOS;
            return -1;
        }

        file->state.eof = 0;
        file->position = offset;
        file->buffer_len = 0;
    }

    return 0;
}

int fseek(FILE *file, long offset, int whence) {
    int result;

    __begin_critical_region(stdin_access);
    result = _fseek(file, offset, whence);
    __end_critical_region(stdin_access);

    return result;
}
