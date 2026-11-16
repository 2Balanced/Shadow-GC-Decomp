#ifndef DOLPHIN_EXI_H
#define DOLPHIN_EXI_H

/* Dolphin SDK EXI: the expansion interface, three serial channels (the two memory-card slots and
 * the internal one that carries the clock/SRAM chip, the serial ports and the debug UART).
 *
 * A caller locks a channel for one device, selects the device, runs immediate (up to four bytes)
 * or DMA transfers, deselects and unlocks. Slot devices are also attached, which arms the
 * "device removed" interrupt. */

#include <dolphin/os.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXI_READ 0
#define EXI_WRITE 1

/* EXIGetState */
#define EXI_STATE_DMA 0x01
#define EXI_STATE_IMM 0x02
#define EXI_STATE_BUSY (EXI_STATE_DMA | EXI_STATE_IMM)
#define EXI_STATE_SELECTED 0x04
#define EXI_STATE_ATTACHED 0x08
#define EXI_STATE_LOCKED 0x10

/* Device IDs (EXIGetID) the library itself tests for. The values are the binary's; the names
 * are ours, taken from the device names in the library's data where one fits. */
#define EXI_ID_USB_ADAPTER 0x01010000
#define EXI_ID_ARTIST_ETHER 0x04220001
#define EXI_ID_IS_VIEWER 0x05070000
#define EXI_ID_DEBUG_ADAPTER 0x07010000 /* in a slot: the debug UART is on the other port */

typedef void (*EXICallback)(s32 chan, OSContext *context);

void EXIInit(void);
BOOL EXILock(s32 chan, u32 dev, EXICallback unlockedCallback);
BOOL EXIUnlock(s32 chan);
BOOL EXISelect(s32 chan, u32 dev, u32 freq);
BOOL EXIDeselect(s32 chan);
BOOL EXIImm(s32 chan, void *buf, s32 len, u32 type, EXICallback callback);
BOOL EXIImmEx(s32 chan, void *buf, s32 len, u32 mode);
BOOL EXIDma(s32 chan, void *buf, s32 len, u32 type, EXICallback callback);
BOOL EXISync(s32 chan);
BOOL EXIProbe(s32 chan);
s32 EXIProbeEx(s32 chan);
BOOL EXIAttach(s32 chan, EXICallback extCallback);
BOOL EXIDetach(s32 chan);
u32 EXIGetState(s32 chan);
s32 EXIGetID(s32 chan, u32 dev, u32 *id);
EXICallback EXISetExiCallback(s32 chan, EXICallback exiCallback);
u32 EXIClearInterrupts(s32 chan, BOOL exi, BOOL tc, BOOL ext);

/* Routes the debug UART to a Barnacle adapter found on (chan, dev). */
void __OSEnableBarnacle(s32 chan, u32 dev);

#ifdef __cplusplus
}
#endif

#endif
