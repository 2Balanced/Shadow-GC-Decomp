/* TGiPause (class name from RTTI): the task of the pause screen. It loads the CSD project of the
 * pause menu ("CsdFiles/pause/pause%04d", or "CsdFiles/pause/pauseBoss" in boss stages) and the
 * target window's project "CsdFiles/TWindow", with the menu's texture dictionary, then creates the
 * three panels (folderPause, folderStoryRoute, folderTargetWindow) and draws them from a Render
 * callback. While the menu or the story route is open it tells the pause manager. The file name
 * and the names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstdio>

#define TGI_NO_NUMBER_COLORS
#include "Game/Render.h"
#include "Game/StageTable.h"
#include "Game/TGi.h"
#include "Game/TGiPause.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The file system (as far as this unit uses it; see Game/ActStageController.cpp). */
class UnkFileSystem_800371D8 {
  public:
    /* 0x0 */ u32 field_0x0;
    /* Whether a file exists (name inferred). */
    bool Exists(const char *path) {
        return exists(path);
    }

    /* 0x4 */ bool (*exists)(const char *path);
};

/* A RenderWare memory stream's source (rwSTREAMMEMORY). */
struct TGiPauseRwMemory {
    /* 0x0 */ const void *start;
    /* 0x4 */ u32 length;
};

extern "C" {
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* The path of a localized file (behaviour inferred). */
const char *fn_800C02CC(const char *name);
/* Opens and closes a file name for the file system (behaviour inferred). */
const char *fn_80051810(const char *path);
void fn_80051830(const char *path);
UnkFileSystem_800371D8 *fn_800371D8(void);
/* Reads a whole file, and releases the data (see Game/AudioDataBank.cpp). */
void *fn_8004509C(UnkFileSystem_800371D8 *fileSystem, const char *path, u32 *size);
void fn_80049B34(void *data);
/* RenderWare: RwStreamOpen, RwStreamFindChunk, RwTexDictionaryStreamRead, RwStreamClose and
 * RwTexDictionaryDestroy (names inferred from their use). */
void *fn_80481368(int type, int accessType, void *data);
int fn_8047CF50(void *stream, u32 type, u32 *lengthOut, u32 *versionOut);
void *fn_80483FBC(void *stream);
int fn_8048124C(void *stream, void *data);
int fn_8048CB38(void *dict);
/* Gives a project the textures of dict (behaviour inferred). */
void fn_800421BC(Chao::CSD::CProject *project, void *dict);
}

/* Not original: an unused table that pools the unit's .rodata strings in retail's order (the
 * linker drops the table; the RTTI names and literals below reuse these copies). The .rodata is
 * kept whole (Load addresses it through one base), so it also holds the RTTI names of the CSD and
 * boost headers' inline classes, which this unit does not reconstruct. */
static const char *s_rodataPool[] = {
    "boost::_bi::bind_t<void, boost::_mfi::mf1<void, TGiPause, RednerDrawArgs &>, boost::_bi::list2<boost::_bi::value<TGiPause *>, boost::arg<1>>>",
    "TGiPause",
    "CsdFiles/TWindow",
    "CsdFiles/pause/pauseBoss",
    "CsdFiles/pause/pause%04d",
    "CsdFiles/pause/pause",
    "Memory allocation failure",
    "Not inititalized m_eDealloctor\n",
    "csdRCObject.h",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>::CFunctorBase",
    "Chao::CSD::CBase",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>",
    "Chao::CSD::CResourceBase<Chao::CSD::Project>",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>",
    "boost::detail::sp_counted_base_impl<folderPause *, boost::checked_deleter<folderPause>>",
    "boost::detail::sp_counted_base",
    "boost::detail::sp_counted_base_impl<folderStoryRoute *, boost::checked_deleter<folderStoryRoute>>",
    "boost::detail::sp_counted_base_impl<folderTargetWindow *, boost::checked_deleter<folderTargetWindow>>",
    "std::exception",
    "exception",
    "std::bad_alloc",
    "bad_alloc",
    "boost::checked_deleter<folderTargetWindow>",
    "boost::checked_deleter<folderStoryRoute>",
    "boost::checked_deleter<folderPause>",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>",
};

TGiPauseProjects::TGiPauseProjects() {}

TGiPause::TGiPause(Task *parent) : Task(parent) {
    m_name = "TGiPause";
    m_flags |= 0x100;
    m_textures = 0;
    if (Load()) {
        m_render = fn_8004E24C(fn_80046178()->field_0x38);
        fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&TGiPause::Draw, this, _1));
        m_pause = boost::shared_ptr<folderPause>(new folderPause(&m_projects.m_pause));
        m_storyRoute = boost::shared_ptr<folderStoryRoute>(new folderStoryRoute(&m_projects.m_pause));
        m_targetWindow =
            boost::shared_ptr<folderTargetWindow>(new folderTargetWindow(&m_projects.m_window));
    }
}

TGiPause::~TGiPause() {
    if (m_textures != 0) {
        fn_8048CB38(m_textures);
        m_textures = 0;
    }
}

void TGiPause::Tick(float dt) {
    if (m_projects.m_loaded != 0) {
        int open = m_pause->Update();
        open += m_storyRoute->Update();
        m_targetWindow->Update();
        if (lbl_805EF9F4 != 0) {
            if (open != 0) {
                lbl_805EF9F4->field_0x4D = 0;
            } else {
                lbl_805EF9F4->field_0x4D = 1;
            }
        }
    } else {
        m_flags |= 1;
    }
}

const char *TGiPause::GetProjectPath() {
    static char s_path[32];
    if (lbl_8057D6A0.field_0xB1 || lbl_8057D6A0.field_0x125 ||
        (fn_80176C3C(fn_800B74B4(), -1)->m_flags & 0x10) ||
        (fn_80176C3C(fn_800B74B4(), -1)->m_flags & 4)) {
        sprintf(s_path, "CsdFiles/pause/pauseBoss");
    } else {
        sprintf(s_path, "CsdFiles/pause/pause");
    }
    return fn_800423B8(fn_8003F8D4(), s_path, 0);
}

bool TGiPause::Load() {
    bool loaded = false;
    if (fn_8036E114(&m_projects.m_pause, GetProjectPath()) &&
        fn_8036E114(&m_projects.m_window, fn_800423B8(fn_8003F8D4(), "CsdFiles/TWindow", 0))) {
        loaded = true;
    }
    m_projects.m_loaded = loaded;
    if (m_projects.m_loaded != 0) {
        char path[40];
        if (lbl_8057D6A0.field_0xB1 || lbl_8057D6A0.field_0x125 ||
        (fn_80176C3C(fn_800B74B4(), -1)->m_flags & 0x10) ||
        (fn_80176C3C(fn_800B74B4(), -1)->m_flags & 4)) {
            sprintf(path, "CsdFiles/pause/pauseBoss");
        } else {
            sprintf(path, "CsdFiles/pause/pause%04d", lbl_8057D6A0.field_0xA8);
        }
        if (LoadTextures(fn_800C02CC(path))) {
            fn_8003F8D4();
            fn_800421BC(&m_projects.m_pause, m_textures);
            fn_8003F8D4();
            fn_800421BC(&m_projects.m_window, m_textures);
        }
    }
    return m_projects.m_loaded != 0;
}

bool TGiPause::LoadTextures(const char *path) {
    const char *name = fn_80051810(path);
    bool loaded = false;
    if (fn_800371D8()->Exists(name)) {
        u32 size;
        void *data = fn_8004509C(fn_800371D8(), path, &size);
        if (data != 0) {
            TGiPauseRwMemory memory;
            memory.start = data;
            memory.length = size;
            void *stream = fn_80481368(3, 1, &memory);
            if (stream != 0) {
                if (fn_8047CF50(stream, 0x16, 0, 0)) {
                    m_textures = fn_80483FBC(stream);
                    loaded = true;
                }
                fn_8048124C(stream, &memory);
            }
            fn_80049B34(data);
        }
    }
    fn_80051830(name);
    return loaded;
}

void TGiPause::Draw(RednerDrawArgs &args) {
    if (lbl_8057D6A0.m_hudVisible) {
        fn_8036D94C(&m_projects.m_window);
        m_targetWindow->Draw();
        fn_8036D94C(&m_projects.m_pause);
        m_pause->Draw();
    }
}
