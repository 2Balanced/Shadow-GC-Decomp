/* Dolphin SDK DVD: records the last drive error in SRAM, packed into one byte, so the IPL can
 * show it after a reset. The byte is statusCode * 30 + the error's index in ErrorTable. */
#include <dolphin/dvd/__dvd.h>

typedef struct OSSramEx {
    u8 flashID[2][12];
    u32 wirelessKeyboardID;
    u16 wirelessPadID[4];
    u8 dvdErrorCode;
    u8 _padding0;
    u8 flashIDCheckSum[2];
    u16 gbs;
    u8 _padding1[2];
} OSSramEx;

OSSramEx *__OSLockSramEx(void);
BOOL __OSUnlockSramEx(BOOL commit);

#define ERROR_TABLE_SIZE 18

static u32 ErrorTable[ERROR_TABLE_SIZE] = {
    0x00000000, 0x00023A00, 0x00062800, 0x00030200, 0x00031100, 0x00052000,
    0x00052001, 0x00052100, 0x00052400, 0x00052401, 0x00052402, 0x000B5A01,
    0x00056300, 0x00020401, 0x00020400, 0x00040800, 0x00100007, 0x00000000,
};

/* Retail calls this out of line; the compiler would otherwise inline it into its one caller. */
#pragma dont_inline on
static u8 ErrorCode2Num(u32 errorCode) {
    u32 i;

    for (i = 0; i < ERROR_TABLE_SIZE; i++) {
        if (errorCode == ErrorTable[i]) {
            return (u8)i;
        }
    }
    if (errorCode >= 0x00100000 && errorCode <= 0x00100008) {
        return 17;
    }
    return 29;
}
#pragma dont_inline reset

void __DVDStoreErrorCode(u32 error) {
    OSSramEx *sram;
    u32 num;

    if (error == 0x01234567) {
        num = 255;
    } else if (error == 0x01234568) {
        num = 254;
    } else {
        u32 statusCode = error >> 24;
        u8 errorCode = ErrorCode2Num(error & 0x00FFFFFF);

        if (statusCode >= 6) {
            statusCode = 6;
        }
        num = statusCode * 30 + errorCode;
    }

    sram = __OSLockSramEx();
    sram->dvdErrorCode = (u8)num;
    __OSUnlockSramEx(TRUE);
}
