/* RenderWare core: the engine (0x80485BCC-0x80486888).
 * The allocation pair used without free lists, _rwDeviceSystemRequest (inlined by every caller
 * after it), the open engine count, RwEngineGetVersion, plugin registration and offsets, the video
 * mode queries, RwEngineStop/Start/Close/Open/Term/Init (the core modules registered as engine
 * plugins). The file owns the engine count and RwEngineInstance (.sbss), the static engine globals
 * (.bss), the engine plugin registry and the subsystem name (.data 0x8056DFB0-0x8056DFE8).
 * NonMatching: fn_80485C3C (_rwDeviceSystemRequest), 2 rows: retail loads the 1 that request 13
 * stores separately from the TRUE it returns; GC/2.7 shares one register (nine spellings in
 * probes/Game_RwCpp48.py). C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

void fn_8047FA28(RwBool enable);
void *fn_8047FDB0(RwFreeList *list, u32 hint);
RwFreeList *fn_8047FF50(RwFreeList *list, void *entry);
RwBool fn_80480274(const RwMemoryFunctions *memFuncs);
void fn_8048038C(void);
RwBool fn_80481B98(void);
RwBool fn_80481C58(void);
s32 fn_80481D98(const RwPluginRegistry *reg, u32 pluginID);
s32 fn_80481DC8(RwPluginRegistry *reg, s32 size, u32 pluginID, RwPluginObjectConstructor constructCB,
                RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB);
const RwPluginRegistry *fn_8048209C(const RwPluginRegistry *reg, void *object);
const RwPluginRegistry *fn_80482144(const RwPluginRegistry *reg, void *object);
RwBool fn_80482E20(void);
void fn_80482F60(void);
RwBool fn_80489830(f32 gamma);
s32 fn_80493E64(void);
RwDevice *fn_804950A0(void);
s32 fn_8049B140(void);

/* The module open and close functions the engine registers as plugins. */
void *fn_8047DB10(void *instance, s32 offset, s32 size);
void *fn_8047DB24(void *instance, s32 offset, s32 size);
void *fn_8047DB38(void *instance, s32 offset, s32 size);
void *fn_8047DB70(void *instance, s32 offset, s32 size);
void *fn_8047E828(void *instance, s32 offset, s32 size);
void *fn_8047E88C(void *instance, s32 offset, s32 size);
void *fn_80480C6C(void *instance, s32 offset, s32 size);
void *fn_80480CF8(void *instance, s32 offset, s32 size);
void *fn_8048053C(void *instance, s32 offset, s32 size);
void *fn_80480690(void *instance, s32 offset, s32 size);
void *fn_80482548(void *instance, s32 offset, s32 size);
void *fn_804825EC(void *instance, s32 offset, s32 size);
void *fn_80482BC4(void *instance, s32 offset, s32 size);
void *fn_80482C50(void *instance, s32 offset, s32 size);
void *fn_804853C0(void *instance, s32 offset, s32 size);
void *fn_80485424(void *instance, s32 offset, s32 size);
void *fn_80486888(void *instance, s32 offset, s32 size);
void *fn_8048692C(void *instance, s32 offset, s32 size);
void *fn_80487914(void *instance, s32 offset, s32 size);
void *fn_80487BB0(void *instance, s32 offset, s32 size);
void *fn_80489FC0(void *instance, s32 offset, s32 size);
void *fn_8048A02C(void *instance, s32 offset, s32 size);
void *fn_8048D300(void *instance, s32 offset, s32 size);
void *fn_8048D4DC(void *instance, s32 offset, s32 size);
void *fn_8049AF94(void *instance, s32 offset, s32 size);
void *fn_8049AFF8(void *instance, s32 offset, s32 size);
void *fn_8049B0CC(void *instance, s32 offset, s32 size);
void *fn_8049B110(void *instance, s32 offset, s32 size);

RwGlobals *RwEngineInstance;
static s32 engineInstancesOpened;
static RwGlobals staticGlobals;
static RwPluginRegistry engineTKList = {sizeof(RwGlobals), sizeof(RwGlobals), 0, 0, NULL, NULL};

/* The engine's allocation when it runs without free lists: straight from the memory functions. */
void *fn_80485BCC(RwFreeList *list, u32 hint) {
    return RwEngineInstance->memoryFuncs.rwmalloc(list->entrySize, hint);
}

RwFreeList *fn_80485BFC(RwFreeList *list, void *entry) {
    RwEngineInstance->memoryFuncs.rwfree(entry);
    return list;
}

/* _rwDeviceSystemRequest: the device's system function, with defaults for the subsystem and
 * start/stop requests a device does not implement. */
static inline RwBool DeviceSystemRequest(RwDevice *device, s32 request, void *out, void *inOut,
                                         s32 number) {
    RwBool result = device->fpSystem(request, out, inOut, number);

    if (!result) {
        switch (request) {
        case 17:
        case 18:
            result = TRUE;
            break;
        case 13:
            *(s32 *)out = 1;
            result = TRUE;
            break;
        case 14:
            result = (number == 0);
            if (result) {
                RwEngineInstance->stringFuncs.vecStrcpy((char *)out, "Only rendering sub system");
            }
            break;
        case 15:
            *(s32 *)out = 0;
            result = TRUE;
            break;
        case 16:
            result = (number == 0);
            break;
        }
    }
    if (!result) {
        RWERROR((0x18, request));
    }
    return result;
}

RwBool fn_80485C3C(RwDevice *device, s32 request, void *out, void *inOut, s32 number) {
    return DeviceSystemRequest(device, request, out, inOut, number);
}

/* The number of engines opened and not yet closed. */
s32 fn_80485D64(void) {
    return engineInstancesOpened;
}

/* RwEngineGetVersion. */
u32 fn_80485D6C(void) {
    return 0x37002;
}

/* RwEngineRegisterPlugin. */
s32 fn_80485D78(s32 size, u32 pluginID, RwPluginObjectConstructor initCB,
                RwPluginObjectDestructor termCB) {
    return fn_80481DC8(&engineTKList, size, pluginID, initCB, termCB, NULL);
}

/* RwEngineGetPluginOffset. */
s32 fn_80485DB8(u32 pluginID) {
    return fn_80481D98(&engineTKList, pluginID);
}

/* RwEngineGetVideoModeInfo. */
void *fn_80485DE4(void *modeInfo, s32 modeIndex) {
    if (!DeviceSystemRequest(&RwEngineInstance->dOpenDevice, 6, modeInfo, NULL, modeIndex)) {
        modeInfo = NULL;
    }
    return modeInfo;
}

/* RwEngineGetCurrentVideoMode. */
s32 fn_80485E70(void) {
    s32 mode;

    if (DeviceSystemRequest(&RwEngineInstance->dOpenDevice, 10, &mode, NULL, 0)) {
        return mode;
    }
    return -1;
}

/* RwEngineStop. */
RwBool fn_80485EF4(void) {
    RwDevice *device = &RwEngineInstance->dOpenDevice;
    RwBool result;

    DeviceSystemRequest(device, 18, NULL, NULL, 0);
    fn_80482144(&engineTKList, RwEngineInstance);
    result = DeviceSystemRequest(device, 3, NULL, NULL, 0);
    if (result) {
        RwEngineInstance->engineStatus = 2;
    }
    return result;
}

/* RwEngineStart. */
RwBool fn_80485FE0(void) {
    RwDevice *device = &RwEngineInstance->dOpenDevice;

    if (DeviceSystemRequest(device, 2, NULL, NULL, 0)) {
        if (fn_8048209C(&engineTKList, RwEngineInstance)) {
            fn_80489830(RwEngineInstance->dOpenDevice.gammaCorrection);
            DeviceSystemRequest(device, 17, NULL, NULL, 0);
            RwEngineInstance->engineStatus = 3;
            return TRUE;
        }
        DeviceSystemRequest(device, 3, NULL, NULL, 0);
    }
    return FALSE;
}

/* RwEngineClose: the globals go back to the static block. */
RwBool fn_80486138(void) {
    RwBool result = DeviceSystemRequest(&RwEngineInstance->dOpenDevice, 1, NULL, NULL, 0);

    if (result) {
        RwGlobals *globals = RwEngineInstance;

        RwEngineInstance = &staticGlobals;
        memcpy(RwEngineInstance, globals, sizeof(RwGlobals));
        RwEngineInstance->memoryFuncs.rwfree(globals);
        engineInstancesOpened--;
        RwEngineInstance->engineStatus = 1;
    }
    return result;
}

/* The device part of RwEngineOpen: the globals move to a block of the size the engine plugins
 * need, the device registers and opens, and its standard functions are fetched. */
static inline RwBool EngineOpenDevice(RwDevice *device, void *initParams) {
    RwGlobals *globals;

    globals = RwEngineInstance = (RwGlobals *)RwEngineInstance->memoryFuncs.rwmalloc(
        engineTKList.sizeOfStruct, 0x40000);
    if (globals) {
        memcpy(RwEngineInstance, &staticGlobals, sizeof(RwGlobals));
        DeviceSystemRequest(device, 4, &RwEngineInstance->dOpenDevice, &RwEngineInstance->memoryFuncs,
                            0);
        if (DeviceSystemRequest(device, 0, NULL, initParams, 0)) {
            DeviceSystemRequest(device, 11, RwEngineInstance->stdFunc, NULL, 29);
            engineInstancesOpened++;
            return TRUE;
        }
        RwEngineInstance = &staticGlobals;
        memcpy(RwEngineInstance, globals, sizeof(RwGlobals));
        RwEngineInstance->memoryFuncs.rwfree(globals);
        return FALSE;
    }
    RwEngineInstance = &staticGlobals;
    RWERROR((0x80000013, engineTKList.sizeOfStruct));
    return FALSE;
}

/* RwEngineOpen. */
RwBool fn_80486204(void *initParams) {
    RwBool result;

    if (!RwEngineInstance) {
        RwEngineInstance = &staticGlobals;
    }
    result = (RwEngineInstance->engineStatus == 1);
    if (result) {
        result = (initParams != NULL);
        if (result) {
            RwDevice *device = fn_804950A0();

            result = (device != NULL);
            if (result) {
                result = EngineOpenDevice(device, initParams);
                if (result) {
                    RwEngineInstance->engineStatus = 2;
                }
            }
        } else {
            RWERROR((0x80000016));
        }
    } else {
        RWERROR((0x80000001));
    }
    return result;
}

/* RwEngineTerm. */
RwBool fn_804864AC(void) {
    RwBool result = (engineInstancesOpened == 0);

    if (result) {
        fn_80481C58();
        fn_8048038C();
        RwEngineInstance->engineStatus = 0;
    }
    return result;
}

/* The core modules, registered as engine plugins (their open and close functions). */
static inline RwBool EngineRegisterModules(void) {
    s32 status;

    status = fn_80481DC8(&engineTKList, 8, 0x40F, fn_8047DB38, fn_8047DB70, NULL);
    status |= fn_80481DC8(&engineTKList, 8, 0x401, fn_804825EC, fn_80482548, NULL);
    status |= fn_80481DC8(&engineTKList, 0, 0x40D, fn_8047DB10, fn_8047DB24, NULL);
    status |= fn_80481DC8(&engineTKList, 0x18, 0x402, fn_8047E88C, fn_8047E828, NULL);
    status |= fn_80481DC8(&engineTKList, 4, 0x403, fn_80486888, fn_8048692C, NULL);
    status |= fn_80481DC8(&engineTKList, 4, 0x404, fn_80480C6C, fn_80480CF8, NULL);
    status |= fn_80481DC8(&engineTKList, 4, 0x405, fn_80485424, fn_804853C0, NULL);
    status |= fn_80481DC8(&engineTKList, 0x220, 0x406, fn_80487914, fn_80487BB0, NULL);
    status |= fn_80481DC8(&engineTKList, 0x64, 0x407, fn_8048A02C, fn_80489FC0, NULL);
    status |= fn_80481DC8(&engineTKList, 0x34, 0x408, fn_8048D4DC, fn_8048D300, NULL);
    status |= fn_80481DC8(&engineTKList, 0x60, 0x409, fn_8049B0CC, fn_8049B110, NULL);
    status |= fn_80481DC8(&engineTKList, 4, 0x412, fn_80482BC4, fn_80482C50, NULL);
    status |= fn_8049B140();
    status |= fn_80481DC8(&engineTKList, 0x74, 0x40A, fn_8049AFF8, fn_8049AF94, NULL);
    status |= fn_80481DC8(&engineTKList, 0x28, 0x40B, fn_8048053C, fn_80480690, NULL);
    return (status >= 0);
}

/* RwEngineInit: the memory functions and free list mode, then the core modules as engine
 * plugins. */
RwBool fn_804864F8(const RwMemoryFunctions *memFuncs, u32 initFlags, u32 resArenaSize) {
    RwBool result = FALSE;

    RwEngineInstance = &staticGlobals;
    if (initFlags & 1) {
        RwEngineInstance->memoryAlloc = fn_80485BCC;
        RwEngineInstance->memoryFree = fn_80485BFC;
        fn_8047FA28(FALSE);
    } else {
        RwEngineInstance->memoryAlloc = fn_8047FDB0;
        RwEngineInstance->memoryFree = fn_8047FF50;
        fn_8047FA28(TRUE);
    }
    RwEngineInstance->resArenaInitSize = resArenaSize;
    if (RwEngineInstance->engineStatus == 0) {
        result = fn_80482E20();
        if (result) {
            result = fn_80480274(memFuncs);
            if (result) {
                result = fn_80481B98();
                if (result) {
                    result = EngineRegisterModules();
                    if (result) {
                        result = fn_80493E64();
                        if (result) {
                            RwEngineInstance->engineStatus = 1;
                            return result;
                        }
                    }
                    fn_80481C58();
                }
                fn_8048038C();
            }
            fn_80482F60();
        }
    }
    return result;
}
