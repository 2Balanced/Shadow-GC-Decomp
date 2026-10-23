/* Pad input helpers of the front-end phases (file name inferred; the game does not name this
 * code). fn_802B7D00 and fn_802B7D5C test a menu button through the button table below, and only
 * while the save message object allows input; fn_802B7CC0 and fn_802B7CE0 forward to two pad
 * functions that return 0. All function names are unknown and the behaviour is inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/UnkSingletons.h"
#include <dolphin/types.h>

extern "C" {
/* Whether the save message object currently lets the player act (see SaveCycleMessage.cpp). */
bool fn_802E5810(UnkObject_802E5B2C *object);
/* Whether a button is held and whether it was pressed on a pad (see ResultTask.cpp). */
bool fn_80356190(int button, int pad);
bool fn_80356260(int button, int pad);
/* Two pad queries that always return 0. */
bool fn_803560B8(void);
bool fn_803560C0(void);
}

/* The pad button of each menu button. */
static const s32 lbl_804FE628[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

/* Whether menu button button was pressed on pad pad. */
extern "C" bool fn_802B7D5C(int button, int pad) {
    if (fn_802E5810(fn_80117514())) {
        return fn_80356260(lbl_804FE628[button], pad);
    }
    return false;
}

/* Whether menu button button is held on pad pad. */
extern "C" bool fn_802B7D00(int button, int pad) {
    if (fn_802E5810(fn_80117514())) {
        return fn_80356190(lbl_804FE628[button], pad);
    }
    return false;
}

extern "C" bool fn_802B7CE0(void) {
    return fn_803560C0();
}

extern "C" bool fn_802B7CC0(void) {
    return fn_803560B8();
}
