/* Dolphin SDK AXFX: the memory hooks the effects allocate their delay lines through. They default
 * to the current OS heap; the game installs its own pair. */
#include <dolphin/types.h>

/* The RenderWare units call this one by its placeholder symbol. */
#define AXFXSetHooks fn_80389320

typedef void *(*AXFXAllocHook)(u32 size);
typedef void (*AXFXFreeHook)(void *block);

extern volatile int __OSCurrHeap;
void *OSAllocFromHeap(int heap, u32 size);
void OSFreeToHeap(int heap, void *block);

static void *__AXFXAllocFunction(u32 size);
static void __AXFXFreeFunction(void *block);

AXFXAllocHook __AXFXAlloc = __AXFXAllocFunction;
AXFXFreeHook __AXFXFree = __AXFXFreeFunction;

static void *__AXFXAllocFunction(u32 size) { return OSAllocFromHeap(__OSCurrHeap, size); }

static void __AXFXFreeFunction(void *block) { OSFreeToHeap(__OSCurrHeap, block); }

void AXFXSetHooks(AXFXAllocHook alloc, AXFXFreeHook free) {
    __AXFXAlloc = alloc;
    __AXFXFree = free;
}
