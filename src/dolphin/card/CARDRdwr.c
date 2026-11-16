/* Dolphin SDK CARD: multi-page transfers. A read is done in 512-byte segments, a write in pages of
 * the card's page size; each completion callback advances the transfer and starts the next piece
 * until none are left or one fails. */
#include <dolphin/card_internal.h>

static void BlockReadCallback(s32 chan, s32 result) {
    CARDControl *card = &__CARDBlock[chan];
    CARDCallback callback;

    if (result < 0) {
        goto error;
    }
    card->xferred += 0x200;
    card->addr += 0x200;
    card->buffer = (u8 *)card->buffer + 0x200;
    if (--card->repeat <= 0) {
        goto error;
    }
    result = fn_8038CCB0(chan, BlockReadCallback);
    if (result < 0) {
        goto error;
    }
    return;

error:
    if (card->apiCallback == NULL) {
        fn_8038D1D4(card, result);
    }
    callback = card->xferCallback;
    if (callback) {
        card->xferCallback = NULL;
        callback(chan, result);
    }
}

s32 __CARDRead(s32 chan, u32 addr, s32 length, void *dst, CARDCallback callback) {
    CARDControl *card = &__CARDBlock[chan];

    if (!card->attached) {
        return CARD_RESULT_NOCARD;
    }
    card->xferCallback = callback;
    card->repeat = (s32)((u32)length / 0x200);
    card->addr = addr;
    card->buffer = dst;
    return fn_8038CCB0(chan, BlockReadCallback);
}

static void BlockWriteCallback(s32 chan, s32 result) {
    CARDControl *card = &__CARDBlock[chan];
    CARDCallback callback;

    if (result < 0) {
        goto error;
    }
    card->xferred += card->pageSize;
    card->addr += card->pageSize;
    card->buffer = (u8 *)card->buffer + card->pageSize;
    if (--card->repeat <= 0) {
        goto error;
    }
    result = fn_8038CDE4(chan, BlockWriteCallback);
    if (result < 0) {
        goto error;
    }
    return;

error:
    if (card->apiCallback == NULL) {
        fn_8038D1D4(card, result);
    }
    callback = card->xferCallback;
    if (callback) {
        card->xferCallback = NULL;
        callback(chan, result);
    }
}

s32 __CARDWrite(s32 chan, u32 addr, s32 length, void *src, CARDCallback callback) {
    CARDControl *card = &__CARDBlock[chan];

    if (!card->attached) {
        return CARD_RESULT_NOCARD;
    }
    card->xferCallback = callback;
    card->repeat = length / card->pageSize;
    card->addr = addr;
    card->buffer = src;
    return fn_8038CDE4(chan, BlockWriteCallback);
}
