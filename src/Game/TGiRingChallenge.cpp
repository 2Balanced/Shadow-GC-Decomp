/* TGiRingChallenge (class name from RTTI): the heads-up display of the rings and lives. Its state
 * lives in a TGiRingChallengeImpl (class name from the RTTI of its draw callback's bind_t) that plays
 * the CSD project "CsdFiles/InfoWindow" (the scenes Ring and Zanki) or, in two-player mode,
 * "CsdFiles/2P_Ring" (the scenes Ring1P and Ring2P). Three TGiNumberDisplays print the rings, the
 * lives and a counter of the save data. The file name and the names marked "(name inferred)" are not
 * original; the functions other units call keep their fn_ names. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstdio>

#include "Game/CsdRCPtr.h"
#include "Game/Render.h"
#include "Game/TGi.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CNode;
using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

/* The argument a Render passes to its draw callbacks (unused here). */
struct RednerDrawArgs;

extern "C" {
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* A player's rings and lives, and the time they last changed (behaviour inferred). */
int fn_8016F280(void *rings);
int fn_8016EF84(void *lives);
u32 fn_8016EF7C(void *lives);
/* A counter of the save data and the time it last changed (behaviour inferred). */
int fn_803357B8(void *counter);
u32 fn_803357B0(void *counter);
}

/* Not original: an unused table that pools the unit's .rodata strings in retail's order (the
 * linker drops the table; the RTTI names and literals below reuse these copies). The .rodata is
 * kept whole (the constructor addresses it through one base), so it also holds the RTTI names of
 * the CSD and boost headers' inline classes, which this unit does not reconstruct. */
static const char *s_rodataPool[] = {
    "boost::_bi::bind_t<void, boost::_mfi::mf1<void, TGiRingChallengeImpl, RednerDrawArgs &>, boost::_bi::list2<boost::_bi::value<TGiRingChallengeImpl *>, boost::arg<1>>>",
    "TGiRingChallenge",
    "Not inititalized m_eDealloctor\n",
    "csdRCObject.h",
    "Memory allocation failure",
    "csdBase.h",
    "CsdFiles/2P_Ring",
    "OpenRing",
    "CsdFiles/InfoWindow",
    "ad_hpasc1",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>::CFunctorBase",
    "Chao::CSD::CBase",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>",
    "Chao::CSD::CResourceBase<Chao::CSD::Project>",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>",
    "boost::detail::sp_counted_base_impl<TGiNumberDisplay *, boost::checked_deleter<TGiNumberDisplay>>",
    "boost::detail::sp_counted_base",
    "std::exception",
    "exception",
    "std::bad_alloc",
    "bad_alloc",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>",
    "boost::checked_deleter<TGiNumberDisplay>",
};

class TGiRingChallengeImpl {
  public:
    TGiRingChallengeImpl();
    ~TGiRingChallengeImpl();

    /* The Render's draw callback (name inferred). */
    void Draw(RednerDrawArgs &args);

    /* 0x00 */ boost::shared_ptr<Render> m_render;
    /* 0x08 */ UnkCsdResource_8003E808 *m_project;
    /* 0x0C */ boost::shared_ptr<TGiNumberDisplay> m_numbers[3];
};

/* Deletes the impl (name inferred; see checked_delete below). */
inline void DeleteImpl(TGiRingChallengeImpl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<TGiRingChallengeImpl> out of line and the destructor at its
 * own place; this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<TGiRingChallengeImpl>(TGiRingChallengeImpl *x) {
    DeleteImpl(x);
}
} // namespace boost

class TGiRingChallenge : public Task {
  public:
    TGiRingChallenge(Task *parent);
    virtual ~TGiRingChallenge();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<TGiRingChallengeImpl> m_impl;
};

SIZE_ASSERT(TGiRingChallengeImpl, 0x24);
SIZE_ASSERT(TGiRingChallenge, 0x2C);

TGiRingChallengeImpl::TGiRingChallengeImpl() {
    m_render = fn_8004E24C(fn_80046178()->field_0x38);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&TGiRingChallengeImpl::Draw, this, _1));

    RCPtr<CScene> scene;
    if (lbl_8057D6A0.m_twoPlayer) {
        m_project = new UnkCsdResource_8003E808();
        fn_8036E114(m_project, fn_800423B8(fn_8003F8D4(), "CsdFiles/2P_Ring", 0));
        fn_8036D598(m_project, 0);
        scene = fn_8036D660(m_project, "Ring1P");
        if (scene.Get() != 0) {
            CScene *s = scene.Get();
            fn_80366740(s, fn_80364430(scene.Get(), "OpenRing"));
            fn_803659DC(scene.Get(), 0);
        }
        scene = fn_8036D660(m_project, "Ring2P");
        if (scene.Get() != 0) {
            CScene *s = scene.Get();
            fn_80366740(s, fn_80364430(scene.Get(), "OpenRing"));
            fn_803659DC(scene.Get(), 0);
        }
    } else {
        m_project = new UnkCsdResource_8003E808();
        fn_8036E114(m_project, fn_800423B8(fn_8003F8D4(), "CsdFiles/InfoWindow", 0));
        fn_8036D598(m_project, 0);
        scene = fn_8036D660(m_project, "Ring");
        if (scene.Get() != 0) {
            fn_80367238(scene.Get(), "Open");
            CScene *s = scene.Get();
            fn_80366740(s, fn_803644A0(scene.Get(), "Open"));
            fn_80367140(scene.Get(), 1.0f);
            fn_803659DC(scene.Get(), 0);
            fn_803659CC(scene.Get(), 0);
        }
        scene = fn_8036D660(m_project, "Zanki");
        if (scene.Get() != 0) {
            fn_80367238(scene.Get(), "Open");
            CScene *s = scene.Get();
            fn_80366740(s, fn_803644A0(scene.Get(), "Open"));
            fn_80367140(scene.Get(), 1.0f);
            fn_803659DC(scene.Get(), 0);
            fn_803659CC(scene.Get(), 0);
        }
    }

    TGiColorSet colors = s_numberColors;
    for (int i = 0; i < 3; i++) {
        m_numbers[i] = boost::shared_ptr<TGiNumberDisplay>(new TGiNumberDisplay);
        fn_800BCEA8(m_numbers[i].get(), lbl_805EE68C, "ad_hpasc1");
        fn_800BCC54(m_numbers[i].get(), 64, 64, 16, 18, 4);
        fn_800BCB7C(m_numbers[i].get(), &colors);
        fn_800BCB2C(m_numbers[i].get(), -2.7f);
    }
}

TGiRingChallengeImpl::~TGiRingChallengeImpl() {
    if (m_project != 0) {
        delete m_project;
    }
}

void TGiRingChallengeImpl::Draw(RednerDrawArgs &args) {
    if (lbl_8057D6A0.m_hudVisible) {
        RCPtr<CScene> scene;
        RCPtr<CNode> node;
        char text[16];
        if (lbl_8057D6A0.m_twoPlayer) {
            if (m_project == 0) {
                return;
            }
            fn_8036DAD4(m_project);
            fn_8036D94C(m_project);
            scene = fn_8036D660(m_project, "Ring1P");
            if (scene.Get() != 0) {
                UnkPlayerInfo_80178B5C *player = fn_80178B5C(fn_80079808(), 0);
                if (player != 0 && scene->IsEnd()) {
                    sprintf(text, "%03d", fn_8016F280(player->m_rings));
                    fn_800BC7BC(m_numbers[0].get(), text, 84.0f, 1, 68.0f);
                }
            }
            scene = fn_8036D660(m_project, "Ring2P");
            if (scene.Get() != 0) {
                UnkPlayerInfo_80178B5C *player = fn_80178B5C(fn_80079808(), 1);
                if (player != 0 && scene->IsEnd()) {
                    sprintf(text, "%03d", fn_8016F280(player->m_rings));
                    fn_800BC7BC(m_numbers[1].get(), text, 560.0f, 1, 68.0f);
                }
            }
        } else {
            UnkPlayerInfo_80178B5C *player = fn_80178B5C(fn_80079808(), 0);
            if (player == 0) {
                return;
            }
            fn_8036DAD4(m_project);
            fn_8036D94C(m_project);
            scene = fn_8036D660(m_project, "Ring");
            if (scene.Get() != 0 && scene->IsEnd() && fn_80367138(scene.Get()) > 0.0f) {
                sprintf(text, "%03d", fn_8016F280(player->m_rings));
                fn_800BC7BC(m_numbers[0].get(), text, 78.0f, 1, 395.0f);
            }
            scene = fn_8036D660(m_project, "Zanki");
            if (scene.Get() != 0 && scene->IsEnd()) {
                if (fn_80367138(scene.Get()) > 0.0f) {
                    sprintf(text, "%d", fn_8016EF84(player->m_lives));
                    fn_800BC7BC(m_numbers[1].get(), text, 78.0f, 1, 332.0f);
                    sprintf(text, "%d", fn_803357B8((u8 *)fn_80079808() + 0x724));
                    fn_800BC7BC(m_numbers[2].get(), text, 78.0f, 1, 266.0f);
                }
                bool show = false;
                if (lbl_8057D6A0.field_0x84 ||
                    lbl_8057D6A0.m_frame - fn_803357B0((u8 *)fn_80079808() + 0x724) < 180 ||
                    lbl_8057D6A0.m_frame - fn_8016EF7C(player->m_lives) < 180) {
                    show = true;
                }
                if (show && !(fn_80367138(scene.Get()) > 0.0f)) {
                    fn_80367140(scene.Get(), 1.0f);
                    fn_803659E4(scene.Get());
                } else if (!show && fn_80367138(scene.Get()) > 0.0f) {
                    fn_80367140(scene.Get(), -1.0f);
                    fn_803659E4(scene.Get());
                }
            }
        }
    }
}

TGiRingChallenge::TGiRingChallenge(Task *parent) : Task(parent), m_impl(new TGiRingChallengeImpl) {
    m_name = "TGiRingChallenge";
}

TGiRingChallenge::~TGiRingChallenge() {}

void TGiRingChallenge::Tick(float dt) {}
