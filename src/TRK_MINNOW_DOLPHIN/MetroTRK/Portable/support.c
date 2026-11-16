#include <TRK_MINNOW_DOLPHIN/trk.h>
#include <TRK_MINNOW_DOLPHIN/msghndlr.h>

/* Services the target asks of the host: file access through the debugger (the C library's console
 * and file I/O while a debugger is attached). */

extern TRKSize strlen(const char *string);

#define TRK_FILE_CHUNK_SIZE 0x800
#define TRK_REPLY_POLL_LIMIT 79999980

TRKResult TRKSuppAccessFile(u32 handle, u8 *data, TRKSize *count, int *ioResult, BOOL waitForReply,
                            BOOL read) {
    TRKFileDataCommand command;
    int replyId;
    int bufferId;
    TRKBuffer *buffer;
    TRKBuffer *reply;
    u32 length;
    u32 done;
    BOOL stop;
    u32 replyIoResult;
    u16 replyLength;
    TRKResult result;
    BOOL console;

    if (data == NULL || *count == 0) {
        return 2;
    }
    stop = FALSE;
    *ioResult = 0;
    done = 0;
    result = kTRKOk;
    while (!stop && done < *count && result == kTRKOk && *ioResult == 0) {
        memset(&command, 0, sizeof(command));
        if (*count - done > TRK_FILE_CHUNK_SIZE) {
            length = TRK_FILE_CHUNK_SIZE;
        } else {
            length = *count - done;
        }
        command.command = read ? kTRKCmdReadFile : kTRKCmdWriteFile;
        command.size = read ? sizeof(command) : length + sizeof(command);
        command.handle = handle;
        command.length = length;
        TRKGetFreeBuffer(&bufferId, &buffer);
        result = TRKAppendBuffer_ui8(buffer, (u8 *)&command, sizeof(command));
        if (!read && result == kTRKOk) {
            result = TRKAppendBuffer_ui8(buffer, data + done, length);
        }
        if (result == kTRKOk) {
            if (waitForReply) {
                console = FALSE;
                if (read && handle == 0) {
                    console = TRUE;
                }
                result = TRKRequestSend(buffer, &replyId, read ? 5 : 5, 3, !console);
                if (result == kTRKOk) {
                    reply = TRKGetBuffer(replyId);
                }
                replyLength = ((TRKFileReply *)reply->data)->length;
                replyIoResult = (u8)((TRKFileReply *)reply->data)->ioResult;
                if (read && result == kTRKOk && replyLength <= length) {
                    TRKSetBufferPosition(reply, TRK_MESSAGE_HEADER_SIZE);
                    result = TRKReadBuffer_ui8(reply, data + done, replyLength);
                    if (result == kTRKBufferReadPastEnd) {
                        result = kTRKOk;
                    }
                }
                if (replyLength != length) {
                    length = replyLength;
                    stop = TRUE;
                }
                *ioResult = replyIoResult;
                TRKReleaseBuffer(replyId);
            } else {
                result = TRKMessageSend(buffer);
            }
        }
        TRKReleaseBuffer(bufferId);
        done += length;
    }
    *count = done;
    return result;
}

/* Sends a request and waits for the host's acknowledgement, serving any commands that arrive
 * meanwhile. */
TRKResult TRKRequestSend(TRKBuffer *message, int *replyId, u32 replyMinimum, int retries,
                         BOOL timeout) {
    TRKResult result = kTRKOk;
    TRKBuffer *reply;
    u32 polls;
    int tries;
    u8 command;
    u8 error;
    BOOL bad = TRUE;

    *replyId = -1;
    for (tries = retries + 1; tries != 0 && *replyId == -1 && result == kTRKOk; tries--) {
        MWTRACE(1, "Calling MessageSend\n");
        result = TRKMessageSend(message);
        if (result == kTRKOk) {
            if (timeout) {
                polls = 0;
            }
            while (TRUE) {
                do {
                    *replyId = TRKTestForPacket();
                } while (*replyId == -1 && (!timeout || ++polls < TRK_REPLY_POLL_LIMIT));
                if (*replyId == -1) {
                    break;
                }
                bad = FALSE;
                reply = TRKGetBuffer(*replyId);
                TRKSetBufferPosition(reply, 0);
                OutputData(reply->data, reply->length);
                command = reply->data[4];
                MWTRACE(1, "msg_command : 0x%02x hdr->cmdID 0x%02x\n", command, command);
                if (command >= kTRKCmdReplyAck) {
                    break;
                }
                TRKProcessInput(*replyId);
                *replyId = -1;
            }
            if (*replyId != -1) {
                if (reply->length < TRK_MESSAGE_HEADER_SIZE) {
                    bad = TRUE;
                }
                if (result == kTRKOk && !bad) {
                    error = reply->data[8];
                    MWTRACE(1, "msg_error : 0x%02x\n", error);
                }
                if (result == kTRKOk && !bad) {
                    if ((int)command != kTRKCmdReplyAck || (int)error != kTRKReplyOk) {
                        MWTRACE(8,
                                "RequestSend : Bad ack or non ack received msg_command : 0x%02x msg_error 0x%02x\n",
                                command, error);
                        bad = TRUE;
                    }
                }
                if (result != kTRKOk || bad) {
                    TRKReleaseBuffer(*replyId);
                    *replyId = -1;
                }
            }
        }
    }
    if (*replyId == -1) {
        result = kTRKWaitAckError;
    }
    return result;
}

TRKResult HandleOpenFileSupportRequest(const char *path, u8 mode, u32 *handle, int *ioResult) {
    TRKFileOpenCommand command;
    int replyId;
    int bufferId;
    TRKBuffer *buffer;
    TRKResult result;
    TRKBuffer *reply;

    memset(&command, 0, sizeof(command));
    *handle = 0;
    command.command = kTRKCmdOpenFile;
    command.size = strlen(path) + 1 + sizeof(command);
    command.mode = mode;
    command.pathLength = strlen(path) + 1;
    TRKGetFreeBuffer(&bufferId, &buffer);
    result = TRKAppendBuffer_ui8(buffer, (u8 *)&command, sizeof(command));
    if (result == kTRKOk) {
        result = TRKAppendBuffer_ui8(buffer, (u8 *)path, strlen(path) + 1);
    }
    if (result == kTRKOk) {
        *ioResult = 0;
        result = TRKRequestSend(buffer, &replyId, 7, 3, FALSE);
        if (result == kTRKOk) {
            reply = TRKGetBuffer(replyId);
        }
        *ioResult = ((TRKFileReply *)reply->data)->ioResult;
        *handle = ((TRKFileReply *)reply->data)->handle;
        TRKReleaseBuffer(replyId);
    }
    TRKReleaseBuffer(bufferId);
    return result;
}

TRKResult HandleCloseFileSupportRequest(u32 handle, int *ioResult) {
    TRKFileCloseCommand command;
    int replyId;
    int bufferId;
    TRKBuffer *buffer;
    TRKResult result;
    TRKBuffer *reply;

    memset(&command, 0, sizeof(command));
    command.command = kTRKCmdCloseFile;
    command.size = sizeof(command);
    command.handle = handle;
    result = TRKGetFreeBuffer(&bufferId, &buffer);
    if (result == kTRKOk) {
        result = TRKAppendBuffer_ui8(buffer, (u8 *)&command, sizeof(command));
    }
    if (result == kTRKOk) {
        *ioResult = 0;
        result = TRKRequestSend(buffer, &replyId, 3, 3, FALSE);
        if (result == kTRKOk) {
            reply = TRKGetBuffer(replyId);
        }
        if (result == kTRKOk) {
            *ioResult = ((TRKFileReply *)reply->data)->ioResult;
        }
        TRKReleaseBuffer(replyId);
    }
    TRKReleaseBuffer(bufferId);
    return result;
}

TRKResult HandlePositionFileSupportRequest(u32 handle, u32 *position, u8 mode, int *ioResult) {
    TRKFilePositionCommand command;
    int replyId;
    int bufferId;
    TRKBuffer *buffer;
    TRKResult result;
    TRKBuffer *reply;

    memset(&command, 0, sizeof(command));
    command.command = kTRKCmdPositionFile;
    command.size = sizeof(command);
    command.handle = handle;
    command.position = *position;
    command.mode = mode;
    result = TRKGetFreeBuffer(&bufferId, &buffer);
    if (result == kTRKOk) {
        result = TRKAppendBuffer_ui8(buffer, (u8 *)&command, sizeof(command));
    }
    if (result == kTRKOk) {
        *ioResult = 0;
        *position = -1;
        result = TRKRequestSend(buffer, &replyId, 3, 3, FALSE);
        if (result == kTRKOk) {
            reply = TRKGetBuffer(replyId);
            if (reply != NULL) {
                *ioResult = ((TRKFileReply *)reply->data)->ioResult;
                *position = ((TRKFileReply *)reply->data)->position;
            }
        }
        TRKReleaseBuffer(replyId);
    }
    TRKReleaseBuffer(bufferId);
    return result;
}
