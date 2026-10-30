/* The .SNN file of an event scene (class name unknown; Game/PjsEventActionScene.cpp keeps one): it
 * loads the .SNN file of the scene's archive, builds an animation from it and plays it at the
 * scene's time, flagging the end of the animation. It works like the scene's .SNB camera
 * (UnkEventCamera_80171F80 in Game/PjsEventActionScene.cpp). File and member names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <string.h>

#include "size_assert.h"
#include <dolphin/types.h>

/* The game state (struct name unknown; see Game/PjsEventActionScene.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x50];
    /* 0x050 */ s32 m_frameRate; /* (name inferred) */
    /* 0x054 */ u8 field_0x54[0x150 - 0x54];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The animation built from the file (class name unknown; constructor 0x80048A44, destructor
 * 0x800489C0, advance 0x8004883C). */
class UnkSnnAnim_80048A44 {
  public:
    UnkSnnAnim_80048A44(void *data, int arg1, int arg2, int arg3);

    /* 0x00 */ u8 field_0x0[0x24];
};
SIZE_ASSERT(UnkSnnAnim_80048A44, 0x24);

class UnkSnnFile_802289B0 {
  public:
    UnkSnnFile_802289B0();
    ~UnkSnnFile_802289B0();

    /* 0x00 */ void *m_data;     /* the file's animation data */
    /* 0x04 */ void *m_resource; /* the loaded file */
    /* 0x08 */ UnkSnnAnim_80048A44 *m_anim;
    /* 0x0C */ float m_time;
    /* 0x10 */ u8 m_finished;
    /* 0x11 */ u8 m_loading;
    /* 0x12 */ char m_name[0x54 - 0x12];
};
SIZE_ASSERT(UnkSnnFile_802289B0, 0x54);

extern "C" {
/* Files of an archive: the name of one, and loading one with a callback (see Game/CenFile.cpp). */
const char *fn_8004CA28(void *archive, int index);
void fn_8004C358(void *archive, int index, void (*callback)(void *, UnkSnnFile_802289B0 *),
                 UnkSnnFile_802289B0 *user);
/* Whether name ends in the extension ext (Game/PjsEventActionScene.cpp). */
bool fn_80171E74(const char *name, const char *ext);
/* A loaded resource: whether it is valid, and one of its parts (see Game/BossCtrlUnit.cpp). */
int fn_80010648(void *resource);
void *fn_80011CE8(void *resource, int arg1);
/* Frees a buffer. */
void fn_80049B34(void *p);

/* The animation (behaviours inferred): destroy, and advance by step (true at its end). */
void fn_800489C0(UnkSnnAnim_80048A44 *anim, int deleteFlag);
int fn_8004883C(UnkSnnAnim_80048A44 *anim, float step);

void fn_8022873C(void *resource, UnkSnnFile_802289B0 *file);
void fn_802287C4(UnkSnnFile_802289B0 *file);
void fn_8022890C(UnkSnnFile_802289B0 *file);
}

/* The files' extension (TPjsEventActionScene::IsSnnFile reads it too). */
extern const char lbl_805F6F80[];
const char lbl_805F6F80[] = ".SNN";

UnkSnnFile_802289B0::UnkSnnFile_802289B0() {
    m_data = 0;
    m_resource = 0;
    m_anim = 0;
    m_time = 0.0f;
    m_finished = 0;
    m_loading = 0;
    m_name[0] = 0;
}

UnkSnnFile_802289B0::~UnkSnnFile_802289B0() {
    fn_8022890C(this);
}

/* Destroys the animation and frees the file (name inferred). */
extern "C" void fn_8022890C(UnkSnnFile_802289B0 *file) {
    if (file->m_anim != 0) {
        fn_800489C0(file->m_anim, 1);
        file->m_anim = 0;
    }
    if (file->m_resource != 0) {
        fn_80049B34(file->m_resource);
        file->m_resource = 0;
    }
}

/* Starts loading file index of archive if it is a .SNN file (name inferred). */
extern "C" bool fn_8022887C(UnkSnnFile_802289B0 *file, void *archive, int index) {
    if (fn_80171E74(fn_8004CA28(archive, index), lbl_805F6F80)) {
        file->m_loading = 1;
        strcpy(file->m_name, fn_8004CA28(archive, index));
        fn_8004C358(archive, index, fn_8022873C, file);
        return true;
    }
    return false;
}

/* Whether the file is still loading (name inferred). */
extern "C" u8 fn_80228874(UnkSnnFile_802289B0 *file) {
    return file->m_loading;
}

/* Plays the animation at time (name inferred). */
extern "C" void fn_80228844(UnkSnnFile_802289B0 *file, float time) {
    if (file->m_anim != 0) {
        file->m_time = time;
        fn_802287C4(file);
    }
}

/* Advances the animation by one frame (name inferred). */
extern "C" void fn_802287C4(UnkSnnFile_802289B0 *file) {
    if (file->m_time < 0.0f) {
        return;
    }
    if (fn_8004883C(file->m_anim, 1.0f / lbl_8057D6A0.m_frameRate)) {
        file->m_finished = 1;
    }
}

/* Called when the file has loaded: builds the animation (name inferred). */
extern "C" void fn_8022873C(void *resource, UnkSnnFile_802289B0 *file) {
    file->m_resource = resource;
    if (fn_80010648(file->m_resource) != 0) {
        file->m_data = fn_80011CE8(file->m_resource, 4);
    }
    file->m_anim = new UnkSnnAnim_80048A44(file->m_data, 0, 0, 0);
    fn_802287C4(file);
    file->m_loading = 0;
}
