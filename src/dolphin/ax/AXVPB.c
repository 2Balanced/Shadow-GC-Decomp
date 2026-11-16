/* Dolphin SDK AX: voices.
 *
 * Each of the 64 voices has a CPU-side record (AXVPB) holding a working copy of its parameter
 * block, and a parameter block in the array the DSP walks (__AXPB). The AXSetVoice* calls only
 * edit the copy and mark what changed in `sync`; once per frame __AXSyncPBs pushes the marked
 * parts to the DSP's block and reads back what the DSP moves on its own (state, volume envelope,
 * play position). It also budgets DSP time: voices are serviced from the highest priority down,
 * and a voice that would overrun the frame's cycle budget is stopped and reported to its owner. */
#include <dolphin/os.h>
#include <dolphin/ax.h>

/* The RenderWare units call these by their placeholder symbols. */
#define AXSetVoiceSrcType fn_80386284
#define AXSetVoiceState fn_80386348
#define AXSetVoiceMix fn_803863A4
#define AXSetVoiceAddr fn_803865F8
#define AXSetVoiceAdpcm fn_80386708
#define AXSetVoiceSrc fn_803867AC
#define AXSetVoiceSrcRatio fn_80386844
#define AXSetVoiceAdpcmLoop fn_803868DC

#define AX_MAX_VOICES 64


/* Bits in AXVPB.sync: which parts of the working copy the DSP's block still has to take over. */
#define AX_SYNC_USER_SRCSELECT     0x00000001
#define AX_SYNC_USER_MIXCTRL       0x00000002
#define AX_SYNC_USER_STATE         0x00000004
#define AX_SYNC_USER_TYPE          0x00000008
#define AX_SYNC_USER_MIX           0x00000010
#define AX_SYNC_USER_ITD           0x00000020
#define AX_SYNC_USER_ITDTARGET     0x00000040
#define AX_SYNC_USER_UPDATE        0x00000080
#define AX_SYNC_USER_DPOP          0x00000100
#define AX_SYNC_USER_VE            0x00000200
#define AX_SYNC_USER_VEDELTA       0x00000400
#define AX_SYNC_USER_FIR           0x00000800
#define AX_SYNC_USER_ADDR          0x00001000
#define AX_SYNC_USER_LOOP          0x00002000
#define AX_SYNC_USER_LOOPADDR      0x00004000
#define AX_SYNC_USER_ENDADDR       0x00008000
#define AX_SYNC_USER_CURRADDR      0x00010000
#define AX_SYNC_USER_ADPCM         0x00020000
#define AX_SYNC_USER_SRC           0x00040000
#define AX_SYNC_USER_SRCRATIO      0x00080000
#define AX_SYNC_USER_ADPCMLOOP     0x00100000
#define AX_SYNC_USER_LPF           0x00200000
#define AX_SYNC_USER_LPF_COEF      0x00400000
#define AX_SYNC_USER_ALLPARAMS     0x80000000

#define AX_PB_STATE_RUN 1
#define AX_SRC_TYPE_NONE 2

AXVPB *__AXGetStackHead(u32 priority);
void __AXPushFreeStack(AXVPB *voice);
void __AXPushCallbackStack(AXVPB *voice);
u32 __AXGetCommandListCycles(void);
void __AXDepopVoice(AXPB *pb);

/* DSP cycles per voice: sample-rate conversion by integer ratio, then the main and aux mixes by
 * the mixer-control bits. */
static u32 __AXSrcCycles[5] = {0xDF8, 0xF78, 0x14B8, 0x19F8, 0x19F8};
static u32 __AXMainMixCycles[16] = {
    0x000, 0x2F8, 0x2F8, 0x5BE, 0x2F8, 0x5F0, 0x5F0, 0x8B6,
    0x000, 0x4F1, 0x4F1, 0x9A6, 0x4F1, 0x9E2, 0x9E2, 0xE97,
};
static u32 __AXAuxMixCycles[32] = {
    0x000, 0x2F8, 0x2F8, 0x5BE, 0x000, 0x4F1, 0x4F1, 0x9A6,
    0x2F8, 0x5F0, 0x5F0, 0x8B6, 0x2F8, 0x7E9, 0x7E9, 0xC9E,
    0x000, 0x2F8, 0x2F8, 0x5BE, 0x000, 0x4F1, 0x4F1, 0x9A6,
    0x4F1, 0x7E9, 0x7E9, 0xAAF, 0x4F1, 0x9E2, 0x9E2, 0xE97,
};

static AXPB __AXPB[AX_MAX_VOICES] __attribute__((aligned(32)));
static u32 __AXITD[AX_MAX_VOICES][16] __attribute__((aligned(32)));
static u32 __AXUpdates[AX_MAX_VOICES][64] __attribute__((aligned(32)));
static AXVPB __AXVPB[AX_MAX_VOICES];

static u32 __AXMaxDspCycles;
static u32 __AXRecDspCycles;
static u32 __AXNumVoices;

u32 __AXGetNumVoices(void) { return __AXNumVoices; }

/* Copy what `sync` marks from the voice's working block to the DSP's block, and take back the
 * fields the DSP owns. */
void __AXServiceVPB(AXVPB *voice) {
    AXPB *dsp;
    AXPB *user;
    u32 sync;

    __AXNumVoices++;
    dsp = &__AXPB[voice->index];
    user = &voice->pb;
    sync = voice->sync;

    if (sync == 0) {
        user->state = dsp->state;
        user->ve.currentVolume = dsp->ve.currentVolume;
        user->addr.currentAddressHi = dsp->addr.currentAddressHi;
        user->addr.currentAddressLo = dsp->addr.currentAddressLo;
        return;
    }

    if (sync & AX_SYNC_USER_ALLPARAMS) {
        u32 *src = (u32 *)user;
        u32 *dst = (u32 *)dsp;
        u32 i;

        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        dst[6] = src[6];
        dst[7] = src[7];
        dst[8] = src[8];
        dst[9] = src[9];
        dst[10] = src[10];
        dst[11] = src[11];
        dst[12] = src[12];
        dst[13] = src[13];
        dst[14] = src[14];
        dst[15] = src[15];
        dst[16] = src[16];
        dst[17] = src[17];
        dst[18] = src[18];
        dst[19] = src[19];
        dst[20] = src[20];
        dst[21] = src[21];
        dst[22] = src[22];
        dst[23] = src[23];
        dst[24] = src[24];
        dst[25] = src[25];
        dst[26] = src[26];
        dst[27] = src[27];
        dst[28] = src[28];
        dst[29] = src[29];
        dst[30] = src[30];
        dst[31] = src[31];
        dst[32] = src[32];
        dst[33] = src[33];
        dst[34] = src[34];
        dst[35] = src[35];
        dst[36] = src[36];
        dst[37] = src[37];
        dst[38] = src[38];
        dst[39] = src[39];
        dst[40] = src[40];
        dst[41] = src[41];
        dst[42] = src[42];
        dst[43] = src[43];
        dst[44] = src[44];
        dst[45] = src[45];
        dst[46] = src[46];
        dst[47] = src[47];
        dst[48] = src[48];
        dst[49] = src[49];
        dst[50] = src[50];
        dst[51] = src[51];
        if (voice->updateCounter) {
            u32 *from = __AXUpdates[voice->index];
            u32 *to = (u32 *)voice->updateData;

            for (i = voice->updateCounter; i; i--) {
                *to++ = *from++;
            }
        }
        return;
    }

    if (sync & AX_SYNC_USER_SRCSELECT) {
        dsp->srcSelect = user->srcSelect;
        dsp->coefSelect = user->coefSelect;
    }
    if (sync & AX_SYNC_USER_MIXCTRL) {
        dsp->mixerCtrl = user->mixerCtrl;
    }
    if (sync & AX_SYNC_USER_STATE) {
        dsp->state = user->state;
    } else {
        user->state = dsp->state;
    }
    if (sync & AX_SYNC_USER_TYPE) {
        dsp->type = user->type;
    }
    if (sync & AX_SYNC_USER_MIX) {
        dsp->mix.vL = user->mix.vL;
        dsp->mix.vDeltaL = user->mix.vDeltaL;
        dsp->mix.vR = user->mix.vR;
        dsp->mix.vDeltaR = user->mix.vDeltaR;
        dsp->mix.vAuxAL = user->mix.vAuxAL;
        dsp->mix.vDeltaAuxAL = user->mix.vDeltaAuxAL;
        dsp->mix.vAuxAR = user->mix.vAuxAR;
        dsp->mix.vDeltaAuxAR = user->mix.vDeltaAuxAR;
        dsp->mix.vAuxBL = user->mix.vAuxBL;
        dsp->mix.vDeltaAuxBL = user->mix.vDeltaAuxBL;
        dsp->mix.vAuxBR = user->mix.vAuxBR;
        dsp->mix.vDeltaAuxBR = user->mix.vDeltaAuxBR;
        dsp->mix.vAuxBS = user->mix.vAuxBS;
        dsp->mix.vDeltaAuxBS = user->mix.vDeltaAuxBS;
        dsp->mix.vS = user->mix.vS;
        dsp->mix.vDeltaS = user->mix.vDeltaS;
        dsp->mix.vAuxAS = user->mix.vAuxAS;
        dsp->mix.vDeltaAuxAS = user->mix.vDeltaAuxAS;
    }
    if (sync & AX_SYNC_USER_ITDTARGET) {
        dsp->itd.targetShiftL = user->itd.targetShiftL;
        dsp->itd.targetShiftR = user->itd.targetShiftR;
    } else if (sync & AX_SYNC_USER_ITD) {
        u32 *itd;

        dsp->itd.flag = user->itd.flag;
        dsp->itd.bufferHi = user->itd.bufferHi;
        dsp->itd.bufferLo = user->itd.bufferLo;
        dsp->itd.shiftL = user->itd.shiftL;
        dsp->itd.shiftR = user->itd.shiftR;
        dsp->itd.targetShiftL = user->itd.targetShiftL;
        dsp->itd.targetShiftR = user->itd.targetShiftR;
        itd = voice->itdBuffer;
        itd[0] = 0;
        itd[1] = 0;
        itd[2] = 0;
        itd[3] = 0;
        itd[4] = 0;
        itd[5] = 0;
        itd[6] = 0;
        itd[7] = 0;
        itd[8] = 0;
        itd[9] = 0;
        itd[10] = 0;
        itd[11] = 0;
        itd[12] = 0;
        itd[13] = 0;
        itd[14] = 0;
        itd[15] = 0;
    }
    if (sync & AX_SYNC_USER_UPDATE) {
        dsp->update.updNum[0] = user->update.updNum[0];
        dsp->update.updNum[1] = user->update.updNum[1];
        dsp->update.updNum[2] = user->update.updNum[2];
        dsp->update.updNum[3] = user->update.updNum[3];
        dsp->update.updNum[4] = user->update.updNum[4];
        if (voice->updateCounter) {
            u32 *from = (u32 *)voice->updateData;
            u32 *to = __AXUpdates[voice->index];
            u32 i;

            for (i = voice->updateCounter; i; i--) {
                *to++ = *from++;
            }
        }
    }
    if (sync & AX_SYNC_USER_DPOP) {
        dsp->dpop.aL = user->dpop.aL;
        dsp->dpop.aAuxAL = user->dpop.aAuxAL;
        dsp->dpop.aAuxBL = user->dpop.aAuxBL;
        dsp->dpop.aR = user->dpop.aR;
        dsp->dpop.aAuxAR = user->dpop.aAuxAR;
        dsp->dpop.aAuxBR = user->dpop.aAuxBR;
        dsp->dpop.aS = user->dpop.aS;
        dsp->dpop.aAuxAS = user->dpop.aAuxAS;
        dsp->dpop.aAuxBS = user->dpop.aAuxBS;
    }
    if (sync & AX_SYNC_USER_VEDELTA) {
        user->ve.currentVolume = dsp->ve.currentVolume;
        dsp->ve.currentDelta = user->ve.currentDelta;
    } else if (sync & AX_SYNC_USER_VE) {
        dsp->ve.currentVolume = user->ve.currentVolume;
        dsp->ve.currentDelta = user->ve.currentDelta;
    }
    if (sync & AX_SYNC_USER_FIR) {
        dsp->fir.numCoefs = user->fir.numCoefs;
        dsp->fir.coefsHi = user->fir.coefsHi;
        dsp->fir.coefsLo = user->fir.coefsLo;
    }
    if (sync & (AX_SYNC_USER_LOOP | AX_SYNC_USER_LOOPADDR | AX_SYNC_USER_ENDADDR |
                AX_SYNC_USER_CURRADDR)) {
        if (sync & AX_SYNC_USER_LOOP) {
            dsp->addr.loopFlag = user->addr.loopFlag;
        }
        if (sync & AX_SYNC_USER_LOOPADDR) {
            *(u32 *)&dsp->addr.loopAddressHi = *(u32 *)&user->addr.loopAddressHi;
        }
        if (sync & AX_SYNC_USER_ENDADDR) {
            *(u32 *)&dsp->addr.endAddressHi = *(u32 *)&user->addr.endAddressHi;
        }
        if (sync & AX_SYNC_USER_CURRADDR) {
            *(u32 *)&dsp->addr.currentAddressHi = *(u32 *)&user->addr.currentAddressHi;
        } else {
            *(u32 *)&user->addr.currentAddressHi = *(u32 *)&dsp->addr.currentAddressHi;
        }
    } else if (sync & AX_SYNC_USER_ADDR) {
        *(u32 *)&dsp->addr.loopFlag = *(u32 *)&user->addr.loopFlag;
        *(u32 *)&dsp->addr.loopAddressHi = *(u32 *)&user->addr.loopAddressHi;
        *(u32 *)&dsp->addr.endAddressHi = *(u32 *)&user->addr.endAddressHi;
        *(u32 *)&dsp->addr.currentAddressHi = *(u32 *)&user->addr.currentAddressHi;
    } else {
        user->addr.currentAddressHi = dsp->addr.currentAddressHi;
        user->addr.currentAddressLo = dsp->addr.currentAddressLo;
    }
    if (sync & AX_SYNC_USER_ADPCM) {
        u32 *src = (u32 *)&user->adpcm;
        u32 *dst = (u32 *)&dsp->adpcm;

        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        dst[6] = src[6];
        dst[7] = src[7];
        dst[8] = src[8];
        dst[9] = src[9];
    }
    if (sync & AX_SYNC_USER_SRCRATIO) {
        dsp->src.ratioHi = user->src.ratioHi;
        dsp->src.ratioLo = user->src.ratioLo;
    } else if (sync & AX_SYNC_USER_SRC) {
        dsp->src.ratioHi = user->src.ratioHi;
        dsp->src.ratioLo = user->src.ratioLo;
        dsp->src.currentAddressFrac = user->src.currentAddressFrac;
        dsp->src.last_samples[0] = user->src.last_samples[0];
        dsp->src.last_samples[1] = user->src.last_samples[1];
        dsp->src.last_samples[2] = user->src.last_samples[2];
        dsp->src.last_samples[3] = user->src.last_samples[3];
    }
    if (sync & AX_SYNC_USER_ADPCMLOOP) {
        dsp->adpcmLoop.loop_pred_scale = user->adpcmLoop.loop_pred_scale;
        dsp->adpcmLoop.loop_yn1 = user->adpcmLoop.loop_yn1;
        dsp->adpcmLoop.loop_yn2 = user->adpcmLoop.loop_yn2;
    }
    if (sync & AX_SYNC_USER_LPF_COEF) {
        dsp->lpf.a0 = user->lpf.a0;
        dsp->lpf.b0 = user->lpf.b0;
    } else if (sync & AX_SYNC_USER_LPF) {
        dsp->lpf.on = user->lpf.on;
        dsp->lpf.yn1 = user->lpf.yn1;
        dsp->lpf.a0 = user->lpf.a0;
        dsp->lpf.b0 = user->lpf.b0;
    }
}

void __AXSyncPBs(u32 lessDspCycles) {
    u32 cycles;
    u32 i;
    AXVPB *voice;
    AXPB *dsp;

    __AXNumVoices = 0;
    DCInvalidateRange(__AXPB, sizeof(__AXPB));
    DCInvalidateRange(__AXITD, sizeof(__AXITD));
    cycles = __AXGetCommandListCycles() + 0xAA10 + lessDspCycles;

    for (i = 31; i; i--) {
        for (voice = __AXGetStackHead(i); voice; voice = voice->next) {
            if (voice->depop) {
                __AXDepopVoice(&__AXPB[voice->index]);
            }
            if (voice->pb.state == AX_PB_STATE_RUN || voice->updateCounter) {
                if (voice->pb.srcSelect != AX_SRC_TYPE_NONE) {
                    cycles += __AXSrcCycles[voice->pb.src.ratioHi];
                }
                if (voice->pb.lpf.on) {
                    cycles += 0x22B;
                }
                cycles += __AXMainMixCycles[voice->pb.mixerCtrl & 0xF] +
                          __AXAuxMixCycles[(voice->pb.mixerCtrl >> 4) & 0x1F] +
                          __AXAuxMixCycles[(voice->pb.mixerCtrl >> 9) & 0x1F] + 0x8C;
                if (__AXMaxDspCycles > cycles) {
                    __AXServiceVPB(voice);
                } else {
                    if ((dsp = &__AXPB[voice->index])->state == AX_PB_STATE_RUN) {
                        __AXDepopVoice(dsp);
                    }
                    dsp->update.updNum[4] = 0;
                    dsp->update.updNum[3] = 0;
                    dsp->update.updNum[2] = 0;
                    dsp->update.updNum[1] = 0;
                    dsp->update.updNum[0] = 0;
                    dsp->state = 0;
                    voice->pb.state = 0;
                    __AXPushCallbackStack(voice);
                }
            } else {
                __AXServiceVPB(voice);
            }
            voice->sync = 0;
            voice->depop = FALSE;
            voice->updateCounter = 0;
            voice->updateMS = 0;
            voice->updateWrite = voice->updateData;
        }
    }
    __AXRecDspCycles = cycles;

    for (voice = __AXGetStackHead(0); voice; voice = voice->next) {
        if (voice->depop) {
            __AXDepopVoice(&__AXPB[voice->index]);
        }
        voice->depop = FALSE;
        __AXPB[voice->index].update.updNum[4] = 0;
        __AXPB[voice->index].update.updNum[3] = 0;
        __AXPB[voice->index].update.updNum[2] = 0;
        __AXPB[voice->index].update.updNum[1] = 0;
        __AXPB[voice->index].update.updNum[0] = 0;
        __AXPB[voice->index].state = 0;
    }

    DCFlushRange(__AXPB, sizeof(__AXPB));
    DCFlushRange(__AXITD, sizeof(__AXITD));
    DCFlushRange(__AXUpdates, sizeof(__AXUpdates));
}

AXPB *__AXGetPBs(void) { return __AXPB; }

void __AXSetPBDefault(AXVPB *voice) {
    voice->pb.state = 0;
    voice->pb.itd.flag = 0;
    voice->sync = AX_SYNC_USER_LPF | AX_SYNC_USER_UPDATE | AX_SYNC_USER_ITD | AX_SYNC_USER_STATE;
    voice->updateCounter = 0;
    voice->updateMS = 0;
    voice->updateWrite = voice->updateData;
    voice->pb.update.updNum[4] = 0;
    voice->pb.update.updNum[3] = 0;
    voice->pb.update.updNum[2] = 0;
    voice->pb.update.updNum[1] = 0;
    voice->pb.update.updNum[0] = 0;
    voice->pb.lpf.on = 0;
}

#define OS_BUS_CLOCK (*(u32 *)0x800000F8)

void __AXVPBInit(void) {
    u32 i;
    AXPB *pb;
    u32 *itd;
    u32 *update;
    AXVPB *voice;
    u32 *p;

    __AXMaxDspCycles = OS_BUS_CLOCK / 400;
    __AXRecDspCycles = 0;

    p = (u32 *)__AXPB;
    i = sizeof(__AXPB) / 4;
    while (i--) {
        *p++ = 0;
    }
    p = (u32 *)__AXITD;
    i = sizeof(__AXITD) / 4;
    while (i--) {
        *p++ = 0;
    }
    p = (u32 *)__AXVPB;
    i = sizeof(__AXVPB) / 4;
    while (i--) {
        *p++ = 0;
    }

    for (i = 0; i < AX_MAX_VOICES; i++) {
        pb = &__AXPB[i];
        itd = __AXITD[i];
        update = __AXUpdates[i];
        voice = &__AXVPB[i];

        voice->index = i;
        voice->updateWrite = voice->updateData;
        voice->itdBuffer = itd;
        __AXSetPBDefault(voice);

        if (i == AX_MAX_VOICES - 1) {
            pb->nextHi = pb->nextLo = 0;
            voice->pb.nextHi = voice->pb.nextLo = 0;
        } else {
            voice->pb.nextHi = (u16)((u32)(pb + 1) >> 16);
            voice->pb.nextLo = (u16)(u32)(pb + 1);
            pb->nextHi = (u16)((u32)(pb + 1) >> 16);
            pb->nextLo = (u16)(u32)(pb + 1);
        }
        voice->pb.currHi = (u16)((u32)pb >> 16);
        voice->pb.currLo = (u16)(u32)pb;
        pb->currHi = (u16)((u32)pb >> 16);
        pb->currLo = (u16)(u32)pb;
        voice->pb.itd.bufferHi = (u16)((u32)itd >> 16);
        voice->pb.itd.bufferLo = (u16)(u32)itd;
        pb->itd.bufferHi = (u16)((u32)itd >> 16);
        pb->itd.bufferLo = (u16)(u32)itd;
        voice->pb.update.dataHi = (u16)((u32)update >> 16);
        voice->pb.update.dataLo = (u16)(u32)update;
        pb->update.dataHi = (u16)((u32)update >> 16);
        pb->update.dataLo = (u16)(u32)update;
        voice->priority = 1;
        __AXPushFreeStack(voice);
    }
    DCFlushRange(__AXPB, sizeof(__AXPB));
}

void __AXVPBQuit(void) {}

void AXSetVoiceSrcType(AXVPB *voice, u32 type) {
    BOOL enabled = OSDisableInterrupts();
    AXPB *pb = &voice->pb;

    switch (type) {
    case 0:
        pb->srcSelect = 2;
        break;
    case 1:
        pb->srcSelect = 1;
        break;
    case 2:
        pb->srcSelect = 0;
        pb->coefSelect = 0;
        break;
    case 3:
        pb->srcSelect = 0;
        pb->coefSelect = 1;
        break;
    case 4:
        pb->srcSelect = 0;
        pb->coefSelect = 2;
        break;
    }
    voice->sync |= AX_SYNC_USER_SRCSELECT;
    OSRestoreInterrupts(enabled);
}

void AXSetVoiceState(AXVPB *voice, u16 state) {
    BOOL enabled = OSDisableInterrupts();

    voice->pb.state = state;
    voice->sync |= AX_SYNC_USER_STATE;
    if (state == 0) {
        voice->depop = TRUE;
    }
    OSRestoreInterrupts(enabled);
}

/* Copies the mix and works out which mixer paths are live (non-zero volume or ramp). */
void AXSetVoiceMix(AXVPB *voice, AXPBMIX *mix) {
    u32 mixerCtrl = 0;
    u16 *dst = &voice->pb.mix.vL;
    u16 *src = &mix->vL;
    BOOL enabled = OSDisableInterrupts();

    if ((*dst++ = *src++)) mixerCtrl |= 0x1;
    if ((*dst++ = *src++)) mixerCtrl |= 0x9;
    if ((*dst++ = *src++)) mixerCtrl |= 0x2;
    if ((*dst++ = *src++)) mixerCtrl |= 0xA;
    if ((*dst++ = *src++)) mixerCtrl |= 0x10;
    if ((*dst++ = *src++)) mixerCtrl |= 0x50;
    if ((*dst++ = *src++)) mixerCtrl |= 0x20;
    if ((*dst++ = *src++)) mixerCtrl |= 0x60;
    if ((*dst++ = *src++)) mixerCtrl |= 0x200;
    if ((*dst++ = *src++)) mixerCtrl |= 0xA00;
    if ((*dst++ = *src++)) mixerCtrl |= 0x400;
    if ((*dst++ = *src++)) mixerCtrl |= 0xC00;
    if ((*dst++ = *src++)) mixerCtrl |= 0x1000;
    if ((*dst++ = *src++)) mixerCtrl |= 0x3000;
    if ((*dst++ = *src++)) mixerCtrl |= 0x4;
    if ((*dst++ = *src++)) mixerCtrl |= 0xC;
    if ((*dst++ = *src++)) mixerCtrl |= 0x80;
    if ((*dst = *src)) mixerCtrl |= 0x180;

    voice->pb.mixerCtrl = mixerCtrl;
    voice->sync |= AX_SYNC_USER_MIX | AX_SYNC_USER_MIXCTRL;
    OSRestoreInterrupts(enabled);
}

/* PCM formats get a unity-gain decoder setup instead of ADPCM coefficients. */
void AXSetVoiceAddr(AXVPB *voice, AXPBADDR *addr) {
    u32 *dst = (u32 *)&voice->pb.addr;
    u32 *src = (u32 *)addr;
    BOOL enabled = OSDisableInterrupts();

    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];

    switch (addr->format) {
    case 0x0:
        break;
    case 0xA:
        dst[4] = 0;
        dst[5] = 0;
        dst[6] = 0;
        dst[7] = 0;
        dst[8] = 0;
        dst[9] = 0;
        dst[10] = 0;
        dst[11] = 0;
        dst[12] = 0x08000000;
        dst[13] = 0;
        break;
    case 0x19:
        dst[4] = 0;
        dst[5] = 0;
        dst[6] = 0;
        dst[7] = 0;
        dst[8] = 0;
        dst[9] = 0;
        dst[10] = 0;
        dst[11] = 0;
        dst[12] = 0x01000000;
        dst[13] = 0;
        break;
    }
    voice->sync &= ~(AX_SYNC_USER_LOOP | AX_SYNC_USER_LOOPADDR | AX_SYNC_USER_ENDADDR |
                     AX_SYNC_USER_CURRADDR);
    voice->sync |= AX_SYNC_USER_ADDR | AX_SYNC_USER_ADPCM;
    OSRestoreInterrupts(enabled);
}

void AXSetVoiceAdpcm(AXVPB *voice, AXPBADPCM *adpcm) {
    u32 *dst = (u32 *)&voice->pb.adpcm;
    u32 *src = (u32 *)adpcm;
    BOOL enabled = OSDisableInterrupts();

    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
    dst[5] = src[5];
    dst[6] = src[6];
    dst[7] = src[7];
    dst[8] = src[8];
    dst[9] = src[9];
    voice->sync |= AX_SYNC_USER_ADPCM;
    OSRestoreInterrupts(enabled);
}

void AXSetVoiceSrc(AXVPB *voice, AXPBSRC *src) {
    u16 *dst = (u16 *)&voice->pb.src;
    u16 *from = (u16 *)src;
    BOOL enabled = OSDisableInterrupts();

    dst[0] = from[0];
    dst[1] = from[1];
    dst[2] = from[2];
    dst[3] = from[3];
    dst[4] = from[4];
    dst[5] = from[5];
    dst[6] = from[6];
    voice->sync &= ~AX_SYNC_USER_SRCRATIO;
    voice->sync |= AX_SYNC_USER_SRC;
    OSRestoreInterrupts(enabled);
}

/* `ratio` is the playback rate relative to 32 kHz, at most 4. */
void AXSetVoiceSrcRatio(AXVPB *voice, f32 ratio) {
    u32 r;
    BOOL enabled = OSDisableInterrupts();

    ratio = 65536.0f * ratio;
    r = (u32)ratio;
    if ((u32)ratio > 0x40000) {
        r = 0x40000;
    }
    voice->pb.src.ratioHi = (u16)(r >> 16);
    voice->pb.src.ratioLo = (u16)r;
    voice->sync |= AX_SYNC_USER_SRCRATIO;
    OSRestoreInterrupts(enabled);
}

void AXSetVoiceAdpcmLoop(AXVPB *voice, AXPBADPCMLOOP *loop) {
    u16 *dst = (u16 *)&voice->pb.adpcmLoop;
    u16 *src = (u16 *)loop;
    BOOL enabled = OSDisableInterrupts();

    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    voice->sync |= AX_SYNC_USER_ADPCMLOOP;
    OSRestoreInterrupts(enabled);
}
