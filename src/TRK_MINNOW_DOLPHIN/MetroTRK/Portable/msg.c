#include <TRK_MINNOW_DOLPHIN/trk.h>

/* Sends a finished message to the host over the debugger connection. */
TRKResult TRKMessageSend(TRKBuffer *message) {
    int written = TRKWriteUARTN(message->data, message->length);

    MWTRACE(1, "MessageSend : cc_write returned %ld\n", written);
    return kTRKOk;
}
