/* TGiHissatu (class name from RTTI): the heads-up display of Shadow's special attacks ("hissatsu"),
 * Chaos Control on the hero side and Chaos Blast on the dark side. Each plays a TPjsCsd project
 * ("CsdFiles/Hissatu_Hero", "CsdFiles/Hissatu_Dark") and opens while the player's special attack
 * state is that side's. The file name and the names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first).
 *
 * NonMatching: 22 of 25 functions and all the data match. GC/2.7 numbers the registers of Load's
 * loop counter and table pointer the other way round, and of Tick's variables differently (retail
 * also keeps the address of the side's state for the second test); in the destructor retail
 * copies the loop's zero (mr) where GC/2.7 loads it again (li), as in Game/TGiHdGage.cpp. */
#define TGI_NO_NUMBER_COLORS
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <string.h>

#include "Game/PjsCsd.h"
#include "Game/Render.h"
#include "Game/TGi.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* TPjsCsd (behaviour inferred): loading the project of a file, playing a motion, releasing the
 * project (see Game/PjsCsd.h and Game/CsdOneShot.cpp). */
void fn_8003FA08(TPjsCsd &csd, const char *file);
void fn_8003E388(TPjsCsd &csd, const char *motion, int arg2);
void fn_8003E490(TPjsCsd &csd);
/* The player's special attack state: 1 dark, 2 hero (behaviour inferred). */
int fn_8016EEC0(UnkPlayerInfo_80178B5C *player);
}

/* A side's project, scene and motions, and the special attack state that opens it (struct name
 * and member names inferred). */
struct UnkHissatuSide_804B52E4 {
    /* 0x00 */ const char *m_file;
    /* 0x04 */ const char *m_scene;
    /* 0x08 */ const char *m_open;
    /* 0x0C */ const char *m_loop;
    /* 0x10 */ const char *m_close;
    /* 0x14 */ s32 m_state;
};

static const UnkHissatuSide_804B52E4 s_sides[2] = {
    {"CsdFiles/Hissatu_Hero", "HissatuHero", "OpenHero", "Chaos_Control", "CloseHero", 2},
    {"CsdFiles/Hissatu_Dark", "HissatuDark", "OpenDark", "Chaos_Blast", "CloseDark", 1},
};

class TGiHissatu : public Task {
  public:
    /* A side on screen (struct name and member names inferred). */
    struct Side {
        /* 0x00 */ TPjsCsd m_csd;
        /* 0x54 */ u8 m_open;
    };

    TGiHissatu(Task *parent);
    virtual ~TGiHissatu();
    virtual void Tick(float dt);

    /* Loads the projects (name inferred). */
    void Load();
    /* The Render's draw callback (name inferred). */
    void Draw(RednerDrawArgs &args);

    /* 0x28 */ Side m_sides[2]; /* hero, dark */
    /* 0xD8 */ boost::shared_ptr<Render> m_render;
};
SIZE_ASSERT(TGiHissatu, 0xE0);

TGiHissatu::TGiHissatu(Task *parent) : Task(parent) {
    m_name = "TGiHissatu";
    Load();
    m_render = fn_8004E24C(fn_80046178()->field_0x38);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&TGiHissatu::Draw, this, _1));
}

TGiHissatu::~TGiHissatu() {
    for (int i = 0; i < 2; i++) {
        fn_8003E490(m_sides[i].m_csd);
    }
}

void TGiHissatu::Tick(float dt) {
    UnkPlayerInfo_80178B5C *player = fn_80178B5C(fn_80079808(), 0);
    if (player != 0) {
        for (int i = 0; i < 2; i++) {
            const UnkHissatuSide_804B52E4 *side = &s_sides[i];
            if (side->m_state == fn_8016EEC0(player) && !m_sides[i].m_open) {
                m_sides[i].m_open = 1;
                fn_8003E388(m_sides[i].m_csd, side->m_open, 1);
                fn_8003E388(m_sides[i].m_csd, side->m_loop, 0);
            } else if (side->m_state != fn_8016EEC0(player) && m_sides[i].m_open) {
                fn_8003FB10(m_sides[i].m_csd);
                m_sides[i].m_open = 0;
                fn_8003E388(m_sides[i].m_csd, side->m_loop, 1);
                fn_8003E388(m_sides[i].m_csd, side->m_close, 1);
            }
            fn_8003FE2C(m_sides[i].m_csd);
        }
    }
}

void TGiHissatu::Load() {
    for (int i = 0; i < 2; i++) {
        TPjsCsd &csd = m_sides[i].m_csd;
        const UnkHissatuSide_804B52E4 *side = &s_sides[i];
        fn_8003FA08(csd, side->m_file);
        strcpy(csd.m_sceneName, side->m_scene);
        fn_8003E388(csd, side->m_close, 1);
        fn_8003FB10(csd);
    }
}

void TGiHissatu::Draw(RednerDrawArgs &args) {
    if (lbl_8057D6A0.m_hudVisible) {
        for (int i = 0; i < 2; i++) {
            fn_8003FDF4(m_sides[i].m_csd);
        }
    }
}
