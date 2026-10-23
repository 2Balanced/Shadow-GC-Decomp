/* The number display the front end keeps in a function-local static (class name unknown; see
 * Game/AdvFolder.cpp, whose copy of the class declaration this is): fn_80353A90 loads the
 * texture dictionary "number.txd" and a TGiNumberDisplay with its font "ad_hpasc1", fn_80353A70
 * drops both. The file name and the method names are inferred; the functions other units call keep
 * their fn_ names. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define TGI_NO_NUMBER_COLORS
#include "Game/TGi.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* The texture dictionary manager, loading a dictionary through it, and freeing a dictionary
 * (behaviour inferred; see Game/TextureRasterCache.cpp). */
void *fn_80014534(void);
void *fn_80039650(void *manager, const char *name);
void fn_802E0600(void *dictionary);
}

namespace boost {

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line (boost
 * 1.32's definition). */
template <class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}

} // namespace boost

class UnkDisplayHolder_80353AB0 {
  public:
    /* Game/AdvFolder.cpp has it inline; retail keeps this unit's copy (0x80353AB0). */
    UnkDisplayHolder_80353AB0();

    /* Drops the display and the dictionary. */
    bool Release();
    /* Loads the dictionary and the display; false when the dictionary is missing. */
    bool Load();

    /* 0x0 */ boost::shared_ptr<TGiNumberDisplay> m_display;
    /* 0x8 */ void *field_0x8; /* the texture dictionary */
};
SIZE_ASSERT(UnkDisplayHolder_80353AB0, 0xC);

UnkDisplayHolder_80353AB0::UnkDisplayHolder_80353AB0() : field_0x8(0) {}

extern "C" void fn_80353A90(UnkDisplayHolder_80353AB0 *holder) {
    holder->Load();
}

extern "C" void fn_80353A70(UnkDisplayHolder_80353AB0 *holder) {
    holder->Release();
}

bool UnkDisplayHolder_80353AB0::Load() {
    field_0x8 = fn_80039650(fn_80014534(), "number.txd");
    if (field_0x8 == 0) {
        return false;
    }
    m_display = boost::shared_ptr<TGiNumberDisplay>(new TGiNumberDisplay);
    fn_800BCEA8(m_display.get(), field_0x8, "ad_hpasc1");
    fn_800BCC54(m_display.get(), 64, 64, 16, 18, 4);
    return true;
}

bool UnkDisplayHolder_80353AB0::Release() {
    m_display.reset();
    if (field_0x8 != 0) {
        fn_802E0600(field_0x8);
        field_0x8 = 0;
    }
    return true;
}
