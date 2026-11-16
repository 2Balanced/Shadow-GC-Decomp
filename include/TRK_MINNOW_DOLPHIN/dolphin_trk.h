#ifndef TRK_MINNOW_DOLPHIN_DOLPHIN_TRK_H
#define TRK_MINNOW_DOLPHIN_DOLPHIN_TRK_H

/* The GameCube side of MetroTRK: the connection to the host (a table of functions for the
 * debugging hardware in use) and the hooks into the Dolphin OS. */

#include <TRK_MINNOW_DOLPHIN/trk.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*DBCommCallback)(s16 channel, void *context);

/* The connection functions of one kind of debugging hardware. */
typedef struct DBCommTable {
    int (*initialize)(void *inputPendingPtr, DBCommCallback callback);  /* 0x00 */
    int (*initinterrupts)(void);                                        /* 0x04 */
    int (*shutdown)(void);                                              /* 0x08 */
    int (*peek)(void);                                                  /* 0x0C */
    int (*read)(u8 *data, int length);                                  /* 0x10 */
    int (*write)(const u8 *data, int length);                           /* 0x14 */
    int (*open)(void);                                                  /* 0x18 */
    int (*close)(void);                                                 /* 0x1C */
    int (*pre_continue)(void);                                          /* 0x20 */
    int (*post_stop)(void);                                             /* 0x24 */
} DBCommTable;                                                          /* 0x28 */

/* Hardware ids the OS passes to InitMetroTRKCommTable. */
enum {
    kTRKHardwareDDH = 0,
    kTRKHardwareGDEV = 1,
    kTRKHardwareBBA = 2
};

extern DBCommTable gDBCommTable;
extern u8 TRK_Use_BBA;

int InitMetroTRKCommTable(int hardwareId);
void TRKEXICallBack(s16 channel, void *context);
void EnableEXI2Interrupts(void);
void UnreserveEXI2Port(void);
void ReserveEXI2Port(void);
void TRKUARTInterruptHandler(void);
void TRKLoadContext(void *context, u32 exception);

void InitMetroTRK(void);
void InitMetroTRK_BBA(void);
void EnableMetroTRKInterrupts(void);
void TRK_main(void);
u32 TRKTargetTranslate(u32 address);
void __TRK_copy_vectors(void);

int udp_cc_initialize(void *inputPendingPtr, DBCommCallback callback);
int udp_cc_shutdown(void);
int udp_cc_open(void);
int udp_cc_close(void);
int udp_cc_read(u8 *data, int length);
int udp_cc_write(const u8 *data, int length);
int udp_cc_peek(void);
int udp_cc_pre_continue(void);
int udp_cc_post_stop(void);

int ddh_cc_initialize(void *inputPendingPtr, DBCommCallback callback);
int ddh_cc_shutdown(void);
int ddh_cc_open(void);
int ddh_cc_close(void);
int ddh_cc_read(u8 *data, int length);
int ddh_cc_write(const u8 *data, int length);
int ddh_cc_pre_continue(void);
int ddh_cc_post_stop(void);
int ddh_cc_peek(void);
int ddh_cc_initinterrupts(void);

int gdev_cc_initialize(void *inputPendingPtr, DBCommCallback callback);
int gdev_cc_shutdown(void);
int gdev_cc_open(void);
int gdev_cc_close(void);
int gdev_cc_read(u8 *data, int length);
int gdev_cc_write(const u8 *data, int length);
int gdev_cc_pre_continue(void);
int gdev_cc_post_stop(void);
int gdev_cc_peek(void);
int gdev_cc_initinterrupts(void);

/* Requests the target passes to the nub by trapping (targsupp); the nub forwards them to the
 * host. Each returns the host's I/O result in its low byte. */
u32 TRKAccessFile(u32 command, u32 handle, TRKSize *count, u8 *data);
u32 TRKOpenFile(u32 command, const char *path, u32 mode, u32 *handle);
u32 TRKCloseFile(u32 command, u32 handle);
u32 TRKPositionFile(u32 command, u32 handle, u32 *position, u32 mode);

#ifdef __cplusplus
}
#endif

#endif
