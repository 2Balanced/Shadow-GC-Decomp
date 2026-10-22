/* StageMovie (class name from RTTI; see Game/StageSequence.h): a stage that plays a movie file
 * ("E<number>.sfd"). It starts the movie a few frames after it is created, draws the subtitles of
 * the movie's event script ("event/event<number>.scr", a StageMovieScript) from a Render's draw
 * callback, lets the player skip movies seen before, and reports 7 once the movie ended. The file
 * name and the names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstdio>
#include <cstring>

#include "Game/Render.h"
#include "Game/StageSequence.h"
#include "Game/UnkHintManager.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ float m_frameTime; /* the frame's duration (meaning inferred) */
    /* 0x024 */ u8 field_0x24[0x150 - 0x24];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The movie playing (struct name unknown; see Game/StageSequenceMovieTest.cpp). Setting bit 0 of
 * the halfword at 0x4 ends it (meaning inferred). */
struct UnkMovieStatus_805EE0B0 {
    /* 0x00 */ u8 field_0x0[0x4];
    /* 0x04 */ u16 m_flags;
    /* 0x06 */ u8 field_0x6[0xA4 - 0x6];
    /* 0xA4 */ s32 frmid; /* the frame shown */
};

extern UnkMovieStatus_805EE0B0 *lbl_805EE0B0;

/* The pad state fn_80051734 returns (struct name unknown; see Game/PjsEventActionManager.cpp). */
struct UnkPad_80051734 {
    /* 0x00 */ u8 field_0x0[0xD0];
    /* 0xD0 */ u32 m_pressed; /* 0x400: the skip button (meaning inferred) */
};

/* The save data fn_800BB74C returns (class unknown; see Game/SecretKey.cpp). */
struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0x1754];
    /* 0x1754 */ u8 field_0x1754[4]; /* the movies seen (meaning inferred; size unknown) */
};

/* A subtitle of an event script: a message shown from a frame on (struct name unknown). */
struct UnkScriptEntry_8004509C {
    /* 0x00 */ s32 m_frame;
    /* 0x04 */ s32 m_messageId; /* none when negative */
    /* 0x08 */ u8 field_0x8[0x80 - 0x8];
};

/* A loaded event script (struct name unknown). */
struct UnkScript_8004509C {
    /* 0x000 */ u8 field_0x0[0x4];
    /* 0x004 */ s32 m_count;
    /* 0x008 */ u8 field_0x8[0x18C - 0x8];
    /* 0x18C */ UnkScriptEntry_8004509C m_entries[1];
};

/* TMessageDisp (class name from the RTTI of its shared_ptr control block; see
 * Game/StageManager.cpp). */
class UnkHintTarget_801FB180 {
  public:
    /* 0x00 */ u8 field_0x0[0x64];
    /* 0x64 */ float m_scale;
};

class TMessageDisp : public UnkHintTarget_801FB180 {
  public:
    /* 0x68 */ u8 field_0x68[0x74 - 0x68];
};

UnkHintManager_801FB180 *GetHintManager();

extern "C" {
/* Reads values from text (behaviour inferred; sscanf's signature). */
int fn_803AB5B8(const char *text, const char *format, ...);
/* Starts playing a movie file (behaviour inferred). */
bool fn_80036E78(const char *fileName, int arg1, int arg2, int arg3);
void fn_80036570(int arg0, int arg1, float arg2, float arg3);
UnkSaveData_800BB74C *fn_800BB74C(void);
void fn_801E4F70(UnkSaveData_800BB74C *save, const char *fileName);
bool fn_80174424(void *flags, int index);
void fn_80174448(void *flags, int index);
/* The index of a movie in the list of movies the player can watch again, or -1 (meaning
 * inferred). */
int fn_8025D280(int movieId);
void fn_802BEB90(void);
void fn_80173834(int arg0, float arg1);
UnkPad_80051734 *fn_80051734(void);
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* The disc's file system, and a file loaded from it (behaviour inferred). */
void *fn_800371D8(void);
UnkScript_8004509C *fn_8004509C(void *fileSystem, const char *path, int arg2);
void fn_80049B34(void *data);
void fn_80173ED0(UnkScript_8004509C *script);
TMessageDisp *fn_8004ABD0(TMessageDisp *message, const char *name, int arg2);
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
void fn_801FAFFC(UnkHintManager_801FB180 *manager);
void fn_801FB0BC(UnkHintManager_801FB180 *manager,
                 const boost::weak_ptr<UnkHintTarget_801FB180> &target);
}

/* The subtitles of a movie (class name unknown): its event script, and the message source they
 * are shown through. */
class StageMovieScript {
  public:
    StageMovieScript(const char *movieName);
    ~StageMovieScript();

    /* Shows the subtitles that start after frame from, up to frame to (name inferred). */
    void Update(int from, int to);

    /* 0x0 */ boost::shared_ptr<UnkHintTarget_801FB180> m_message;
    /* 0x8 */ UnkScript_8004509C *m_script;
};
SIZE_ASSERT(StageMovieScript, 0xC);
SIZE_ASSERT(StageMovie, 0x58);

/* Deletes the script (name inferred; see checked_delete below). */
inline void DeleteScript(StageMovieScript *script) {
    delete script;
}

/* Retail keeps boost::checked_delete<StageMovieScript> out of line and the destructor at its own
 * place; this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <> inline void checked_delete<StageMovieScript>(StageMovieScript *x) {
    DeleteScript(x);
}
} // namespace boost

StageMovieScript::StageMovieScript(const char *movieName) : m_script(0) {
    TMessageDisp *message = (TMessageDisp *)operator new(0x74);
    if (message != 0) {
        message = fn_8004ABD0(message, "Movie", 1);
    }
    m_message = boost::shared_ptr<UnkHintTarget_801FB180>(message);
    if (m_message) {
        fn_801FB0BC(GetHintManager(), boost::weak_ptr<UnkHintTarget_801FB180>(m_message));
        char path[0x40];
        int movieId;
        fn_803AB5B8(movieName, "E%04d.sfd", &movieId);
        sprintf(path, "event/event%04d.scr", movieId);
        m_script = fn_8004509C(fn_800371D8(), path, 0);
        if (m_script != 0) {
            fn_80173ED0(m_script);
        }
    }
}

StageMovieScript::~StageMovieScript() {
    if (m_script != 0) {
        fn_80049B34(m_script);
        m_script = 0;
    }
    fn_801FAFFC(GetHintManager());
}

void StageMovieScript::Update(int from, int to) {
    if (m_script != 0) {
        int messageId;
        for (int i = 0; i < m_script->m_count; i++) {
            if (m_script->m_entries[i].m_frame > from && m_script->m_entries[i].m_frame <= to) {
                messageId = m_script->m_entries[i].m_messageId;
                if (messageId >= 0) {
                    fn_801FAF00(GetHintManager(), messageId, 0, 1, 0);
                }
            }
        }
    }
}

static UnkMovieStatus_805EE0B0 *GetMovieStatus();

StageMovie::StageMovie(const char *fileName)
    : StageBase::Base(2), m_frame(0), m_prevFrame(0), m_firstView(1), m_delay(2), m_failures(0),
      m_ended(0), m_state(0), m_script(new StageMovieScript(fileName)) {
    fn_802BEB90();
    strcpy(m_fileName, fileName);
    UnkHintManager_801FB180 *manager = GetHintManager();
    manager->m_altLayout = 1;
}

StageMovie::~StageMovie() {
    if (lbl_805EE0B0 != 0) {
        GetMovieStatus()->m_flags |= 1;
    }
    UnkHintManager_801FB180 *manager = GetHintManager();
    manager->m_altLayout = 0;
}

/* The movie playing (name inferred). */
static UnkMovieStatus_805EE0B0 *GetMovieStatus() {
    return lbl_805EE0B0;
}

int StageMovie::Tick() {
    switch (m_state) {
    case 0:
        if (m_delay == 0) {
            if (Start()) {
                if (m_movieId != 1001) {
                    fn_80173834(1, 0.0f);
                    m_state = 1;
                } else {
                    m_state = 4;
                    m_wait = 0.5f;
                }
            } else {
                m_failures++;
                if (m_failures > 100) {
                    m_state = 2;
                }
            }
        } else {
            m_delay--;
        }
        return 1;
    case 1:
        if (lbl_805EE0B0 != 0) {
            if ((!m_firstView && (fn_80051734()->m_pressed & 0x400) == 0x400) || m_ended) {
                GetMovieStatus()->m_flags |= 1;
            }
            return 1;
        }
        m_state = 3;
        break;
    case 4:
        m_wait -= lbl_8057D6A0.m_frameTime;
        if (m_wait <= 0.0f) {
            fn_80173834(1, 0.0f);
            m_state = 1;
        }
        return 1;
    }
    return 7;
}

void StageMovie::vf_0x10() {
    m_ended = 1;
}

void StageMovie::Draw(RednerDrawArgs &args) {
    if (lbl_805EE0B0 != 0) {
        fn_80036570(0, 0, 1.0f, 1.0f);
        if (lbl_805EE0B0 != 0) {
            m_frame = lbl_805EE0B0->frmid;
            m_script->Update(m_prevFrame, m_frame);
            m_prevFrame = m_frame;
        }
    }
}

bool StageMovie::Start() {
    fn_803AB5B8(m_fileName, "E%04d.sfd", &m_movieId);
    if (fn_80036E78(m_fileName, 0, 0, 0)) {
        fn_801E4F70(fn_800BB74C(), m_fileName);
    } else {
        return false;
    }
    m_render = fn_8004E24C(fn_80046178()->field_0x4C);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&StageMovie::Draw, this, _1));
    int index = fn_8025D280(m_movieId);
    if (index != -1) {
        UnkSaveData_800BB74C *save = fn_800BB74C();
        if (fn_80174424(save->field_0x1754, index)) {
            m_firstView = 0;
        } else {
            m_firstView = 1;
        }
        fn_80174448(save->field_0x1754, index);
    }
    fn_802BEB90();
    return true;
}
