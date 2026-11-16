/* fwide: query or set whether a stream is byte- or wide-oriented. */

/* The start of the library's FILE: a handle, then the stream's mode bits. */
typedef struct FileMode {
    unsigned int openMode : 2;
    unsigned int ioMode : 3;
    unsigned int bufferMode : 2;
    unsigned int fileKind : 3;
    unsigned int orientation : 2;
    unsigned int binaryIo : 1;
} FileMode;

typedef struct FILE {
    unsigned long handle;
    FileMode mode;
} FILE;

enum { FILE_CLOSED = 0 };
enum { ORIENT_NONE, ORIENT_CHAR, ORIENT_WIDE };

int fwide(FILE *stream, int mode) {
    int orientation;
    int result;

    if (stream == 0 || stream->mode.fileKind == FILE_CLOSED) {
        return 0;
    }

    orientation = stream->mode.orientation;
    switch (orientation) {
    case ORIENT_NONE:
        /* An unoriented stream takes the orientation asked for; 0 only queries. */
        if (mode > 0) {
            stream->mode.orientation = ORIENT_WIDE;
        } else if (mode < 0) {
            stream->mode.orientation = ORIENT_CHAR;
        }
        result = mode;
        break;
    case ORIENT_WIDE:
        result = 1;
        break;
    case ORIENT_CHAR:
        result = -1;
        break;
    }
    return result;
}
