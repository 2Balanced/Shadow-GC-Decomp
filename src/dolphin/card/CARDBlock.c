/* Dolphin SDK CARD: the block allocation table. The card keeps two copies; every update is written
 * to the current one (erase, then write) and the roles are swapped once it is safely on the card.
 * Each entry links a block to the next block of its file, 0xFFFF ends a chain, 0 marks it free. */
#include <dolphin/os.h>
#include <dolphin/card_internal.h>

void *memcpy(void *dst, const void *src, unsigned long n);

u16 *__CARDGetFatBlock(CARDControl *card) { return card->currentFat; }

static void WriteCallback(s32 chan, s32 result) {
    CARDControl *card = &__CARDBlock[chan];
    CARDCallback callback;
    u16 *fat;
    u16 *fatBack;

    if (result >= 0) {
        fat = (u16 *)((u8 *)card->workArea + 0x6000);
        fatBack = (u16 *)((u8 *)card->workArea + 0x8000);
        if (card->currentFat == fat) {
            card->currentFat = fatBack;
            memcpy(fatBack, fat, CARD_SYSTEM_BLOCK_SIZE);
        } else {
            card->currentFat = fat;
            memcpy(fat, fatBack, CARD_SYSTEM_BLOCK_SIZE);
        }
    }
    if (card->apiCallback == NULL) {
        fn_8038D1D4(card, result);
    }
    callback = card->eraseCallback;
    if (callback) {
        card->eraseCallback = NULL;
        callback(chan, result);
    }
}

static void EraseCallback(s32 chan, s32 result) {
    CARDControl *card = &__CARDBlock[chan];
    CARDCallback callback;
    u16 *fat;
    u32 addr;
    u32 unused; /* never touched; retail's frame has room for it */

    if (result < 0) {
        goto error;
    }
    fat = __CARDGetFatBlock(card);
    addr = ((u32)fat - (u32)card->workArea) / CARD_SYSTEM_BLOCK_SIZE * card->sectorSize;
    result = __CARDWrite(chan, addr, CARD_SYSTEM_BLOCK_SIZE, fat, WriteCallback);
    if (result < 0) {
        goto error;
    }
    return;

error:
    if (card->apiCallback == NULL) {
        fn_8038D1D4(card, result);
    }
    callback = card->eraseCallback;
    if (callback) {
        card->eraseCallback = NULL;
        callback(chan, result);
    }
}

/* Chains `cBlock` free blocks together, searching on from the last slot handed out; the first
 * block of the chain is left in card->startBlock. */
s32 __CARDAllocBlock(s32 chan, u32 cBlock, CARDCallback callback) {
    CARDControl *card = &__CARDBlock[chan];
    u16 *fat;
    u16 iBlock;
    u16 startBlock;
    u16 prevBlock;
    u16 count;

    if (!card->attached) {
        return CARD_RESULT_NOCARD;
    }
    fat = __CARDGetFatBlock(card);
    if (fat[CARD_FAT_FREEBLOCKS] < cBlock) {
        return CARD_RESULT_INSSPACE;
    }
    fat[CARD_FAT_FREEBLOCKS] -= cBlock;

    startBlock = 0xFFFF;
    iBlock = fat[CARD_FAT_LASTSLOT];
    count = 0;
    while (0 < cBlock) {
        if (card->cBlock - CARD_NUM_SYSTEM_BLOCK < ++count) {
            return CARD_RESULT_BROKEN;
        }
        iBlock++;
        if (!(CARD_NUM_SYSTEM_BLOCK <= iBlock && iBlock < card->cBlock)) {
            iBlock = CARD_NUM_SYSTEM_BLOCK;
        }
        if (fat[iBlock] == 0) {
            if (startBlock == 0xFFFF) {
                startBlock = iBlock;
            } else {
                fat[prevBlock] = iBlock;
            }
            prevBlock = iBlock;
            fat[iBlock] = 0xFFFF;
            --cBlock;
        }
    }
    fat[CARD_FAT_LASTSLOT] = iBlock;
    card->startBlock = startBlock;
    return __CARDUpdateFatBlock(chan, fat, callback);
}

/* Frees the chain that starts at `nBlock`. */
s32 __CARDFreeBlock(s32 chan, u16 nBlock, CARDCallback callback) {
    u16 *fat;
    u16 nextBlock;

    if (!__CARDBlock[chan].attached) {
        return CARD_RESULT_NOCARD;
    }
    fat = __CARDGetFatBlock(&__CARDBlock[chan]);
    while (nBlock != 0xFFFF) {
        if (!(CARD_NUM_SYSTEM_BLOCK <= nBlock && nBlock < __CARDBlock[chan].cBlock)) {
            return CARD_RESULT_BROKEN;
        }
        nextBlock = fat[nBlock];
        fat[nBlock] = 0;
        nBlock = nextBlock;
        ++fat[CARD_FAT_FREEBLOCKS];
    }
    return __CARDUpdateFatBlock(chan, fat, callback);
}

s32 __CARDUpdateFatBlock(s32 chan, u16 *fat, CARDCallback callback) {
    CARDControl *card = &__CARDBlock[chan];

    ++fat[CARD_FAT_CHECKCODE];
    __CARDCheckSum(&fat[CARD_FAT_CHECKCODE], CARD_SYSTEM_BLOCK_SIZE - 4, &fat[CARD_FAT_CHECKSUM],
                &fat[CARD_FAT_CHECKSUMINV]);
    DCStoreRange(fat, CARD_SYSTEM_BLOCK_SIZE);
    card->eraseCallback = callback;
    return fn_8038CF20(
        chan, ((u32)fat - (u32)card->workArea) / CARD_SYSTEM_BLOCK_SIZE * card->sectorSize,
        EraseCallback);
}
