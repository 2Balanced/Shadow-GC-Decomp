/* TGiScoreDisplay (class name from RTTI): the heads-up display of the score, the points and the
 * stage time. Its state lives in a TGiScoreDisplayImpl (class name from the RTTI of its draw
 * callback's bind_t). Five TGiNumberDisplays print the time, the score, two point counters and a
 * second time; the time turns yellow, then red, and pulses as it runs low. In two-player mode it
 * plays the CSD project "CsdFiles/2P_Star" (the scene Star, whose nodes star1P_1 to star2P_2 show
 * each player's stars). The file name and the names marked "(name inferred)" are not original; the
 * functions other units call keep their fn_ names. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
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

/* A time split into minutes, seconds and hundredths (class name unknown; its out-of-line members
 * are fn_801E45A4 to fn_801E46E4). */
class UnkTime_801E46E4 {
  public:
    UnkTime_801E46E4();
    UnkTime_801E46E4(float time);

    UnkTime_801E46E4 &operator=(const UnkTime_801E46E4 &other) {
        m_time = other.m_time;
        Split(m_time);
        return *this;
    }

    /* Sets the minutes, seconds and hundredths from time (name inferred). */
    void Split(float time);

    /* 0x0 */ s32 m_minutes;
    /* 0x4 */ s8 m_seconds;
    /* 0x5 */ s8 m_hundredths;
    /* 0x8 */ float m_time;
};
SIZE_ASSERT(UnkTime_801E46E4, 0xC);

extern "C" {
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* The sound manager and its play call (as in GadgetUtil.h). */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
/* Shows (0) or hides (1) a node (behaviour inferred). */
void fn_8036AD78(CNode *node, int hidden);
}

/* Not original: unused tables that pool the unit's .rodata strings in retail's order (the linker
 * drops them; the RTTI names and literals below reuse these copies). The .rodata is kept whole
 * (the constructor addresses it through one base), so it also holds the RTTI names of the CSD and
 * boost headers' inline classes, which this unit does not reconstruct. */
static const char *s_rodataPool1[] = {
    "boost::_bi::bind_t<void, boost::_mfi::mf1<void, TGiScoreDisplayImpl, RednerDrawArgs &>, boost::_bi::list2<boost::_bi::value<TGiScoreDisplayImpl *>, boost::arg<1>>>",
};

/* The colours of the time (yellow); UpdateTime clears red when a minute or more is left and green
 * when less than 30 seconds are left. */
static const TGiColorSet s_timeColors = {{
    {255, 255, 0, 255},
    {128, 128, 0, 255},
    {64, 64, 0, 255},
    {255, 255, 0, 255},
}};

static const char *s_rodataPool2[] = {
    "TGiScoreDisplay",
    "99:59:99",
    "%02d:%02d:%02d",
    "star1P_1",
    "star1P_2",
    "star2P_1",
    "star2P_2",
    "Not inititalized m_eDealloctor\n",
    "csdRCObject.h",
    "Memory allocation failure",
    "csdBase.h",
    "CsdFiles/2P_Star",
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
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>",
    "boost::checked_deleter<TGiNumberDisplay>",
};

class TGiScoreDisplayImpl {
  public:
    TGiScoreDisplayImpl();
    ~TGiScoreDisplayImpl();

    /* The Render's draw callback (name inferred). */
    void Draw(RednerDrawArgs &args);
    /* Colours display after time and pulses it when time passes a step (name inferred). */
    void UpdateTime(boost::shared_ptr<TGiNumberDisplay> display, UnkTime_801E46E4 time);

    /* Shrinks the pulse back (name inferred). */
    void Tick(float dt) {
        m_scale -= dt;
        if (m_scale < 1.0f) {
            m_scale = 1.0f;
        }
    }

    /* 0x00 */ boost::shared_ptr<Render> m_render;
    /* 0x08 */ Chao::CSD::CProject *m_project;
    /* 0x0C */ boost::shared_ptr<TGiNumberDisplay> m_numbers[5];
    /* 0x34 */ UnkTime_801E46E4 m_time;
    /* 0x40 */ float m_scale;
};

/* Deletes the impl (name inferred; see checked_delete below). */
inline void DeleteImpl(TGiScoreDisplayImpl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<TGiScoreDisplayImpl> out of line and the destructor at its own
 * place; this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<TGiScoreDisplayImpl>(TGiScoreDisplayImpl *x) {
    DeleteImpl(x);
}
} // namespace boost

class TGiScoreDisplay : public Task {
  public:
    TGiScoreDisplay(Task *parent);
    virtual ~TGiScoreDisplay();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<TGiScoreDisplayImpl> m_impl;
};

SIZE_ASSERT(TGiScoreDisplayImpl, 0x44);
SIZE_ASSERT(TGiScoreDisplay, 0x2C);

TGiScoreDisplayImpl::TGiScoreDisplayImpl() {
    m_render = fn_8004E24C(fn_80046178()->field_0x54);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&TGiScoreDisplayImpl::Draw, this, _1));

    if (lbl_8057D6A0.m_twoPlayer) {
        m_project = new Chao::CSD::CProject();
        fn_8036E114(m_project, fn_800423B8(fn_8003F8D4(), "CsdFiles/2P_Star", 0));
        fn_8036D598(m_project, 0);
        RCPtr<CScene> scene;
        scene = fn_8036D660(m_project, "Star");
        if (scene.Get() != 0) {
            RCPtr<CNode> node;
            if ((node = fn_803666B4(scene.Get(), "star1P_1")).Get() != 0) {
                fn_8036AD78(node.Get(), 1);
            }
            if ((node = fn_803666B4(scene.Get(), "star1P_2")).Get() != 0) {
                fn_8036AD78(node.Get(), 1);
            }
            if ((node = fn_803666B4(scene.Get(), "star2P_1")).Get() != 0) {
                fn_8036AD78(node.Get(), 1);
            }
            if ((node = fn_803666B4(scene.Get(), "star2P_2")).Get() != 0) {
                fn_8036AD78(node.Get(), 1);
            }
        }
    } else {
        m_project = 0;
    }

    TGiColorSet colors = s_numberColors;
    for (int i = 0; i < 5; i++) {
        m_numbers[i] = boost::shared_ptr<TGiNumberDisplay>(new TGiNumberDisplay);
        fn_800BCEA8(m_numbers[i].get(), lbl_805EE68C, "ad_hpasc1");
        fn_800BCC54(m_numbers[i].get(), 64, 64, 16, 18, 4);
        fn_800BCB7C(m_numbers[i].get(), &colors);
        fn_800BCB2C(m_numbers[i].get(), -2.7f);
    }
    m_scale = 1.0f;
}

TGiScoreDisplayImpl::~TGiScoreDisplayImpl() {
    if (m_project != 0) {
        delete m_project;
    }
}

void TGiScoreDisplayImpl::Draw(RednerDrawArgs &args) {
    if (lbl_8057D6A0.m_hudVisible) {
        char text[24];
        UnkTime_801E46E4 time;
        if (lbl_8057D6A0.m_twoPlayer) {
            bool shown = false;
            if (lbl_8057D6A0.field_0x9C > 0.0f) {
                time = UnkTime_801E46E4(lbl_8057D6A0.m_time2);
                UpdateTime(m_numbers[4], time);
                if (lbl_8057D6A0.field_0x14A) {
                    shown = true;
                    if (time.m_minutes >= 100) {
                        sprintf(text, "99:59:99", time.m_minutes, time.m_seconds, time.m_hundredths);
                    } else {
                        sprintf(text, "%02d:%02d:%02d", time.m_minutes, time.m_seconds, time.m_hundredths);
                    }
                    fn_800BC7BC(m_numbers[4].get(), text, 320.0f, 1, 41.0f);
                }
            }
            if (!shown) {
                time = UnkTime_801E46E4(lbl_8057D6A0.m_time);
                if (time.m_minutes >= 100) {
                    sprintf(text, "99:59:99", time.m_minutes, time.m_seconds, time.m_hundredths);
                } else {
                    sprintf(text, "%02d:%02d:%02d", time.m_minutes, time.m_seconds, time.m_hundredths);
                }
                fn_800BC7BC(m_numbers[0].get(), text, 320.0f, 1, 41.0f);
            }

            RCPtr<CScene> scene = fn_8036D660(m_project, "Star");
            if (scene.Get() != 0) {
                RCPtr<CNode> node;
                if ((node = fn_803666B4(scene.Get(), "star1P_1")).Get() != 0) {
                    if (lbl_8057D6A0.m_stars[0] > 0) {
                        fn_8036AD78(node.Get(), 0);
                    } else {
                        fn_8036AD78(node.Get(), 1);
                    }
                }
                if ((node = fn_803666B4(scene.Get(), "star1P_2")).Get() != 0) {
                    if (lbl_8057D6A0.m_stars[0] > 1) {
                        fn_8036AD78(node.Get(), 0);
                    } else {
                        fn_8036AD78(node.Get(), 1);
                    }
                }
                if ((node = fn_803666B4(scene.Get(), "star2P_1")).Get() != 0) {
                    if (lbl_8057D6A0.m_stars[1] > 0) {
                        fn_8036AD78(node.Get(), 0);
                    } else {
                        fn_8036AD78(node.Get(), 1);
                    }
                }
                if ((node = fn_803666B4(scene.Get(), "star2P_2")).Get() != 0) {
                    if (lbl_8057D6A0.m_stars[1] > 1) {
                        fn_8036AD78(node.Get(), 0);
                    } else {
                        fn_8036AD78(node.Get(), 1);
                    }
                }
            }
            fn_8036DAD4(m_project);
            fn_8036D94C(m_project);
        } else {
            UnkPlayerInfo_80178B5C *player = fn_80178B5C(fn_80079808(), 0);
            if (player == 0) {
                return;
            }
            time = UnkTime_801E46E4(lbl_8057D6A0.m_time);
            if (time.m_minutes >= 100) {
                sprintf(text, "99:59:99", time.m_minutes, time.m_seconds, time.m_hundredths);
            } else {
                sprintf(text, "%02d:%02d:%02d", time.m_minutes, time.m_seconds, time.m_hundredths);
            }
            fn_800BC7BC(m_numbers[0].get(), text, 320.0f, 1, 41.0f);
            sprintf(text, "%08d", player->m_score);
            fn_800BC7BC(m_numbers[1].get(), text, 320.0f, 1, 61.0f);
            sprintf(text, "%07d", player->m_heroPoints);
            fn_800BC7BC(m_numbers[2].get(), text, 477.0f, 0, 61.0f);
            sprintf(text, "%07d", player->m_darkPoints);
            fn_800BC7BC(m_numbers[3].get(), text, 54.0f, 0, 61.0f);
            if (lbl_8057D6A0.field_0x9C > 0.0f) {
                time = UnkTime_801E46E4(lbl_8057D6A0.m_time2);
                UpdateTime(m_numbers[4], time);
                if (lbl_8057D6A0.field_0x14A) {
                    if (time.m_minutes >= 100) {
                        sprintf(text, "99:59:99", time.m_minutes, time.m_seconds, time.m_hundredths);
                    } else {
                        sprintf(text, "%02d:%02d:%02d", time.m_minutes, time.m_seconds, time.m_hundredths);
                    }
                    fn_800BC7BC(m_numbers[4].get(), text, 320.0f, 1, 100.0f);
                }
            }
        }
    }
}

void TGiScoreDisplayImpl::UpdateTime(boost::shared_ptr<TGiNumberDisplay> display, UnkTime_801E46E4 time) {
    TGiColorSet colors = s_timeColors;
    if (time.m_minutes >= 1) {
        colors.m_colors[3].r = 0;
        colors.m_colors[2].r = 0;
        colors.m_colors[1].r = 0;
        colors.m_colors[0].r = 0;
    } else if (time.m_seconds < 30) {
        colors.m_colors[3].g = 0;
        colors.m_colors[2].g = 0;
        colors.m_colors[1].g = 0;
        colors.m_colors[0].g = 0;
    }
    fn_800BCB7C(display.get(), &colors);

    bool passed = false;
    if (time.m_minutes < m_time.m_minutes) {
        passed = true;
    }
    if (time.m_minutes == 0) {
        if (time.m_seconds >= 30) {
            if (time.m_seconds / 10 < m_time.m_seconds / 10) {
                passed = true;
            }
        } else if (time.m_seconds >= 10) {
            if (time.m_seconds / 5 < m_time.m_seconds / 5) {
                passed = true;
            }
        } else if (time.m_seconds < m_time.m_seconds) {
            passed = true;
        }
    }
    if (passed) {
        m_scale = 1.5f;
        if (lbl_8057D6A0.field_0x14A) {
            fn_801D560C(fn_80053164(), 0xE016, 0, 0);
        }
    }
    m_time = time;
    display->SetScale(m_scale);
}

TGiScoreDisplay::TGiScoreDisplay(Task *parent) : Task(parent), m_impl(new TGiScoreDisplayImpl) {
    m_name = "TGiScoreDisplay";
}

TGiScoreDisplay::~TGiScoreDisplay() {}

void TGiScoreDisplay::Tick(float dt) {
    m_impl->Tick(dt);
}
