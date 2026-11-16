#include <TRK_MINNOW_DOLPHIN/trk.h>
#include <TRK_MINNOW_DOLPHIN/msghndlr.h>

/* Handlers of the debugger's commands. Each answers the host with a reply message; most replies
 * are a bare header written straight to the connection. */

/* The most a memory command transfers; ARAM transfers start at a 32-byte boundary, so the data
 * may begin up to 31 bytes into the buffer. */
#define TRK_MEMORY_DATA_SIZE 0x800

static BOOL IsTRKConnected;

static inline void TRKSendReply(int error) {
    TRKReplyHeader reply;

    memset(&reply, 0, sizeof(reply));
    reply.command = kTRKCmdReplyAck;
    reply.size = sizeof(reply);
    reply.error = error;
    TRKWriteUARTN(&reply, sizeof(reply));
}

/* Sends the reply built in the command's own buffer. */
static inline TRKResult TRKSendBuffer(TRKBuffer *message) {
    TRKResult result;

    MWTRACE(1, "SendACK : Calling MessageSend\n");
    result = TRKMessageSend(message);
    MWTRACE(1, "MessageSend err : %ld\n", result);
    return result;
}

void OutputData(void *data, int length) {
    u8 *bytes = (u8 *)data;
    int i;

    for (i = 0; i < length; i++) {
        MWTRACE(8, "%02x ", bytes[i]);
        if (i % 16 == 15) {
            MWTRACE(8, "\n");
        }
    }
    MWTRACE(8, "\n");
}

BOOL GetTRKConnected(void) {
    return IsTRKConnected;
}

void SetTRKConnected(BOOL connected) {
    IsTRKConnected = connected;
}

TRKResult TRKDoConnect(TRKBuffer *message) {
    IsTRKConnected = TRUE;
    TRKSendReply(kTRKReplyOk);
    return kTRKOk;
}

TRKResult TRKDoDisconnect(TRKBuffer *message) {
    TRKEvent event;

    IsTRKConnected = FALSE;
    TRKSendReply(kTRKReplyOk);
    TRKConstructEvent(&event, kTRKEventShutdown);
    TRKPostEvent(&event);
    return kTRKOk;
}

TRKResult TRKDoReset(TRKBuffer *message) {
    TRKSendReply(kTRKReplyOk);
    __TRK_reset();
    return kTRKOk;
}

TRKResult TRKDoOverride(TRKBuffer *message) {
    TRKSendReply(kTRKReplyOk);
    __TRK_copy_vectors();
    return kTRKOk;
}

TRKResult TRKDoVersions(TRKBuffer *message) {
    return kTRKOk;
}

TRKResult TRKDoSupportMask(TRKBuffer *message) {
    return kTRKOk;
}

TRKResult TRKDoReadMemory(TRKBuffer *message) {
    u8 data[TRK_MEMORY_DATA_SIZE + 0x20] __attribute__((aligned(32)));
    TRKReplyHeader reply;
    TRKSize length;
    TRKResult result;
    int error;
    TRKMemoryCommand *command = (TRKMemoryCommand *)message->data;
    u8 options;
    u16 commandLength;
    u32 address;

    address = command->address;
    commandLength = command->length;
    options = command->options;

    MWTRACE(1, "ReadMemory (0x%02x) : 0x%08x 0x%08x 0x%08x\n", command->command, address,
            commandLength, options);
    if (options & TRK_MEMORY_EXTENDED) {
        TRKSendReply(kTRKReplyUnsupported);
        return kTRKOk;
    }
    length = commandLength;
    if (options & TRK_MEMORY_ARAM) {
        result = TRKTargetAccessARAM(data, address, &length, TRUE);
    } else {
        result = TRKTargetAccessMemory(data, address, &length, (options & TRK_MEMORY_USER) ? 0 : 1,
                                       TRUE);
    }
    TRKResetBuffer(message, FALSE);
    if (result == kTRKOk) {
        memset(&reply, 0, sizeof(reply));
        reply.size = length + sizeof(reply);
        reply.error = result;
        reply.command = kTRKCmdReplyAck;
        TRKAppendBuffer(message, &reply, sizeof(reply));
        if (options & TRK_MEMORY_ARAM) {
            result = TRKAppendBuffer(message, data + (address & 0x1F), length);
        } else {
            result = TRKAppendBuffer(message, data, length);
        }
    }
    if (result != kTRKOk) {
        switch (result) {
        case kTRKCWDSException:
            error = kTRKReplyException;
            break;
        case kTRKInvalidMemory:
            error = kTRKReplyInvalidMemory;
            break;
        case kTRKInvalidProcessID:
            error = kTRKReplyInvalidProcess;
            break;
        case kTRKInvalidThreadID:
            error = kTRKReplyInvalidThread;
            break;
        case kTRKOsError:
            error = kTRKReplyOsError;
            break;
        default:
            error = kTRKReplyCWDS;
            break;
        }
        TRKSendReply(error);
        return kTRKOk;
    }
    return TRKSendBuffer(message);
}

TRKResult TRKDoWriteMemory(TRKBuffer *message) {
    u8 data[TRK_MEMORY_DATA_SIZE + 0x20] __attribute__((aligned(32)));
    TRKReplyHeader reply;
    TRKSize length;
    TRKResult result;
    int error;
    TRKMemoryCommand *command = (TRKMemoryCommand *)message->data;
    u8 options;
    u16 commandLength;
    u32 address;

    address = command->address;
    commandLength = command->length;
    options = command->options;

    MWTRACE(1, "WriteMemory (0x%02x) : 0x%08x 0x%08x 0x%08x\n", command->command, address,
            commandLength, options);
    if (options & TRK_MEMORY_EXTENDED) {
        TRKSendReply(kTRKReplyUnsupported);
        return kTRKOk;
    }
    length = commandLength;
    TRKSetBufferPosition(message, TRK_MESSAGE_HEADER_SIZE);
    if (options & TRK_MEMORY_ARAM) {
        TRKReadBuffer(message, data + (address & 0x1F), length);
        result = TRKTargetAccessARAM(data, address, &length, FALSE);
    } else {
        TRKReadBuffer(message, data, length);
        result = TRKTargetAccessMemory(data, address, &length, (options & TRK_MEMORY_USER) ? 0 : 1,
                                       FALSE);
    }
    TRKResetBuffer(message, FALSE);
    if (result == kTRKOk) {
        memset(&reply, 0, sizeof(reply));
        reply.size = sizeof(reply);
        reply.command = kTRKCmdReplyAck;
        reply.error = result;
        result = TRKAppendBuffer(message, &reply, sizeof(reply));
    }
    if (result != kTRKOk) {
        switch (result) {
        case kTRKCWDSException:
            error = kTRKReplyException;
            break;
        case kTRKInvalidMemory:
            error = kTRKReplyInvalidMemory;
            break;
        case kTRKInvalidProcessID:
            error = kTRKReplyInvalidProcess;
            break;
        case kTRKInvalidThreadID:
            error = kTRKReplyInvalidThread;
            break;
        case kTRKOsError:
            error = kTRKReplyOsError;
            break;
        default:
            error = kTRKReplyCWDS;
            break;
        }
        TRKSendReply(error);
        return kTRKOk;
    }
    return TRKSendBuffer(message);
}

TRKResult TRKDoReadRegisters(TRKBuffer *message) {
    TRKReplyHeader reply;
    TRKSize registerBytes;
    TRKResult result;
    int error;
    TRKRegistersCommand *command = (TRKRegistersCommand *)message->data;

    if (command->first > command->last) {
        TRKSendReply(kTRKReplyInvalidRegister);
        return kTRKOk;
    }
    reply.command = kTRKCmdReplyAck;
    reply.size = 0x468;
    TRKResetBuffer(message, FALSE);
    MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", message->length);
    TRKAppendBuffer_ui8(message, (u8 *)&reply, sizeof(reply));
    MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", message->length);
    result = TRKTargetAccessDefault(0, 0x24, message, &registerBytes, TRUE);
    MWTRACE(4, "DoReadRegisters : Error reading  default regs 0x%08x\n", result);
    MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", message->length);
    if (result == kTRKOk) {
        result = TRKTargetAccessFP(0, 0x21, message, &registerBytes, TRUE);
    }
    MWTRACE(4, "DoReadRegisters : Error FP regs 0x%08x\n", result);
    MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", message->length);
    if (result == kTRKOk) {
        result = TRKTargetAccessExtended1(0, 0x60, message, &registerBytes, TRUE);
    }
    MWTRACE(4, "DoReadRegisters : Error extended1 regs 0x%08x\n", result);
    MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", message->length);
    if (result == kTRKOk) {
        result = TRKTargetAccessExtended2(0, 0x1F, message, &registerBytes, TRUE);
    }
    MWTRACE(4, "DoReadRegisters : Error extended2 regs 0x%08x\n", result);
    MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", message->length);
    if (result != kTRKOk) {
        switch (result) {
        case kTRKUnsupported:
            error = kTRKReplyUnsupported;
            break;
        case kTRKInvalidRegister:
            error = kTRKReplyInvalidRegister;
            break;
        case kTRKCWDSException:
            error = kTRKReplyException;
            break;
        case kTRKInvalidProcessID:
            error = kTRKReplyInvalidProcess;
            break;
        case kTRKInvalidThreadID:
            error = kTRKReplyInvalidThread;
            break;
        case kTRKOsError:
            error = kTRKReplyOsError;
            break;
        default:
            error = kTRKReplyCWDS;
            break;
        }
        TRKSendReply(error);
        return kTRKOk;
    }
    return TRKSendBuffer(message);
}

TRKResult TRKDoWriteRegisters(TRKBuffer *message) {
    TRKReplyHeader reply;
    TRKSize registerBytes;
    TRKResult result;
    int error;
    TRKRegistersCommand *command = (TRKRegistersCommand *)message->data;
    u8 block = command->block;
    u16 first = command->first;
    u16 last = command->last;

    TRKSetBufferPosition(message, 0);
    if (first > last) {
        TRKSendReply(kTRKReplyInvalidRegister);
        return kTRKOk;
    }
    TRKSetBufferPosition(message, TRK_MESSAGE_HEADER_SIZE);
    switch (block) {
    case kTRKRegistersDefault:
        result = TRKTargetAccessDefault(first, last, message, &registerBytes, FALSE);
        break;
    case kTRKRegistersFP:
        result = TRKTargetAccessFP(first, last, message, &registerBytes, FALSE);
        break;
    case kTRKRegistersExtended1:
        result = TRKTargetAccessExtended1(first, last, message, &registerBytes, FALSE);
        break;
    case kTRKRegistersExtended2:
        result = TRKTargetAccessExtended2(first, last, message, &registerBytes, FALSE);
        break;
    default:
        result = kTRKUnsupported;
        break;
    }
    TRKResetBuffer(message, FALSE);
    if (result == kTRKOk) {
        memset(&reply, 0, sizeof(reply));
        reply.size = sizeof(reply);
        reply.command = kTRKCmdReplyAck;
        reply.error = result;
        result = TRKAppendBuffer(message, &reply, sizeof(reply));
    }
    if (result != kTRKOk) {
        switch (result) {
        case kTRKUnsupported:
            error = kTRKReplyUnsupported;
            break;
        case kTRKInvalidRegister:
            error = kTRKReplyInvalidRegister;
            break;
        case kTRKBufferReadPastEnd:
            error = kTRKReplyPacketSize;
            break;
        case kTRKCWDSException:
            error = kTRKReplyException;
            break;
        case kTRKInvalidProcessID:
            error = kTRKReplyInvalidProcess;
            break;
        case kTRKInvalidThreadID:
            error = kTRKReplyInvalidThread;
            break;
        case kTRKOsError:
            error = kTRKReplyOsError;
            break;
        default:
            error = kTRKReplyCWDS;
            break;
        }
        TRKSendReply(error);
        return kTRKOk;
    }
    return TRKSendBuffer(message);
}

/* Not linked into the game (nothing dispatches to it), but its message is. */
TRKResult TRKDoFlushCache(TRKBuffer *message) {
    MWTRACE(1, "DoFlushCache unimplemented!!!\n");
    return kTRKOk;
}

TRKResult TRKDoContinue(TRKBuffer *message) {
    MWTRACE(1, "DoContinue\n");
    if (!TRKTargetStopped()) {
        TRKSendReply(kTRKReplyNotStopped);
        return kTRKOk;
    }
    TRKSendReply(kTRKReplyOk);
    return TRKTargetContinue();
}

TRKResult TRKDoStep(TRKBuffer *message) {
    TRKResult result;
    TRKStepCommand *command;
    u8 options;
    u8 count;
    u32 rangeStart;
    u32 rangeEnd;
    u32 pc;

    TRKSetBufferPosition(message, 0);
    command = (TRKStepCommand *)message->data;
    options = command->options;
    rangeStart = command->rangeStart;
    rangeEnd = command->rangeEnd;
    switch (options) {
    case kTRKStepIntoCount:
    case kTRKStepOverCount:
        count = command->count;
        if (count < 1) {
            TRKSendReply(kTRKReplyParameter);
            return kTRKOk;
        }
        break;
    case kTRKStepIntoRange:
    case kTRKStepOverRange:
        pc = TRKTargetGetPC();
        if (pc < rangeStart || pc > rangeEnd) {
            TRKSendReply(kTRKReplyParameter);
            return kTRKOk;
        }
        break;
    default:
        TRKSendReply(kTRKReplyUnsupported);
        return kTRKOk;
    }
    if (!TRKTargetStopped()) {
        TRKSendReply(kTRKReplyNotStopped);
        return kTRKOk;
    }
    TRKSendReply(kTRKReplyOk);
    result = kTRKOk;
    switch (options) {
    case kTRKStepIntoCount:
    case kTRKStepOverCount:
        result = TRKTargetSingleStep(count, options == kTRKStepOverCount);
        break;
    case kTRKStepIntoRange:
    case kTRKStepOverRange:
        result = TRKTargetStepOutOfRange(rangeStart, rangeEnd, options == kTRKStepOverRange);
        break;
    }
    return result;
}

TRKResult TRKDoStop(TRKBuffer *message) {
    int error;

    switch (TRKTargetStop()) {
    case kTRKOk:
        error = kTRKReplyOk;
        break;
    case kTRKInvalidProcessID:
        error = kTRKReplyInvalidProcess;
        break;
    case kTRKInvalidThreadID:
        error = kTRKReplyInvalidThread;
        break;
    case kTRKOsError:
        error = kTRKReplyOsError;
        break;
    default:
        error = kTRKReplyError;
        break;
    }
    TRKSendReply(error);
    return kTRKOk;
}

TRKResult TRKDoSetOption(TRKBuffer *message) {
    TRKOptionCommand *command = (TRKOptionCommand *)message->data;
    u8 option = command->option;
    u8 value = command->value;

    if (option == TRK_OPTION_SERIAL_IO) {
        usr_puts_serial("\nMetroTRK Option : SerialIO - ");
        if (value) {
            usr_puts_serial("Enable\n");
        } else {
            usr_puts_serial("Disable\n");
        }
        SetUseSerialIO(value);
    }
    TRKSendReply(kTRKReplyOk);
    return kTRKOk;
}
