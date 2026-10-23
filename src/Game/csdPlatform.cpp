/* csdPlatform.cpp (file name inferred from the class CPlatformRW, which the library's assert
 * messages name, and its base CPlatform; the library's records name no file here): the platform
 * layer the Chao CSD library draws and prints through, which the game installs, and the chunk
 * identifiers of the project file format, which the platform sets up. All names are inferred.
 * Code generation follows Game/csdLoader.cpp. */
#include <dolphin/types.h>

#include "Game/CsdLib.h"

#pragma pool_data off

/* The MSL variable argument list (layout of the PowerPC EABI va_list; see
 * Game/PjsEventActionManager.cpp). */
typedef struct __va_list_struct {
    char gpr;
    char fpr;
    char reserved[2];
    char *input_arg_area;
    char *reg_save_area;
} va_list[1];
#define va_start(ap, fmt) ((void)fmt, __builtin_va_info(&ap))
#define va_end(ap) ((void)0)

extern "C" {
/* MSL vsnprintf (the symbol is still a placeholder). */
int fn_803A9350(char *s, u32 n, const char *format, va_list arg);
}

namespace Chao {
namespace CSD {

/* The chunk identifiers of a project file ('NGIF', 'NGTL', ...; set by the platform). */
#pragma pool_data on
u32 g_chunkInfo;
u32 g_chunkTexList;
u32 g_chunkObject;
u32 g_chunkMotion;
u32 g_chunkMM;
u32 g_chunkMC;
u32 g_chunkML;
u32 g_chunkMA;
u32 g_chunkLI;
u32 g_chunkCA;
u32 g_chunkMT;
u32 g_chunkNN;
u32 g_chunkOffsets;
u32 g_chunkEnd;

static CPlatform *s_platform;
#pragma pool_data off

void Terminate() {
    Shutdown();
}

} // namespace CSD
} // namespace Chao

using namespace Chao::CSD;

/* Prints an error message and stops. */
extern "C" void fn_80361D18(const char *format, ...) {
    char buffer[0x100];
    va_list args;
    va_start(args, format);
    fn_803A9350(buffer, 0x100, format, args);
    va_end(args);
    s_platform->Print(buffer);
    for (;;) {
    }
}

/* Prints a message. */
extern "C" void fn_80361DC4(const char *format, ...) {
    char buffer[0x100];
    va_list args;
    va_start(args, format);
    fn_803A9350(buffer, 0x100, format, args);
    va_end(args);
    s_platform->Print(buffer);
}

namespace Chao {
namespace CSD {

void EndPlatform(CPlatform *platform) {
    platform->m_beginCount--;
    if (platform->m_beginCount == 0) {
        platform->End();
    }
}

void BeginPlatform(CPlatform *platform) {
    if (platform->m_beginCount == 0) {
        platform->Begin();
    }
    platform->m_beginCount++;
}

CPlatform *GetPlatform() {
    return s_platform;
}

void SetPlatform(CPlatform *platform) {
    s_platform = platform;
}

} // namespace CSD
} // namespace Chao
