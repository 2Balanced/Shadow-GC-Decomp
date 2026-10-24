/* CsdOneShot (class name from RTTI): a task that loads a CSD project file, plays one motion of one
 * scene of it once over the game (drawn from a Render's draw callback), and ends itself when the
 * motion has ended. fn_8035367C creates it when the file exists (the bosses' character cut-ins:
 * "CsdFiles/BChara_BBull", "bchara", "open"). The file name comes from the class name; the
 * functions other units call keep their fn_ names and the other names are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstring>

#include "Game/CsdRCPtr.h"
#include "Game/PjsCsd.h"
#include "Game/Render.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The loaded CSD project (Chao::CSD::CProject, Game/csdmProject.cpp; declared as in Game/TGi.h). */
namespace Chao {
namespace CSD {
class CProject {
  public:
    CProject();
    virtual ~CProject();

    /* 0x04 */ u8 field_0x4[0x4C - 0x4];
};
} // namespace CSD
} // namespace Chao

/* The file system fn_800371D8 returns (class name unknown; see Game/GadgetStageData.cpp). */
class UnkFileSystem_800371D8 {
  public:
    /* Whether a file exists (name inferred). */
    bool Exists(const char *path) {
        return exists(path, this);
    }

    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ bool (*exists)(const char *path, UnkFileSystem_800371D8 *fileSystem);
};

/* What the file system's load request takes (struct name unknown; as in Game/ResultTask.cpp). */
struct CsdOneShotRequest {
    /* 0x00 */ const char *m_path;
    /* 0x04 */ void *m_data;
    /* 0x08 */ u32 m_size;
    /* 0x0C */ void *m_user;
    /* 0x10 */ void (*m_callback)(CsdOneShotRequest *request);
};

extern "C" {
/* The resource manager's language-specific path of a file (see Game/TGi.h). */
void *fn_8003F8D4(void);
const char *fn_800423B8(void *arg0, const char *name, int arg2);
/* The file system, a load request to it, and the release of loaded data (see
 * Game/ResultTask.cpp). */
UnkFileSystem_800371D8 *fn_800371D8(void);
bool fn_80045044(UnkFileSystem_800371D8 *fileSystem, CsdOneShotRequest *request, int arg2);
void fn_80049B34(void *data);
/* Sets the motion of the project's scene (behaviour inferred). */
void fn_8003E388(TPjsCsd &csd, const char *motion, int arg2);
/* The task registry, and adding and removing a named task (see Game/NpcNameDisplay.cpp). */
void *fn_80014B90(void);
void fn_8025D144(void *registry, Task *task);
void fn_8025D1C4(void *registry, Task *task, int arg2, const char *name);
/* Creates a Render (behaviour inferred; see Game/StageMovie.cpp). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* The task tree's root, and the parent task of a layer (see Game/ContinueAction.cpp). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
}

class CsdOneShot : public Task {
  public:
    CsdOneShot(Task *parent, const char *file, const char *scene, const char *motion);
    virtual ~CsdOneShot();
    virtual void Tick(float dt);

    /* The Render's draw callback. */
    void Draw(RednerDrawArgs &args);
    /* Starts loading the project file. */
    void Load();
    /* The load callback. */
    static void OnLoaded(CsdOneShotRequest *request);

    /* 0x028 */ s32 m_state; /* 2 loading, 3 playing, 4 failed */
    /* 0x02C */ u8 m_stopped;
    /* 0x030 */ boost::shared_ptr<Render> m_render;
    /* 0x038 */ TPjsCsd m_csd;
    /* 0x08C */ Chao::CSD::CProject m_project;
    /* 0x0D8 */ char m_file[0x20];
    /* 0x0F8 */ char m_scene[0x20];
    /* 0x118 */ char m_motion[0x20];
};
SIZE_ASSERT(CsdOneShot, 0x138);

static CsdOneShotRequest s_request;

/* Creates a CsdOneShot playing motion of scene of the project file file, or nothing when the file
 * does not exist. */
extern "C" CsdOneShot *fn_8035367C(const char *file, const char *scene, const char *motion) {
    const char *path = fn_800423B8(fn_8003F8D4(), file, 0);
    if (fn_800371D8()->Exists(path)) {
        return new CsdOneShot(fn_801EB304(fn_8001C4F8(), 3), file, scene, motion);
    }
    return 0;
}

CsdOneShot::CsdOneShot(Task *parent, const char *file, const char *scene, const char *motion)
    : Task(parent), m_stopped(0) {
    m_name = "CsdOneShot";
    m_render = fn_8004E24C(fn_80046178()->field_0x38);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&CsdOneShot::Draw, this, _1));
    strcpy(m_file, file);
    strcpy(m_scene, scene);
    strcpy(m_motion, motion);
    Load();
}

CsdOneShot::~CsdOneShot() {}

void CsdOneShot::Tick(float dt) {
    switch (m_state) {
    case 2:
        break;
    case 3:
        if (m_stopped || !m_csd.m_playing) {
            m_flags |= 1;
        } else {
            fn_8003FE2C(m_csd);
        }
        break;
    case 4:
        break;
    }
}

void CsdOneShot::Draw(RednerDrawArgs &args) {
    if (m_state == 3) {
        fn_8003FDF4(m_csd);
    }
}

void CsdOneShot::Load() {
    s_request.m_path = fn_800423B8(fn_8003F8D4(), m_file, 0);
    s_request.m_data = 0;
    s_request.m_size = 0;
    s_request.m_user = this;
    s_request.m_callback = OnLoaded;
    fn_80045044(fn_800371D8(), &s_request, 1);
    m_state = 2;
    fn_8025D1C4(fn_80014B90(), this, 1, "CsdOneShot");
}

void CsdOneShot::OnLoaded(CsdOneShotRequest *request) {
    CsdOneShot *task = (CsdOneShot *)request->m_user;
    if (fn_8003E750(task->m_csd, request->m_data, request->m_size)) {
        strcpy(task->m_csd.m_sceneName, task->m_scene);
        fn_8003E388(task->m_csd, task->m_motion, 1);
        fn_8003FB10(task->m_csd);
        fn_8003FE2C(task->m_csd);
        task->m_state = 3;
    } else {
        task->m_state = 4;
    }
    fn_80049B34(request->m_data);
    fn_8025D144(fn_80014B90(), task);
}
