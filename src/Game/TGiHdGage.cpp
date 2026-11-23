/* TGiHdGage (class name from RTTI): the heads-up display of the hero and dark gauges. It plays the
 * CSD projects "CsdFiles/HeroGauge" and "CsdFiles/DarkGauge" (scene Scene_0000 in each) and sets
 * each scene's frame from the gauge the save data keeps, filling it up by halves. The file name and
 * the names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * Matching since wave 41. Tick's two conversions (float to int and back) share one stack slot
 * under GC/2.7 too; what kept it apart was the gauge's addressing and the null tests' order (see
 * Tick). In the destructor retail copies the loop's zero into the offset register (mr) where
 * GC/2.7 loads it again (li) for a loop written in the destructor itself; the loop in an inline
 * helper matches (wave 39). The object's .rodata also gets the RTTI name of
 * boost::detail::sp_counted_base, which retail's does not have (GC/2.7 instantiates the
 * shared_ptr<Render> helpers here); the link drops it. */
#define TGI_NO_NUMBER_COLORS
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CsdRCPtr.h"
#include "Game/Render.h"
#include "Game/TGi.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

extern "C" {
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* A scene of a project by name, made to play (behaviour inferred; see Game/Gong.cpp). */
RCPtr<CScene> fn_8036D890(Chao::CSD::CProject *project, const char *name, int arg2, int arg3);
/* A scene's end frame (behaviour inferred). */
float fn_80366728(CScene *scene);
}

/* The empty scene pointer (see Game/AdvCsdEffect.cpp). */
extern RCPtr<CScene> lbl_805EE118;

/* A gauge as the save data keeps it (struct name and member names inferred). */
struct UnkHdGauge_80079808 {
    /* 0x0 */ s32 m_value;
    /* 0x4 */ s32 m_max;
    /* 0x8 */ s32 field_0x8;
};

/* A gauge's project and scene (struct name and member names inferred). */
struct UnkHdGageFile_804B5174 {
    const char *m_file;
    const char *m_scene;
};

static const UnkHdGageFile_804B5174 s_files[2] = {
    {"CsdFiles/HeroGauge", "Scene_0000"},
    {"CsdFiles/DarkGauge", "Scene_0000"},
};

class TGiHdGage : public Task {
  public:
    /* A gauge on screen (struct name and member names inferred). */
    struct Gauge {
        /* 0x0 */ Chao::CSD::CProject *m_project;
        /* 0x4 */ s32 m_value; /* the value shown */
    };

    TGiHdGage(Task *parent);
    virtual ~TGiHdGage();
    virtual void Tick(float dt);

    /* Loads the projects (name inferred). */
    void Load();
    /* The Render's draw callback (name inferred). */
    void Draw(RednerDrawArgs &args) const;

    /* 0x28 */ boost::shared_ptr<Render> m_render;
    /* 0x30 */ Gauge m_gauges[2]; /* hero, dark */
};
SIZE_ASSERT(TGiHdGage, 0x40);

TGiHdGage::TGiHdGage(Task *parent) : Task(parent) {
    m_name = "TGiHdGage";
    m_flags |= 0x100;
    Load();
    m_render = fn_8004E24C(fn_80046178()->field_0x38);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&TGiHdGage::Draw, this, _1));
}

/* The destructor's loop, as an inline function: only then does GC/2.7 copy the loop's zero into
 * the offset register (mr r31,r30), as retail does, instead of loading it again (name
 * inferred). */
inline void DeleteGaugeProjects(TGiHdGage *gage) {
    for (int i = 0; i < 2; i++) {
        delete gage->m_gauges[i].m_project;
    }
}

TGiHdGage::~TGiHdGage() {
    DeleteGaugeProjects(this);
}

/* The counter is declared first and the gauge is indexed at each use (retail forms this + i*8
 * once and reaches both members from it); the scene is read into a local before the empty scene
 * is, which orders the two null tests and the compare as retail has them (wave 41,
 * probes/Game_TGiHdGage_W41.py). */
void TGiHdGage::Tick(float dt) {
    int i;
    UnkHdGauge_80079808 *saved = (UnkHdGauge_80079808 *)((u8 *)fn_80079808() + 0x70C);
    for (i = 0; i < 2; i++) {
        RCPtr<CScene> scene = fn_8036D660(m_gauges[i].m_project, s_files[i].m_scene);
        CScene *current = scene.Get();
        if (current != lbl_805EE118.Get()) {
            int frames = fn_80366728(scene.Get());
            UnkHdGauge_80079808 *value = &saved[i];
            if (value->m_value > m_gauges[i].m_value) {
                m_gauges[i].m_value = (m_gauges[i].m_value + value->m_value) / 2;
            } else {
                m_gauges[i].m_value = value->m_value;
            }
            if (m_gauges[i].m_value > value->m_max) {
                m_gauges[i].m_value = value->m_max;
            }
            fn_80366740(scene.Get(), frames * m_gauges[i].m_value / value->m_max);
            fn_8036DAD4(m_gauges[i].m_project);
        }
    }
}

void TGiHdGage::Load() {
    for (int i = 0; i < 2; i++) {
        m_gauges[i].m_project = new Chao::CSD::CProject();
        fn_8036E114(m_gauges[i].m_project, fn_800423B8(fn_8003F8D4(), s_files[i].m_file, 0));
        fn_8036D890(m_gauges[i].m_project, s_files[i].m_scene, 0, 0);
    }
}

void TGiHdGage::Draw(RednerDrawArgs &args) const {
    if (lbl_8057D6A0.m_hudVisible) {
        for (int i = 0; i < 2; i++) {
            fn_8036D94C(m_gauges[i].m_project);
        }
    }
}
