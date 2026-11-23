/* TTextureChange (class name from RTTI): a task that animates the textures of up to 32 materials.
 * Each material follows a texture animation (struct name unknown): a list of frames, each the
 * time it starts and the number appended to the animation's texture name, looked up in the current
 * texture dictionary. The frame time can be pinned for every animation with lbl_805E39A0 (-1: not
 * pinned). The task registers a handle (HandleUser) so that its creator (fn_8004F678) can find it
 * again. The unit also holds the game's copies of the std::string constructor from a C string, of
 * append and of concatenation (fn_8004F5DC, which other units call by that name). Names other
 * than the class's are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first).
 *
 * NonMatching (14/15): Setup differs in 5 rows, all in the frame loop: retail adds the offset to
 * the animation and loads the frame's index at +80 (`add r4,r5,r23; lwz r6,80(r4)`) where GC/2.7
 * adds 80 to the offset and uses an indexed load, and so keeps the animation in r5 where GC/2.7
 * has r4. Wave 40 fixed the saved registers through the declaration order of the locals (the
 * dictionary, the entry and the counter declared at the top) and the copy of the zeroed counter
 * (`mr r23,r22`) by inlining the frame loop from a function (LoadFrames); about 47 forms of the
 * loop and the frame's address (the counter zeroed in a loop of one pass, a second index or a
 * byte offset copied from it, the frame through a pointer, a reference or explicit byte
 * arithmetic, locals for the animation, the frames, the name or the index, a u32 counter, a
 * while loop, the name buffer outside the loop, the helper as a member of the entry or over
 * pointers to the entry's members; probes/Game_TTextureChange_W40*.py) leave those 5 rows. The
 * rest of the unit is ready: .data and .rodata differ only by alignment tails, and the extra
 * .text is the weak HandleUser and std::string members, which fold.
 * GetFrame matches since wave 39 (the declaration order of its locals, see there).
 * lbl_805E39A0 is defined by the next unit (Game/AdbManager.cpp), which starts its .sdata. */
#define MSL_STRING_DEFAULT_CTOR
#define MSL_STRING_COPY
#define MSL_STRING_APPEND
#include <string>
#include <stdio.h>
#include <string.h>

#include "Game/HandleUser.h"
#include "Game/Task.h"
#include "Game/UnkHandleTable.h"
#include <dolphin/types.h>
#include "size_assert.h"

struct RwTexture;
struct RwTexDictionary;

extern "C" {
void *fn_80049B90(u32 size, int heap);
void fn_80049B34(void *block);
/* RenderWare: the current texture dictionary, a texture by name, and a material's texture. */
RwTexDictionary *fn_8048D0D8(void);
RwTexture *fn_8048CFFC(RwTexDictionary *dict, const char *name);
void fn_8045B270(void *material, RwTexture *texture);
/* The task tree (Game/UnkTaskLayers_801EB424.cpp): the root, and one of its layers. */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* The handle table (Game/UnkHandles_800459CC.cpp): giving a HandleUser a handle, and taking it
 * back. */
void fn_80045A64(HandleUser &user);
void fn_800459CC(HandleUser &user);
}

/* The frame time all animations show when not negative (debug; set elsewhere). */
extern float lbl_805E39A0;

/* A frame of a texture animation: when it starts (in 1/60 s) and the number of its texture. */
struct UnkTextureFrame_8004F0A4 {
    /* 0x0 */ s32 m_time;
    /* 0x4 */ s32 m_index;
};

/* A texture animation (struct name unknown). */
struct UnkTextureAnim_8004F0A4 {
    /* 0x00 */ s32 m_length; /* in 1/60 s; the animation loops */
    /* 0x04 */ char m_baseName[0x20]; /* the texture the material has */
    /* 0x24 */ char m_name[0x24];     /* the frames' textures are <m_name>.<index> */
    /* 0x48 */ s32 m_count;
    /* 0x4C */ UnkTextureFrame_8004F0A4 m_frames[1];
};

struct UnkTextureAnimHolder_8004F22C {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ UnkTextureAnim_8004F0A4 *m_anim;
};

/* A material and its animation, as the creator passes them. */
struct UnkTextureChangeParam_8004F22C {
    /* 0x0 */ void *m_material;
    /* 0x4 */ UnkTextureAnimHolder_8004F22C *m_holder;
};

/* One animated material. */
struct UnkTextureChangeEntry_8004F0A4 {
    /* 0x00 */ UnkTextureAnim_8004F0A4 *m_anim;
    /* 0x04 */ void *m_material;
    /* 0x08 */ RwTexture *m_base;
    /* 0x0C */ RwTexture **m_textures; /* one per frame */
    /* 0x10 */ s32 m_current;
};

class TTextureChange : public Task, public HandleUser {
  public:
    TTextureChange(Task *parent, UnkTextureChangeParam_8004F22C *params, s32 count, const char *name);
    virtual ~TTextureChange();
    virtual void Tick(float dt);

    bool Setup(UnkTextureChangeParam_8004F22C *params, s32 count);
    void FreeTextures();
    void Update(s32 index);
    s32 GetFrame(UnkTextureChangeEntry_8004F0A4 *entry);

    /* 0x030 */ s32 m_count;
    /* 0x034 */ float m_time; /* in 1/60 s */
    /* 0x038 */ std::string m_label;
    /* 0x044 */ UnkTextureChangeEntry_8004F0A4 m_entries[32];
    /* 0x2C4 */ u32 field_0x2C4;
};
SIZE_ASSERT(TTextureChange, 0x2C8);

extern "C" {
UnkHandle_80045A98 fn_8004F678(void *params, s32 count, const char *name, Task *parent);
std::string fn_8004F5DC(const std::string &a, const char *b);
}

std::string UnkStringConcat(const std::string &a, const std::string &b);

/* String concatenation: the game's copies (fn_8004F5DC is called by other units by that name).
 * Defined as friends of a class template, so that GC/2.7 treats them as the template instances
 * they stand for: kept out of line and emitted after their first caller. */
template <class T> struct UnkStringConcat_8004F5DC {
    friend std::string fn_8004F5DC(const std::string &a, const char *b) {
        std::string result(a);
        return result.append(b);
    }
    friend std::string UnkStringConcat(const std::string &a, const std::string &b) {
        std::string result(a);
        return result.append(b);
    }
};
template struct UnkStringConcat_8004F5DC<char>;

UnkHandle_80045A98 fn_8004F678(void *params, s32 count, const char *name, Task *parent) {
    if (parent) {
        TTextureChange *task =
            new TTextureChange(parent, (UnkTextureChangeParam_8004F22C *)params, count, name);
        return *(UnkHandle_80045A98 *)task->GetHandle();
    }
    TTextureChange *task = new TTextureChange(fn_801EB304(fn_8001C4F8(), 3),
                                              (UnkTextureChangeParam_8004F22C *)params, count, name);
    return *(UnkHandle_80045A98 *)task->GetHandle();
}

TTextureChange::TTextureChange(Task *parent, UnkTextureChangeParam_8004F22C *params, s32 count,
                               const char *name)
    : Task(parent) {
    m_label = fn_8004F5DC(UnkStringConcat(fn_8004F5DC(std::string("Texture Change : "), name), std::string(" : ")),
                          params->m_holder->m_anim->m_baseName);
    m_name = m_label.c_str();
    fn_80045A64(*this);
    Setup(params, count);
}

TTextureChange::~TTextureChange() {
    FreeTextures();
    fn_800459CC(*this);
}

void TTextureChange::Tick(float dt) {
    m_time += 60.0f * dt;
    for (s32 i = 0; i < m_count; i++) {
        Update(i);
    }
}

/* The textures of the animation's frames (name inferred). A loop inlined from a function starts
 * its strength-reduced offset with a copy of the zeroed counter (`mr r23,r22`), as retail does;
 * written in place it gets a second `li`. */
inline void LoadFrames(UnkTextureChangeEntry_8004F0A4 *entry, RwTexDictionary *dict) {
    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        char name[0x40];
        sprintf(name, "%s.%d", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
    }
}

bool TTextureChange::Setup(UnkTextureChangeParam_8004F22C *params, s32 count) {
    /* Declared in the order of retail's saved registers (r31, r30, r29; the counter of the frame
     * loop inlined from LoadFrames shares r22 with the parameter record). */
    RwTexDictionary *dict;
    UnkTextureChangeEntry_8004F0A4 *entry;
    s32 i;
    m_count = 0;
    m_time = 0.0f;
    memset(m_entries, 0, sizeof(m_entries));
    for (i = 0; i < count; i++) {
        UnkTextureChangeParam_8004F22C *param = &params[i];
        entry = &m_entries[m_count];
        entry->m_material = param->m_material;
        dict = fn_8048D0D8();
        if (dict) {
            RwTexture *base = fn_8048CFFC(dict, param->m_holder->m_anim->m_baseName);
            if (base) {
                entry->m_anim = param->m_holder->m_anim;
                entry->m_base = base;
                entry->m_textures = (RwTexture **)fn_80049B90(entry->m_anim->m_count * 4, 2);
                LoadFrames(entry, dict);
                m_count++;
            }
        }
    }
    return true;
}

void TTextureChange::FreeTextures() {
    for (s32 i = 0; i < m_count; i++) {
        if (m_entries[i].m_textures) {
            fn_80049B34(m_entries[i].m_textures);
        }
    }
}

void TTextureChange::Update(s32 index) {
    UnkTextureChangeEntry_8004F0A4 *entry = &m_entries[index];
    s32 frame = GetFrame(entry);
    if (frame >= 0 && frame != entry->m_current) {
        fn_8045B270(entry->m_material, entry->m_textures[frame]);
        entry->m_current = frame;
    }
}

s32 TTextureChange::GetFrame(UnkTextureChangeEntry_8004F0A4 *entry) {
    /* The frame table is declared after the counter: GC/2.7 hands out the registers in declaration
     * order, and retail keeps the animation in r5, its length in r6, the counter in r7 and the
     * table in r8 (wave 39; probes/Game_TTextureChange.py). */
    UnkTextureAnim_8004F0A4 *anim = entry->m_anim;
    float time;
    s32 length;
    s32 count;
    s32 loops;
    s32 i;
    UnkTextureFrame_8004F0A4 *frames = anim->m_frames;
    if (lbl_805E39A0 >= 0.0f) {
        time = lbl_805E39A0;
    } else {
        time = m_time;
    }
    length = anim->m_length;
    count = anim->m_count;
    loops = time / length;
    time -= length * loops;
    for (i = 0; i < count; i++) {
        if (time < frames[i].m_time) {
            break;
        }
    }
    return i - 1;
}

