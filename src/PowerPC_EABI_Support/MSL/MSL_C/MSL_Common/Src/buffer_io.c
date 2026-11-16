/* Stream buffer bookkeeping. */

#include "msl_internal.h"

/* Empties the buffer and sizes it so that it ends on an alignment boundary of the file. */
void __prep_buffer(FILE *file) {
    file->buffer_ptr = file->buffer;
    file->buffer_len = file->buffer_size;
    file->buffer_len -= file->position & file->buffer_alignment;
    file->buffer_pos = file->position;
}

/* Writes the buffered bytes out. `bytes_flushed`, when given, receives the count written. */
int __flush_buffer(FILE *file, size_t *bytes_flushed) {
    size_t buffer_len;
    int ioresult;

    buffer_len = file->buffer_ptr - file->buffer;

    if (buffer_len != 0) {
        file->buffer_len = buffer_len;
        ioresult = (*file->write_proc)(file->handle, file->buffer, &file->buffer_len,
                                       file->ref_con);
        if (bytes_flushed != NULL) {
            *bytes_flushed = file->buffer_len;
        }
        if (ioresult != __no_io_error) {
            return ioresult;
        }
        file->position += file->buffer_len;
    }

    __prep_buffer(file);

    return __no_io_error;
}
