/* PJSPeripheral::ManagerImpGc (class name from RTTI; it derives from
 * Sonicteam::System::Peripheral::IManagerImp): the GameCube back end of the game's pad manager.
 * The pad setup (0x80349F28) creates one and hands it to the manager (0x8040D8DC), which reads
 * the pads through it every frame: four pads, each read as a connection state, the game's button
 * bits and the two sticks. Pads that are unplugged are reset one channel per read. File name from
 * the class; the method and member names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include <dolphin/types.h>
#include "size_assert.h"

/* The SDK's PADStatus, as PADRead fills it (as in Game/ProgressiveModeCheck.cpp). */
struct UnkPadStatus_803801FC {
    /* 0x0 */ u16 button;
    /* 0x2 */ s8 stickX;
    /* 0x3 */ s8 stickY;
    /* 0x4 */ s8 substickX;
    /* 0x5 */ s8 substickY;
    /* 0x6 */ u8 triggerL;
    /* 0x7 */ u8 triggerR;
    /* 0x8 */ u8 analogA;
    /* 0x9 */ u8 analogB;
    /* 0xA */ s8 err;
};

extern "C" {
/* The SDK's PADReset, PADRecalibrate and PADRead (identified by their use here). */
void fn_8037FE88(u32 mask);
void fn_8037FF98(u32 mask);
void fn_803801FC(UnkPadStatus_803801FC *status);
}

namespace Sonicteam {
namespace System {
namespace Peripheral {

/* One pad as the manager sees it (struct and member names inferred). */
struct PadState {
    /* 0x0 */ u32 m_state;   /* 0: connected, 1: no pad, 3: error */
    /* 0x4 */ u32 m_buttons; /* the game's button bits */
    /* 0x8 */ s16 m_stick[2];
    /* 0xC */ s16 m_substick[2];
};

/* The dead zone and range of the sticks (struct and member names inferred). */
struct StickRange {
    /* 0x0 */ s16 m_deadZone[2];
    /* 0x4 */ s16 m_range[2];
};

/* IManagerImp (class name from RTTI): what a pad back end does for the manager (names of the
 * virtual functions inferred). */
class IManagerImp {
  public:
    virtual ~IManagerImp() {}
    virtual void Initialize() = 0;
    virtual u32 GetPadCount() = 0;
    virtual bool Read(PadState *pads, u32 count) = 0;
    virtual bool GetStickRange(int pad, StickRange *range) = 0;
};

} // namespace Peripheral
} // namespace System
} // namespace Sonicteam

namespace PJSPeripheral {

using Sonicteam::System::Peripheral::PadState;
using Sonicteam::System::Peripheral::StickRange;

class ManagerImpGc : public Sonicteam::System::Peripheral::IManagerImp {
  public:
    ManagerImpGc();
    virtual ~ManagerImpGc();
    virtual void Initialize();
    virtual u32 GetPadCount();
    virtual bool Read(PadState *pads, u32 count);
    virtual bool GetStickRange(int pad, StickRange *range);

    /* 0x4 */ u16 field_0x4[4]; /* cleared, otherwise unused */
    /* 0xC */ s8 m_lastError[4];  /* the PADStatus error of the last read */
};
SIZE_ASSERT(ManagerImpGc, 0x10);

/* A pad button and the game's bit for it. */
struct ButtonMap {
    u16 pad;
    u16 game;
};

static const ButtonMap s_buttonMap[12] = {
    {0x0100, 0x0001}, /* A */
    {0x0200, 0x0002}, /* B */
    {0x0400, 0x0008}, /* X */
    {0x0800, 0x0010}, /* Y */
    {0x0008, 0x0040}, /* up */
    {0x0004, 0x0080}, /* down */
    {0x0001, 0x0100}, /* left */
    {0x0002, 0x0200}, /* right */
    {0x1000, 0x0400}, /* start */
    {0x0010, 0x0020}, /* Z */
    {0x0040, 0x1000}, /* L */
    {0x0020, 0x2000}, /* R */
};

/* The PADReset mask of each channel. */
static const u32 s_channelMasks[4] = {0x80000000, 0x40000000, 0x20000000, 0x10000000};

/* A stick value made symmetric about 0 (-127 to 127). */
inline s16 StickValue(s8 value) {
    int result = value;
    if (result < 0) {
        result++;
    }
    return result;
}

/* The sticks of status, with up positive in the game's sense. */
static void ConvertSticks(PadState *pad, const UnkPadStatus_803801FC *status) {
    pad->m_stick[0] = StickValue(status->stickX);
    pad->m_stick[1] = -StickValue(status->stickY);
    pad->m_substick[0] = StickValue(status->substickX);
    pad->m_substick[1] = -StickValue(status->substickY);
}

ManagerImpGc::ManagerImpGc() {
    for (int i = 0; i < 4; i++) {
        field_0x4[i] = 0;
        m_lastError[i] = -2;
    }
}

void ManagerImpGc::Initialize() {
    fn_8037FF98(0xFFFFFFFF);
}

u32 ManagerImpGc::GetPadCount() {
    return 4;
}

bool ManagerImpGc::Read(PadState *pads, u32 count) {
    /* The pad that is reset next while unplugged: a function-local static with a guard, written
     * out because GC/2.7 puts the guard before the object (see docs/matching-gadgets.md). */
    static u32 s_cursor;
    static s8 s_cursorInit;
    if (!s_cursorInit) {
        s_cursor = 0;
        s_cursorInit = 1;
    }
    UnkPadStatus_803801FC status[4];

    fn_803801FC(status);
    for (u32 i = 0; i < count; i++) {
        UnkPadStatus_803801FC *padStatus = &status[i];
        PadState *pad = &pads[i];
        switch (padStatus->err) {
        case 0: {
            pad->m_state = 0;
            u32 buttons = 0;
            for (int j = 0; j < 12; j++) {
                if (padStatus->button & s_buttonMap[j].pad) {
                    buttons |= s_buttonMap[j].game;
                }
            }
            pad->m_buttons = buttons;
            ConvertSticks(pad, padStatus);
            m_lastError[i] = padStatus->err;
            break;
        }
        case -1:
            pad->m_state = 1;
            if (i == s_cursor && m_lastError[i] != -1) {
                fn_8037FE88(s_channelMasks[i]);
            }
            break;
        case -2:
            pad->m_state = 0;
            pad->m_buttons = 0;
            pad->m_stick[0] = 0;
            pad->m_stick[1] = 0;
            pad->m_substick[0] = 0;
            pad->m_substick[1] = 0;
            m_lastError[i] = padStatus->err;
            break;
        case -3:
        default:
            pad->m_state = 3;
            m_lastError[i] = padStatus->err;
            break;
        }
    }
    s_cursor++;
    s_cursor = s_cursor < count ? s_cursor : 0;
    return true;
}

bool ManagerImpGc::GetStickRange(int pad, StickRange *range) {
    range->m_deadZone[0] = 15;
    range->m_deadZone[1] = 25;
    range->m_range[0] = 57 - range->m_deadZone[0];
    range->m_range[1] = 64 - range->m_deadZone[1];
    return true;
}

ManagerImpGc::~ManagerImpGc() {}

} // namespace PJSPeripheral
