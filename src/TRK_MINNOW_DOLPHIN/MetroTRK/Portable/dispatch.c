#include <TRK_MINNOW_DOLPHIN/trk.h>
#include <TRK_MINNOW_DOLPHIN/msghndlr.h>

TRKResult TRKInitializeDispatcher(void) {
    return kTRKOk;
}

/* Hands a message from the host to the handler of its command (the byte after the size word). */
TRKResult TRKDispatchMessage(TRKBuffer *message) {
    TRKResult result = kTRKDispatchError;

    TRKSetBufferPosition(message, 0);
    MWTRACE(1, "Dispatch command 0x%08x\n", message->data[4]);
    switch (message->data[4]) {
    case kTRKCmdConnect:
        result = TRKDoConnect(message);
        break;
    case kTRKCmdDisconnect:
        result = TRKDoDisconnect(message);
        break;
    case kTRKCmdReset:
        result = TRKDoReset(message);
        break;
    case kTRKCmdOverride:
        result = TRKDoOverride(message);
        break;
    case kTRKCmdVersions:
        result = TRKDoVersions(message);
        break;
    case kTRKCmdSupportMask:
        result = TRKDoSupportMask(message);
        break;
    case kTRKCmdReadMemory:
        result = TRKDoReadMemory(message);
        break;
    case kTRKCmdWriteMemory:
        result = TRKDoWriteMemory(message);
        break;
    case kTRKCmdReadRegisters:
        result = TRKDoReadRegisters(message);
        break;
    case kTRKCmdWriteRegisters:
        result = TRKDoWriteRegisters(message);
        break;
    case kTRKCmdContinue:
        result = TRKDoContinue(message);
        break;
    case kTRKCmdStep:
        result = TRKDoStep(message);
        break;
    case kTRKCmdStop:
        result = TRKDoStop(message);
        break;
    case kTRKCmdSetOption:
        result = TRKDoSetOption(message);
        break;
    }
    MWTRACE(1, "Dispatch complete err = %ld\n", result);
    return result;
}
