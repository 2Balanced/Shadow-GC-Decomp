/* TGiHPGaugeBoss (class name from RTTI): the boss health gauge. Its state lives in a
 * TGiHPGaugeBossImpl (class name from the RTTI of its draw callback's bind_t) that plays the CSD
 * project "CsdFiles/BossGauge" and sets the scene's frame from the ratio the bosses set with
 * fn_800BE2DC, while fn_800BE2EC shows it. Game/DiablonPartsHPGauge.cpp keeps its own declaration
 * of the task. The file name and the names marked "(name inferred)" are not original; the functions
 * other units call keep their fn_ names. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#define TGI_NO_NUMBER_COLORS
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CsdRCPtr.h"
#include "Game/Render.h"
#include "Game/TGi.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

extern "C" {
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* A scene's end frame (behaviour inferred). */
float fn_80366728(CScene *scene);
/* Sets the gauge's ratio and whether it shows; each returns the previous value. */
float fn_800BE2DC(float ratio);
u8 fn_800BE2EC(u8 show);
}

class TGiHPGaugeBossImpl {
  public:
    TGiHPGaugeBossImpl();
    ~TGiHPGaugeBossImpl();

    /* Sets the scene's frame from the ratio (name inferred). */
    void Update();
    /* The Render's draw callback (name inferred). */
    void Draw(RednerDrawArgs &args) const;

    /* 0x00 */ boost::shared_ptr<Render> m_render;
    /* 0x08 */ Chao::CSD::CProject *m_project;
};
SIZE_ASSERT(TGiHPGaugeBossImpl, 0xC);

/* Deletes the impl (name inferred; see checked_delete below). */
inline void DeleteImpl(TGiHPGaugeBossImpl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<TGiHPGaugeBossImpl> out of line and the destructor at its own
 * place; this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<TGiHPGaugeBossImpl>(TGiHPGaugeBossImpl *x) {
    DeleteImpl(x);
}
} // namespace boost

class TGiHPGaugeBoss : public Task {
  public:
    TGiHPGaugeBoss(Task *parent);
    virtual ~TGiHPGaugeBoss();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<TGiHPGaugeBossImpl> m_impl;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(TGiHPGaugeBoss, 0x30);

/* Whether the gauge shows, and the ratio it shows (names inferred). */
static u8 s_show;
static float s_ratio;

TGiHPGaugeBossImpl::TGiHPGaugeBossImpl() {
    s_show = 0;
    s_ratio = 1.0f;
    m_render = fn_8004E24C(fn_80046178()->field_0x38);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&TGiHPGaugeBossImpl::Draw, this, _1));
    m_project = new Chao::CSD::CProject();
    fn_8036E114(m_project, fn_800423B8(fn_8003F8D4(), "CsdFiles/BossGauge", 0));
    fn_8036D598(m_project, 0);
}

TGiHPGaugeBossImpl::~TGiHPGaugeBossImpl() {
    if (m_project != 0) {
        delete m_project;
    }
}

void TGiHPGaugeBossImpl::Update() {
    if (m_project != 0 && s_show) {
        RCPtr<CScene> scene = fn_8036D660(m_project, "Scene_0000");
        if (scene.Get() != 0) {
            float end = fn_80366728(scene.Get()) - 1.0f;
            float frame = end * (1.0f - s_ratio);
            fn_80366740(scene.Get(), frame);
            fn_8036DAD4(m_project);
        }
    }
}

void TGiHPGaugeBossImpl::Draw(RednerDrawArgs &args) const {
    if (lbl_8057D6A0.m_hudVisible && s_show) {
        fn_8036D94C(m_project);
    }
}

TGiHPGaugeBoss::TGiHPGaugeBoss(Task *parent) : Task(parent), m_impl(new TGiHPGaugeBossImpl) {
    m_name = "TGiHPGaugeBoss";
    m_flags |= 0x100;
}

TGiHPGaugeBoss::~TGiHPGaugeBoss() {}

void TGiHPGaugeBoss::Tick(float dt) {
    m_impl->Update();
}

extern "C" u8 fn_800BE2EC(u8 show) {
    u8 old = s_show;
    s_show = show;
    return old;
}

extern "C" float fn_800BE2DC(float ratio) {
    float old = s_ratio;
    s_ratio = ratio;
    return old;
}
