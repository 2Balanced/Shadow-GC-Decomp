/* GiGameOver (class name from RTTI): the game-over screen. It plays the CSD project
 * "csdFiles/GameOver" (the scenes GameOver and Loop): GameOver opens with GameoverOpen (or straight
 * with ContinueOpen when the game state's byte 0xB2 is set), then shows ContinueOpen and the
 * yes/no selection, colouring the chosen answer, and closes when the choice is confirmed. Only one
 * exists at a time; the functions other units call (they keep their fn_ names) create it, pass
 * the choice and query it. File name from the class; names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CsdRCPtr.h"
#include "Game/Render.h"
/* This unit has no number displays (see Game/TGi.h). */
#define TGI_NO_NUMBER_COLORS
#include "Game/TGi.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CNode;
using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

/* The argument a Render passes to its draw callbacks (unused here). */
struct RednerDrawArgs;

extern "C" {
/* Where new tasks go (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* The path of a localized file, and loading a texture list file into resource (behaviour
 * inferred; see Game/TGiCommandWindow.cpp). */
const char *fn_800C02CC(const char *name);
void fn_800422A8(Chao::CSD::CProject *resource, const char *file);
/* The sound manager, and playing sound id through it (behaviour inferred). */
void *fn_80053164(void);
void fn_801D51A0(void *manager, int id, int arg2, int arg3);
/* Nodes (behaviour inferred): the colour of one of the four corners, and two colour settings. */
void fn_8036AC60(CNode *node, int corner, const u32 *color);
void fn_8036ACA8(CNode *node);
void fn_8036ACCC(CNode *node, const u32 *color);
}

/* Not original: an unused table that pools the unit's .rodata strings in retail's order (the
 * linker drops the table; the RTTI names and literals below reuse these copies). The .rodata is
 * kept whole (the constructor addresses it through one base), so it also holds the RTTI names of
 * the CSD and boost headers' inline classes, which this unit does not reconstruct. */
static const char *s_fileName = "csdFiles/GameOver";
static const char *s_rodataPool[] = {
    "boost::_bi::bind_t<void, boost::_mfi::mf1<void, GiGameOver, RednerDrawArgs &>, boost::_bi::list2<boost::_bi::value<GiGameOver *>, boost::arg<1>>>",
    "GiGameOver",
    "GameOver",
    "ContinueOpen",
    "Not inititalized m_eDealloctor\n",
    "Memory allocation failure",
    "csdBase.h",
    "GameoverOpen",
    "csdRCObject.h",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>::CFunctorBase",
    "Chao::CSD::CBase",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>",
    "Chao::CSD::CResourceBase<Chao::CSD::Project>",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>",
    "boost::detail::sp_counted_base",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>",
};

/* The corner colours of the answers: the chosen one and the other (names inferred). */
static u32 s_yesChosen = 0xFFFFFFFF;
static u32 s_noOther = 0x660000FF;
static u32 s_yesOther = 0x660000FF;
static u32 s_noChosen = 0xFFFFFFFF;
/* The colours of the unchosen answer once the choice is shown (names inferred). */
static u32 s_noShown = 0xFFFFFFFF;
static u32 s_yesShown = 0xFFFFFFFF;

class GiGameOver : public Task {
  public:
    GiGameOver(Task *parent);
    virtual ~GiGameOver();
    virtual void Tick(float dt);

    /* The Render's draw callback (name inferred). */
    void RednerDraw(RednerDrawArgs &args);

    /* The screen being shown, if any (name inferred). */
    static GiGameOver *s_instance;

    /* 0x28 */ Chao::CSD::CProject *m_project;
    /* 0x2C */ boost::shared_ptr<Render> m_render;
    /* 0x34 */ float m_timer;
    /* 0x38 */ s32 m_state;
    /* 0x3C */ u8 m_yes;      /* the answer is yes (meaning inferred) */
    /* 0x3D */ u8 m_decided;  /* the answer was confirmed (meaning inferred) */
    /* 0x3E */ u8 field_0x3E;
    /* 0x3F */ u8 m_waiting;  /* not yet asking (meaning inferred) */
};
SIZE_ASSERT(GiGameOver, 0x40);

GiGameOver *GiGameOver::s_instance;

/* Creates the screen unless one exists (name inferred). */
extern "C" bool fn_802E3B64(void) {
    if (GiGameOver::s_instance != 0) {
        return false;
    }
    new GiGameOver(fn_801EB304(fn_8001C4F8(), 3));
    return true;
}

/* Ends the screen (name inferred). */
extern "C" bool fn_802E3B3C(void) {
    if (GiGameOver::s_instance == 0) {
        return false;
    }
    GiGameOver::s_instance->m_flags |= 1;
    return true;
}

/* Whether the screen is not yet asking (name inferred). */
extern "C" u8 fn_802E3B20(void) {
    if (GiGameOver::s_instance != 0) {
        return GiGameOver::s_instance->m_waiting;
    }
    return 0;
}

/* Sets the answer (name inferred). */
extern "C" void fn_802E3B0C(u8 yes) {
    if (GiGameOver::s_instance == 0) {
        return;
    }
    GiGameOver::s_instance->m_yes = yes;
}

/* Confirms the answer (name inferred). */
extern "C" void fn_802E3AEC(void) {
    if (GiGameOver::s_instance == 0) {
        return;
    }
    GiGameOver::s_instance->m_decided = 1;
    GiGameOver::s_instance->m_waiting = 1;
}

/* NonMatching: every instruction matches except the swap of r30 and r31. Retail keeps this in r31
 * and the .rodata base in r30; GC/2.7 gives the base r31 (as in Game/TGiCommandWindow.cpp).
 * Member initializers, statement order, a local copy of this or of the project, and this->
 * did not change it. */
GiGameOver::GiGameOver(Task *parent) : Task(parent) {
    m_timer = 0.0f;
    m_yes = 0;
    m_decided = 0;
    m_waiting = 1;
    m_name = "GiGameOver";
    s_instance = this;
    m_project = new Chao::CSD::CProject();
    if (m_project != 0) {
        fn_8003F8D4();
        fn_800422A8(m_project, fn_800C02CC(s_fileName));
        fn_8036E114(m_project, fn_800423B8(fn_8003F8D4(), s_fileName, 0));
        fn_8036D598(m_project, 0);
        RCPtr<CScene> scene(fn_8036D660(m_project, "GameOver"));
        if (scene.Get() != 0) {
            if (lbl_8057D6A0.m_twoPlayer) {
                fn_80367238(scene.Get(), "ContinueOpen");
                m_state = 1;
            } else {
                fn_80367238(scene.Get(), "GameoverOpen");
                m_state = 0;
            }
            fn_803659DC(scene.Get(), 0);
        }
        scene = fn_8036D660(m_project, "Loop");
        if (scene.Get() != 0) {
            fn_80367238(scene.Get(), "Loop");
            fn_803659DC(scene.Get(), 1);
        }
    }
    m_render = fn_8004E24C(fn_80046178()->field_0x3C);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&GiGameOver::RednerDraw, this, _1));
}

GiGameOver::~GiGameOver() {
    if (m_project != 0) {
        delete m_project;
    }
    s_instance = 0;
}

void GiGameOver::Tick(float dt) {
    if (m_project == 0) {
        m_flags |= 1;
        return;
    }
    RCPtr<CScene> scene(fn_8036D660(m_project, "GameOver"));
    switch (m_state) {
    case 0:
        if (scene->IsEnd()) {
            fn_80367238(scene.Get(), "ContinueOpen");
            CScene *s = scene.Get();
            fn_80366740(s, fn_803644A0(scene.Get(), "ContinueOpen"));
            fn_803659DC(scene.Get(), 0);
            fn_803659E4(scene.Get());
            m_state++;
        }
        break;
    case 1:
        if (scene->IsEnd()) {
            fn_80367238(scene.Get(), "Select");
            CScene *s = scene.Get();
            fn_80366740(s, fn_803644A0(scene.Get(), "Select"));
            fn_803659DC(scene.Get(), 1);
            fn_803659E4(scene.Get());
            m_waiting = 0;
            m_state++;
        }
        break;
    case 2:
        if (m_decided) {
            fn_80367238(scene.Get(), "On");
            CScene *s = scene.Get();
            fn_80366740(s, fn_803644A0(scene.Get(), "On"));
            fn_803659DC(scene.Get(), 0);
            fn_803659E4(scene.Get());
            if (m_yes) {
                if (!lbl_8057D6A0.m_twoPlayer) {
                    fn_801D51A0(fn_80053164(), 0x2A, -1, -1);
                    if (lbl_8057D6A0.m_language == 0) {
                        m_timer = 3.29f;
                    } else {
                        m_timer = 1.97f;
                    }
                } else {
                    m_timer = 0.0f;
                }
            }
            m_state++;
        }
        break;
    case 3:
        m_timer -= dt;
        if (m_timer < 0.0f) {
            fn_80367238(scene.Get(), "ContinueOpen");
            CScene *s = scene.Get();
            fn_80366740(s, fn_80364430(scene.Get(), "ContinueOpen"));
            fn_80367140(scene.Get(), -1.0f);
            fn_803659CC(scene.Get(), 0);
            fn_803659E4(scene.Get());
            m_state++;
        }
        break;
    case 4:
        if (scene->IsEnd()) {
            m_flags |= 1;
        }
        break;
    }
    for (int i = 0; i < 4; i++) {
        if (m_yes) {
            fn_8036AC60(fn_803666B4(scene.Get(), "yes").Get(), i, &s_yesChosen);
            fn_8036AC60(fn_803666B4(scene.Get(), "no").Get(), i, &s_noOther);
        } else {
            fn_8036AC60(fn_803666B4(scene.Get(), "yes").Get(), i, &s_yesOther);
            fn_8036AC60(fn_803666B4(scene.Get(), "no").Get(), i, &s_noChosen);
        }
    }
    if (m_state == 2) {
        if (m_yes) {
            fn_8036ACA8(fn_803666B4(scene.Get(), "yes").Get());
            fn_8036ACCC(fn_803666B4(scene.Get(), "no").Get(), &s_noShown);
        } else {
            fn_8036ACCC(fn_803666B4(scene.Get(), "yes").Get(), &s_yesShown);
            fn_8036ACA8(fn_803666B4(scene.Get(), "no").Get());
        }
    }
    fn_8036DAD4(m_project);
}

void GiGameOver::RednerDraw(RednerDrawArgs &args) {
    if (m_project != 0) {
        fn_8036D94C(m_project);
    }
}
