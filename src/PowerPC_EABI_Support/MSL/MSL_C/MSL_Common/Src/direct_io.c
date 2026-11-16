/* Block output. */

#include "msl_internal.h"

size_t fwrite(const void *ptr, size_t memb_size, size_t num_memb, FILE *file) {
    size_t result;

    __begin_critical_region(stdin_access);
    result = __fwrite(ptr, memb_size, num_memb, file);
    __end_critical_region(stdin_access);

    return result;
}

size_t __fwrite(const void *ptr, size_t memb_size, size_t num_memb, FILE *file) {
    unsigned char *write_ptr;
    size_t num_bytes;
    size_t bytes_to_go;
    size_t bytes_written;
    int ioresult;
    int always_buffer;

    if (fwide(file, 0) == 0) {
        fwide(file, -1);
    }

    bytes_to_go = memb_size * num_memb;

    if (bytes_to_go == 0 || file->state.error || file->mode.file_kind == __closed_file) {
        return 0;
    }

    if (file->mode.file_kind == __console_file) {
        __stdio_atexit();
    }

    /* Text streams and buffered binary streams go through the buffer; an unbuffered binary
     * stream writes straight from the caller's memory once the buffer is empty. */
    always_buffer = !file->mode.binary_io || file->mode.buffer_mode == _IOFBF ||
                    file->mode.buffer_mode == _IOLBF;

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
        return 0;
    }

    write_ptr = (unsigned char *)ptr;
    bytes_written = 0;

    if (bytes_to_go != 0 && (file->buffer_ptr != file->buffer || always_buffer)) {
        file->buffer_len = file->buffer_size - (file->buffer_ptr - file->buffer);

        do {
            unsigned char *newline = NULL;

            num_bytes = file->buffer_len;
            if (num_bytes > bytes_to_go) {
                num_bytes = bytes_to_go;
            }

            /* A line-buffered stream is flushed after the last newline of the block. */
            if (file->mode.buffer_mode == _IOLBF && num_bytes != 0) {
                if ((newline = (unsigned char *)__memrchr(write_ptr, '\n', num_bytes)) != NULL) {
                    num_bytes = newline + 1 - write_ptr;
                }
            }

            if (num_bytes != 0) {
                memcpy(file->buffer_ptr, write_ptr, num_bytes);

                write_ptr += num_bytes;
                bytes_written += num_bytes;
                bytes_to_go -= num_bytes;

                file->buffer_ptr += num_bytes;
                file->buffer_len -= num_bytes;
            }

            if (file->buffer_len == 0 || newline != NULL ||
                file->mode.buffer_mode == _IONBF) {
                ioresult = __flush_buffer(file, NULL);

                if (ioresult != __no_io_error) {
                    set_error(file);
                    bytes_to_go = 0;
                    break;
                }
            }
        } while (bytes_to_go != 0 && always_buffer);
    }

    if (bytes_to_go != 0 && !always_buffer) {
        unsigned char *save_buffer = file->buffer;
        size_t save_size = file->buffer_size;

        file->buffer = write_ptr;
        file->buffer_size = bytes_to_go;
        file->buffer_ptr = write_ptr + bytes_to_go;

        if (__flush_buffer(file, &num_bytes) != __no_io_error) {
            set_error(file);
        }

        bytes_written += num_bytes;

        file->buffer = save_buffer;
        file->buffer_size = save_size;

        __prep_buffer(file);

        file->buffer_len = 0;
    }

    if (file->mode.buffer_mode != _IOFBF) {
        file->buffer_len = 0;
    }

    return (bytes_written + memb_size - 1) / memb_size;
}
