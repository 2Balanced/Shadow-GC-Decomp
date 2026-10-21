/* AdvReal::MemoryCheck::Phase (class name from RTTI): the front-end phase that checks the memory
 * card at start-up. Its checker opens the card in slot A (fn_802E472C with 3), waits for the check
 * to finish, records whether the card is usable and moves on; while the card is missing it follows
 * the selected language. The file name, the checker's class name and all function and member names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/scoped_ptr.hpp>

#include "Game/AdvBase.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The memory card object the checker owns (class name unknown; constructor at 0x802E472C,
 * destructor at 0x802E46E0, size from the allocation). */
class UnkMemoryCard_802E472C {
  public:
    UnkMemoryCard_802E472C(int slots);
    ~UnkMemoryCard_802E472C();

    /* 0x0 */ u8 field_0x0[0x8];
};
SIZE_ASSERT(UnkMemoryCard_802E472C, 0x8);

/* The game state (struct name unknown; see Game/CityBombSmall.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x58];
    /* 0x058 */ s32 m_language; /* (name inferred) */
    /* 0x05C */ u8 field_0x5C[0x7D - 0x5C];
    /* 0x07D */ u8 m_cardUsable; /* (name inferred) */
    /* 0x07E */ u8 field_0x7E[0x150 - 0x7E];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

class UnkManager_801E5D48;
class UnkManager_801E5E00;

extern "C" {
/* The memory card object's check: start it, whether it has finished, and its drawing (behaviour
 * inferred). */
void fn_802E3E7C(UnkMemoryCard_802E472C *card);
bool fn_802E3E34(UnkMemoryCard_802E472C *card);
void fn_802E3E44(UnkMemoryCard_802E472C *card);
/* Whether the checked card can be used (behaviour inferred). */
bool fn_802E4784(void);

/* Screen fades (behaviour inferred). */
void fn_80173834(int arg0, float time);
void fn_80173878(int arg0, int arg1, float time);

/* Language-dependent resources: the manager and its two reload calls (behaviour inferred). */
UnkManager_801E5D48 *fn_801E5D48(void);
void fn_802BEF78(UnkManager_801E5D48 *manager);
void fn_802BEFB4(UnkManager_801E5D48 *manager);

/* The phase manager and the request for phase id (behaviour inferred). */
UnkManager_801E5E00 *fn_801E5E00(void);
void fn_802B7528(UnkManager_801E5E00 *manager, int id);

/* Whether the fade has finished and the menu answer (behaviour inferred). */
void fn_80117514(void);
bool fn_802E5810(void);
int fn_80044E10(void);
}

namespace AdvReal {
namespace MemoryCheck {

/* The phase's state (class name inferred). */
class Checker {
  public:
    Checker();
    ~Checker() {
        if (m_card != 0) {
            delete m_card;
        }
    }

    /* Runs the check; always returns 1 (name inferred). */
    int Tick();
    /* Draws the card's messages (name inferred). */
    void Draw() {
        if (m_card != 0) {
            fn_802E3E44(m_card);
        }
    }

    /* 0x0 */ s32 m_state; /* 0 checking, 1 done */
    /* 0x4 */ UnkMemoryCard_802E472C *m_card;
    /* 0x8 */ s32 m_language;
};
SIZE_ASSERT(Checker, 0xC);

Checker::Checker() {
    m_state = 0;
    fn_80173834(0, 0.0f);
    m_card = new UnkMemoryCard_802E472C(3);
    m_language = lbl_8057D6A0.m_language;
}

int Checker::Tick() {
    if (m_card != 0) {
        switch (m_state) {
        case 0:
            fn_802E3E7C(m_card);
            if (fn_802E3E34(m_card)) {
                if (fn_802E4784()) {
                    lbl_8057D6A0.m_cardUsable = 1;
                } else {
                    lbl_8057D6A0.m_cardUsable = 0;
                }
                m_state = 1;
                fn_80173878(1, 1, 1.0f);
            } else if (m_language != lbl_8057D6A0.m_language) {
                /* Retail holds m_language in r4 and the game state's language in r0 here;
                 * GC/2.7 swaps them to r0 and r3 for every shape tried (a local, a pointer or
                 * reference to the state, an inline getter or helper, early-out control flow),
                 * so this function stays at 99.65%. */
                m_language = lbl_8057D6A0.m_language;
                fn_802BEF78(fn_801E5D48());
                fn_802BEFB4(fn_801E5D48());
            }
            break;
        case 1:
            fn_80117514();
            if (fn_802E5810() && fn_80044E10() == 1) {
                fn_802B7528(fn_801E5E00(), 10);
            }
            break;
        }
    } else {
        fn_802B7528(fn_801E5E00(), 10);
    }
    return 1;
}

class Phase : public AdvBase {
  public:
    Phase();
    virtual ~Phase();
    virtual int Tick();
    virtual int Draw();

    /* 0x4 */ boost::scoped_ptr<Checker> m_checker;
};
SIZE_ASSERT(Phase, 0x8);

Phase::Phase() : m_checker(new Checker) {}

Phase::~Phase() {}

int Phase::Tick() {
    return m_checker->Tick();
}

int Phase::Draw() {
    m_checker->Draw();
    return 1;
}

} // namespace MemoryCheck
} // namespace AdvReal
