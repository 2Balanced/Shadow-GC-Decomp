/* The front-end's message source (class name unknown; the game reaches its one object through
 * fn_801E5D48, see Game/AdvMemoryCheck.cpp): the "Advertise" messages the menus print, kept as
 * the TMessageDisp the hint manager and the message window show messages from. The menus open
 * and close it, colour its font and print messages at a CSD node or at a point of the screen.
 * The file name and every name are inferred; the functions keep their fn_ names, which the
 * front-end units call. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/CsdRCPtr.h"
#include "Game/UnkHintManager.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* RenderWare's colour (rw/rwcore.h). */
typedef struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} RwRGBA;

/* The message source (class name unknown; see Game/StageManager.cpp). */
class UnkHintTarget_801FB180 {
  public:
    /* 0x00 */ u8 field_0x0[0x58];
    /* 0x58 */ void *m_font;
    /* 0x5C */ u8 field_0x5C[0x64 - 0x5C];
    /* 0x64 */ float m_scale;
    /* 0x68 */ u8 field_0x68[0x70 - 0x68];
    /* 0x70 */ s32 m_align; /* 1 or 2 (meaning inferred) */
};

/* TMessageDisp (class name from the RTTI of its shared_ptr control block; see
 * Game/StageManager.cpp). Its constructor (0x8004ABD0) and destructor (0x8004AB88) keep their fn_
 * names. */
class TMessageDisp : public UnkHintTarget_801FB180 {
  public:
    /* 0x74 */
};

/* The message source of the menus (class name unknown; constructed by fn_801E5D48). */
class UnkManager_801E5D48 {
  public:
    UnkManager_801E5D48();

    /* 0x0 */ boost::shared_ptr<UnkHintTarget_801FB180> m_message;
    /* 0x8 */ float m_scale; /* of the messages */
};

/* A node's position (struct name unknown; see Game/SavePointSelect.cpp). */
struct UnkCsdPosition_8036AE98 {
    float x;
    float y;
};

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x48];
    /* 0x048 */ s32 m_screenHeight; /* (meaning inferred) */
    /* 0x04C */ u8 field_0x4C[0x150 - 0x4C];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The empty node pointer (as lbl_805EE110 is the empty texture list). */
extern Chao::CSD::RCPtr<Chao::CSD::CNode> lbl_805EE11C;

class UnkObject_802E5B2C;

UnkHintManager_801FB180 *GetHintManager();

extern "C" {
TMessageDisp *fn_8004ABD0(TMessageDisp *message, const char *name, int arg2);
void fn_8004AB88(TMessageDisp *message, int free);
/* The hint manager: detaching its message source, and attaching one (see Game/StageManager.cpp). */
void fn_801FAFFC(UnkHintManager_801FB180 *manager);
void fn_801FB0BC(UnkHintManager_801FB180 *manager,
                 const boost::weak_ptr<UnkHintTarget_801FB180> &target);
/* The message window: detaching its message source, and attaching one. */
UnkObject_802E5B2C *fn_80117514(void);
void fn_802E5958(UnkObject_802E5B2C *window);
void fn_802E597C(UnkObject_802E5B2C *window, const boost::weak_ptr<UnkHintTarget_801FB180> &target);
/* Prints page index of message id at x, y (see Game/SaveCycleMessage.cpp). */
void *fn_80049BC0(UnkHintTarget_801FB180 *target, s32 id, s32 branch, s32 index, float x, float y,
                  s32 *voice, u8 changed);
/* The font's corner colours (see Game/SaveCycleMessage.cpp). */
void fn_80470694(void *font, const RwRGBA *topLeft, const RwRGBA *topRight,
                 const RwRGBA *bottomLeft, const RwRGBA *bottomRight);
UnkCsdPosition_8036AE98 fn_8036AE98(Chao::CSD::CNode *node);

bool fn_802BEF78(UnkManager_801E5D48 *self);
bool fn_802BEFB4(UnkManager_801E5D48 *self);
}

namespace boost {

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line (boost
 * 1.32's definition). */
template <class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}

/* TMessageDisp's destructor keeps its fn_ name. */
template <> inline void checked_delete<TMessageDisp>(TMessageDisp *x) {
    fn_8004AB88(x, 1);
}

} // namespace boost

namespace Chao {
namespace CSD {

/* The CSD pointers' comparison (name inferred; see Game/ResultTask.cpp). */
template <class T> inline int operator==(const RCPtr<T> &a, const RCPtr<T> &b) {
    if (a.m_object != 0 && b.m_object != 0) {
        return a.m_object->m_object == b.m_object->m_object;
    }
    return a.m_object == b.m_object;
}

} // namespace CSD
} // namespace Chao

/* The font colours: white, black, yellow, red, green and blue. */
static RwRGBA s_colors[6] = {
    {0xFF, 0xFF, 0xFF, 0xFF}, {0x00, 0x00, 0x00, 0xFF}, {0xFF, 0xFF, 0x00, 0xFF},
    {0xFF, 0x00, 0x00, 0xFF}, {0x00, 0xFF, 0x00, 0xFF}, {0x00, 0x00, 0xFF, 0xFF},
};

UnkManager_801E5D48::UnkManager_801E5D48() {}

/* Opens the messages for the menus (name inferred). */
extern "C" void fn_802BF1BC(UnkManager_801E5D48 *self) {
    fn_802BEFB4(self);
    fn_801FB0BC(GetHintManager(), boost::weak_ptr<UnkHintTarget_801FB180>(self->m_message));
    self->m_scale = 0.04f;
}

/* Closes them (name inferred). */
extern "C" void fn_802BF184(UnkManager_801E5D48 *self) {
    fn_802E5958(fn_80117514());
    fn_802BEF78(self);
}

/* Loads the "Advertise" messages and gives them to the message window; always returns true. */
extern "C" bool fn_802BEFB4(UnkManager_801E5D48 *self) {
    self->m_message.reset();
    TMessageDisp *message = (TMessageDisp *)operator new(0x74);
    if (message != 0) {
        message = fn_8004ABD0(message, "Advertise", 1);
    }
    self->m_message = boost::shared_ptr<UnkHintTarget_801FB180>(message);
    fn_802E597C(fn_80117514(), boost::weak_ptr<UnkHintTarget_801FB180>(self->m_message));
    return true;
}

/* Releases the messages; always returns true. */
extern "C" bool fn_802BEF78(UnkManager_801E5D48 *self) {
    fn_801FAFFC(GetHintManager());
    self->m_message.reset();
    return true;
}

/* Prints message id at (x, y), in 512 x height screen units (meaning inferred). */
extern "C" bool fn_802BEEC8(UnkManager_801E5D48 *self, s32 id, s32 branch, float x, float y,
                            bool right) {
    if (self->m_message.get() != 0) {
        self->m_message->m_scale = self->m_scale;
        if (right) {
            self->m_message->m_align = 2;
        } else {
            self->m_message->m_align = 1;
        }
        fn_80049BC0(self->m_message.get(), id, branch, 0, x / 512.0f,
                    1.0f - y / lbl_8057D6A0.m_screenHeight, 0, 0);
        return true;
    }
    return false;
}

/* Prints message id at the position of node. */
extern "C" bool fn_802BED7C(UnkManager_801E5D48 *self, s32 id, s32 branch,
                            const Chao::CSD::RCPtr<Chao::CSD::CNode> &node, bool right) {
    if (node == lbl_805EE11C) {
        return false;
    }
    if (self->m_message.get() != 0) {
        UnkCsdPosition_8036AE98 position = fn_8036AE98(node.Get());
        self->m_message->m_scale = self->m_scale;
        if (right) {
            self->m_message->m_align = 2;
        } else {
            self->m_message->m_align = 1;
        }
        fn_80049BC0(self->m_message.get(), id, branch, 0, position.x / 512.0f,
                    1.0f - position.y / lbl_8057D6A0.m_screenHeight, 0, 0);
        return true;
    }
    return false;
}

/* Sets the font colour, faded towards black by alpha. */
extern "C" bool fn_802BEC5C(UnkManager_801E5D48 *self, int color, float alpha) {
    if (self->m_message.get() != 0 && self->m_message->m_font != 0) {
        void *font = self->m_message->m_font;
        if (alpha >= 1.0f) {
            RwRGBA *c = &s_colors[color];
            fn_80470694(font, c, c, c, c);
        } else {
            RwRGBA c;
            c.red = alpha * s_colors[color].red;
            c.green = alpha * s_colors[color].green;
            c.blue = alpha * s_colors[color].blue;
            c.alpha = 0xFF;
            fn_80470694(font, &c, &c, &c, &c);
        }
        return true;
    }
    return false;
}
