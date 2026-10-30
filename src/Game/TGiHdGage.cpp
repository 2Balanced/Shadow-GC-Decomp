/* TGiHdGage (class name from RTTI): the heads-up display of the hero and dark gauges. It plays the
 * CSD projects "CsdFiles/HeroGauge" and "CsdFiles/DarkGauge" (scene Scene_0000 in each) and sets
 * each scene's frame from the gauge the save data keeps, filling it up by halves. The file name and
 * the names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 22 of 24 functions and all the data match. Tick converts a float to an int and an
 * int to a float; retail shares one stack slot between the two conversions, which GC/2.7 does not
 * do (see docs/research-notes.md); it also addresses the gauge as the array's offset plus the
 * index, then this, where GC/2.7 adds the index to this. In the destructor retail copies the loop's
 * zero into the offset register (mr) where GC/2.7 loads it again (li); loop shapes and declaration
 * orders did not change it (folderTargetWindow's constructor shows the same). The unit's .rodata
 * also gets the RTTI name of boost::detail::sp_counted_base, which retail's does not have
 * (GC/2.7 instantiates the shared_ptr<Render> helpers here). */
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

TGiHdGage::~TGiHdGage() {
    for (int i = 0; i < 2; i++) {
        delete m_gauges[i].m_project;
    }
}

void TGiHdGage::Tick(float dt) {
    UnkHdGauge_80079808 *saved = (UnkHdGauge_80079808 *)((u8 *)fn_80079808() + 0x70C);
    for (int i = 0; i < 2; i++) {
        Gauge *gauge = &m_gauges[i];
        RCPtr<CScene> scene = fn_8036D660(gauge->m_project, s_files[i].m_scene);
        if (scene.Get() != lbl_805EE118.Get()) {
            int frames = fn_80366728(scene.Get());
            UnkHdGauge_80079808 *value = &saved[i];
            if (value->m_value > gauge->m_value) {
                gauge->m_value = (gauge->m_value + value->m_value) / 2;
            } else {
                gauge->m_value = value->m_value;
            }
            if (gauge->m_value > value->m_max) {
                gauge->m_value = value->m_max;
            }
            fn_80366740(scene.Get(), frames * gauge->m_value / value->m_max);
            fn_8036DAD4(gauge->m_project);
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
