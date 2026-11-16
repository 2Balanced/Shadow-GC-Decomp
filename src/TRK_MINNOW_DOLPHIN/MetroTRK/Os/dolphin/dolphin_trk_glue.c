#include <TRK_MINNOW_DOLPHIN/dolphin_trk.h>
#include <dolphin/os.h>

extern void PPCHalt(void);
extern int Hu_IsStub(void);
extern int AMC_IsStub(void);
extern void TRKInterruptHandler(void);

DBCommTable gDBCommTable = {0};
u8 TRK_Use_BBA = FALSE;

/* The connection's interrupt: back into the nub, as an external-interrupt exception of the
 * interrupted context. */
void TRKEXICallBack(s16 channel, void *context) {
    OSEnableScheduler();
    TRKLoadContext(context, 0x500);
}

/* Fills the connection table for the debugging hardware the OS found; returns non-zero when the
 * hardware's library is a stub. */
int InitMetroTRKCommTable(int hardwareId) {
    int isStub = 1;

    OSReport("Devkit set to : %ld\n", hardwareId);
    TRK_Use_BBA = FALSE;
    if (hardwareId == kTRKHardwareBBA) {
        OSReport("MetroTRK : Set to BBA\n");
        TRK_Use_BBA = TRUE;
        gDBCommTable.initialize = udp_cc_initialize;
        gDBCommTable.open = udp_cc_open;
        gDBCommTable.close = udp_cc_close;
        gDBCommTable.read = udp_cc_read;
        gDBCommTable.write = udp_cc_write;
        gDBCommTable.shutdown = udp_cc_shutdown;
        gDBCommTable.peek = udp_cc_peek;
        gDBCommTable.pre_continue = udp_cc_pre_continue;
        gDBCommTable.post_stop = udp_cc_post_stop;
        gDBCommTable.initinterrupts = NULL;
        return 0;
    } else if (hardwareId == kTRKHardwareGDEV) {
        OSReport("MetroTRK : Set to GDEV hardware\n");
        isStub = Hu_IsStub();
        gDBCommTable.initialize = gdev_cc_initialize;
        gDBCommTable.open = gdev_cc_open;
        gDBCommTable.close = gdev_cc_close;
        gDBCommTable.read = gdev_cc_read;
        gDBCommTable.write = gdev_cc_write;
        gDBCommTable.shutdown = gdev_cc_shutdown;
        gDBCommTable.peek = gdev_cc_peek;
        gDBCommTable.pre_continue = gdev_cc_pre_continue;
        gDBCommTable.post_stop = gdev_cc_post_stop;
        gDBCommTable.initinterrupts = gdev_cc_initinterrupts;
    } else if (hardwareId == kTRKHardwareDDH) {
        OSReport("MetroTRK : Set to AMC DDH hardware\n");
        isStub = AMC_IsStub();
        gDBCommTable.initialize = ddh_cc_initialize;
        gDBCommTable.open = ddh_cc_open;
        gDBCommTable.close = ddh_cc_close;
        gDBCommTable.read = ddh_cc_read;
        gDBCommTable.write = ddh_cc_write;
        gDBCommTable.shutdown = ddh_cc_shutdown;
        gDBCommTable.peek = ddh_cc_peek;
        gDBCommTable.pre_continue = ddh_cc_pre_continue;
        gDBCommTable.post_stop = ddh_cc_post_stop;
        gDBCommTable.initinterrupts = ddh_cc_initinterrupts;
    } else {
        OSReport("MetroTRK : Set to UNKNOWN hardware. (%ld)\n", hardwareId);
        OSReport("MetroTRK : Invalid hardware ID passed from OS\n");
        OSReport("MetroTRK : Defaulting to GDEV Hardware\n");
    }
    return isStub;
}

TRKResult TRKInitializeIntDrivenUART(u32 baudRate, u32 intDrivenInput, u32 intDrivenOutput,
                                     void *inputPendingPtr) {
    gDBCommTable.initialize(inputPendingPtr, TRKEXICallBack);
    gDBCommTable.open();
    return kTRKOk;
}

void EnableEXI2Interrupts(void) {
    if (!TRK_Use_BBA) {
        if (gDBCommTable.initinterrupts != NULL) {
            gDBCommTable.initinterrupts();
        }
    }
}

int TRKPollUART(void) {
    return gDBCommTable.peek();
}

int TRKReadUARTN(void *data, u32 length) {
    return gDBCommTable.read(data, length) == 0 ? 0 : -1;
}

int TRKWriteUARTN(const void *data, u32 length) {
    return gDBCommTable.write(data, length) == 0 ? 0 : -1;
}

void ReserveEXI2Port(void) {
    gDBCommTable.post_stop();
}

void UnreserveEXI2Port(void) {
    gDBCommTable.pre_continue();
}

void TRK_board_display(char *text) {
    OSReport("%s\n", text);
}

/* Patches the instruction after PPCHalt's first with a marker the debugger recognises as the
 * end of the program. */
void InitializeProgramEndTrap(void) {
    static const u32 EndofProgramInstruction = 0x00454E44;
    u8 *marker = (u8 *)&EndofProgramInstruction;
    u8 *halt = (u8 *)PPCHalt;

    TRK_memcpy(halt + 4, marker, 4);
    ICInvalidateRange(halt + 4, 4);
    DCFlushRange(halt + 4, 4);
}

void TRKUARTInterruptHandler(void) {
}

/* Enters the nub's interrupt handler as though `exception` had just been taken in the given
 * OSContext: restores the context's registers and passes the exception number in r3. */
asm void TRKLoadContext(register void *context, register u32 exception) {
    nofralloc
    lwz r0, 0x0(context)
    lwz r1, 0x4(context)
    lwz r2, 0x8(context)
    lhz r5, 0x1A2(context)
    rlwinm. r6, r5, 0, 30, 30
    beq volatiles_lost
    rlwinm r5, r5, 0, 31, 29
    sth r5, 0x1A2(context)
    lmw r5, 0x14(context)
    b restore_rest
volatiles_lost:
    lmw r13, 0x34(context)
restore_rest:
    mr r31, context
    mr r3, exception
    lwz r4, 0x80(r31)
    mtcrf 0xFF, r4
    lwz r4, 0x84(r31)
    mtlr r4
    lwz r4, 0x88(r31)
    mtctr r4
    lwz r4, 0x8C(r31)
    mtxer r4
    mfmsr r4
    rlwinm r4, r4, 0, 17, 15
    rlwinm r4, r4, 0, 31, 29
    mtmsr r4
    mtsprg 1, r2
    lwz r4, 0xC(r31)
    mtsprg 2, r4
    lwz r4, 0x10(r31)
    mtsprg 3, r4
    lwz r2, 0x198(r31)
    lwz r4, 0x19C(r31)
    lwz r31, 0x7C(r31)
    b TRKInterruptHandler
}
