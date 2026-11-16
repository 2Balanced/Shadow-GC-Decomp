#include <TRK_MINNOW_DOLPHIN/trk.h>
#include <TRK_MINNOW_DOLPHIN/msghndlr.h>

/* Tells the host that the target stopped (kTRKCmdNotifyStopped) or took an exception, and waits
 * for its acknowledgement. */
TRKResult TRKDoNotifyStopped(int command) {
    int replyId;
    int bufferId;
    TRKBuffer *buffer;
    TRKResult result;

    result = TRKGetFreeBuffer(&bufferId, &buffer);
    if (result == kTRKOk) {
        if (result == kTRKOk) {
            if (command == kTRKCmdNotifyStopped) {
                TRKTargetAddStopInfo(buffer);
            } else {
                TRKTargetAddExceptionInfo(buffer);
            }
        }
        result = TRKRequestSend(buffer, &replyId, 2, 3, TRUE);
        if (result == kTRKOk) {
            TRKReleaseBuffer(replyId);
        }
        TRKReleaseBuffer(bufferId);
    }
    return result;
}
