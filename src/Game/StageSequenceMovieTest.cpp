/* StageSequence::MovieTest::Phase (class name from RTTI): a debug phase of the stage sequence that
 * lists the movie files (".sfd") of the disc's root directory, lets the player pick one with the
 * pad (up, down, then the play button) and plays it as a StageMovie, printing the movie decoder's
 * status while it runs. Its state lives in a StageSequence::MovieTest::phaseImpl (class name from
 * the RTTI of its draw callback's bind_t). The file name and the names marked "(name inferred)" are
 * not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define MSL_STRING_DEFAULT_CTOR
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstdio>
#include <string>

#include "Game/Render.h"
#include "Game/StageSequence.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The argument a Render passes to its draw callbacks (unused here). */
struct RednerDrawArgs;

/* A file of a directory listing (struct name unknown; layout from its use). */
struct UnkDirEntry_80044EAC {
    /* 0x000 */ char m_name[0x104];
    /* 0x104 */ u32 m_flags; /* bit 1: a file (meaning inferred) */
};

/* The pad state fn_80051734 returns (struct name unknown); 0x30 holds the buttons pressed this
 * frame (meaning inferred). */
struct UnkPad_80051734 {
    /* Whether all of buttons, or all of other, were pressed this frame (name inferred; retail tests
     * the pressed buttons twice, so the original presumably took two masks, given the same
     * value here). */
    bool IsTriggered(u32 buttons, u32 other) {
        if ((m_triggered & buttons) == buttons) {
            return true;
        }
        if ((m_triggered & other) == other) {
            return true;
        }
        return false;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_triggered;
};

/* The movie decoder's status (struct name unknown; the members are named after the labels the
 * debug display prints for them). */
struct UnkMovieStatus_805EE0B0 {
    /* 0x000 */ u8 field_0x0[0xA4];
    /* 0x0A4 */ s32 frmid;
    /* 0x0A8 */ u8 field_0xA8[0xAC - 0xA8];
    /* 0x0AC */ s32 width;
    /* 0x0B0 */ s32 height;
    /* 0x0B4 */ s32 x_mb;
    /* 0x0B8 */ s32 y_mb;
    /* 0x0BC */ u8 field_0xBC[0xC0 - 0xBC];
    /* 0x0C0 */ s32 fps;
    /* 0x0C4 */ s32 fno;
    /* 0x0C8 */ s32 time;
    /* 0x0CC */ s32 tunit;
    /* 0x0D0 */ s32 concat_cnt;
    /* 0x0D4 */ s32 fno_per_file;
    /* 0x0D8 */ s32 time_per_file;
    /* 0x0DC */ s32 errcnt;
    /* 0x0E0 */ s32 rcvcnt;
    /* 0x0E4 */ u8 field_0xE4[0xE8 - 0xE4];
    /* 0x0E8 */ s32 usrdatsize;
    /* 0x0EC */ u8 field_0xEC[0x12C - 0xEC];
    /* 0x12C */ s32 sud_size;
};

/* The debug text printer and the movie decoder's status (.sbss pointers; meanings inferred). */
extern void *lbl_805EE170;
extern UnkMovieStatus_805EE0B0 *lbl_805EE0B0;

extern "C" {
/* The disc's file system, and its listing of directory: the number of entries, with the entries
 * returned in an allocated array that fn_80049B34 frees (behaviour inferred). */
void *fn_800371D8(void);
int fn_80044EAC(void *fileSystem, UnkDirEntry_80044EAC **entries, const char *directory, int arg3);
void fn_80049B34(UnkDirEntry_80044EAC *entries);
/* Finds pattern in text (behaviour inferred). */
char *fn_803AC6C0(const char *text, const char *pattern);
/* The pad (behaviour inferred). */
UnkPad_80051734 *fn_80051734(void);
/* Prints text at column x, line y (behaviour inferred). */
void fn_80051400(void *printer, const char *text, int x, int y, int arg4);
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
}

namespace StageSequence {
namespace MovieTest {

class phaseImpl {
  public:
    phaseImpl();
    ~phaseImpl();

    /* Runs the phase; false once it ended. */
    bool Tick();
    /* The Render's draw callback: the file list, or the movie's status (name inferred). */
    void Draw(RednerDrawArgs &args);
    /* Lists the movie files (name inferred). */
    void LoadFiles();

    /* Ends the phase (name inferred). */
    void Retire() {
        m_done = 1;
    }

    /* 0x000 */ boost::shared_ptr<StageBase::Base> m_stage;
    /* 0x008 */ boost::shared_ptr<Render> m_render;
    /* 0x010 */ std::string m_files[64];
    /* 0x310 */ s32 m_count;
    /* 0x314 */ s32 m_cursor;
    /* 0x318 */ u8 m_done;
    /* 0x31C */ s32 m_state; /* 0 choosing, 1 playing */
};

class Phase : public StageSequenceBase {
  public:
    Phase();
    virtual ~Phase();
    virtual bool Tick();
    virtual void Skip();

    /* 0x4 */ boost::scoped_ptr<phaseImpl> m_impl;
};

/* Deletes the phaseImpl (name inferred; see checked_delete below). */
inline void DeleteImpl(phaseImpl *impl) {
    delete impl;
}

} // namespace MovieTest
} // namespace StageSequence

/* Retail keeps boost::checked_delete<phaseImpl> out of line and the destructor at its own place;
 * this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<StageSequence::MovieTest::phaseImpl>(StageSequence::MovieTest::phaseImpl *x) {
    StageSequence::MovieTest::DeleteImpl(x);
}
} // namespace boost

SIZE_ASSERT(StageSequence::MovieTest::phaseImpl, 0x320);
SIZE_ASSERT(StageSequence::MovieTest::Phase, 0x8);

/* Not original: an unused table that pools the unit's .rodata strings in retail's order (the
 * linker drops the table; the RTTI names and literals below reuse these copies). The .rodata is
 * kept whole (Draw addresses it through one base), so it also holds the strings of the inline
 * code of boost and the MSL that this unit does not reconstruct. */
static const char *s_rodataPool[] = {
    "boost::_bi::bind_t<void, boost::_mfi::mf1<void, StageSequence::MovieTest::phaseImpl, RednerDrawArgs &>, boost::_bi::list2<boost::_bi::value<StageSequence::MovieTest::phaseImpl *>, boost::arg<1>>>",
    "StageSequence::MovieTest::Phase",
    "StageSequence::StageSequenceBase",
    "concat_cnt",
    "fno_per_file",
    "time_per_file",
    "usrdatsize",
    "sud_size",
    "Memory allocation failure",
    "boost::detail::sp_counted_base_impl<StageMovie *, boost::checked_deleter<StageMovie>>",
    "boost::detail::sp_counted_base",
    "std::exception",
    "exception",
    "std::bad_alloc",
    "bad_alloc",
    "boost::checked_deleter<StageMovie>",
    "basic_string::length error.  Resizing out of range\n",
};

/* The directory listed, the movie files' extension, and the format of a status line. */
static const char s_movieDirectory[] = ".";
static const char s_movieExtension[] = ".sfd";
static const char s_statusFormat[] = "%16s\t%d";

/* Prints name and value on line y of the status display (name inferred). */
inline void PrintStatus(const char *name, s32 value, int y) {
    char text[64];
    sprintf(text, s_statusFormat, name, value);
    fn_80051400(lbl_805EE170, text, 0x28, y, 1);
}

namespace StageSequence {
namespace MovieTest {

phaseImpl::phaseImpl() : m_done(0), m_state(0) {
    LoadFiles();
    m_render = fn_8004E24C(fn_80046178()->field_0x40);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&phaseImpl::Draw, this, _1));
}

phaseImpl::~phaseImpl() {}

bool phaseImpl::Tick() {
    if (m_done) {
        m_stage.reset();
        return false;
    }
    switch (m_state) {
    case 0:
        if (fn_80051734()->IsTriggered(0x40, 0x40)) {
            m_cursor--;
            if (m_cursor < 0) {
                m_cursor = 0;
            }
        } else if (fn_80051734()->IsTriggered(0x80, 0x80)) {
            m_cursor++;
            if (m_cursor >= m_count) {
                m_cursor = m_count - 1;
            }
        } else if ((fn_80051734()->m_triggered & 0x400) == 0x400) {
            {
                boost::shared_ptr<StageBase::Base> movie(new StageMovie(m_files[m_cursor].c_str()));
                m_stage = movie;
            }
            m_state = 1;
        }
        return true;
    case 1:
        if (m_stage->Tick() != 1) {
            m_stage.reset();
            m_state = 0;
        }
        return true;
    }
    return false;
}

void phaseImpl::Draw(RednerDrawArgs &args) {
    switch (m_state) {
    case 0:
        for (int i = 0; i < m_count; i++) {
            char text[64];
            if (i == m_cursor) {
                sprintf(text, "* %s", m_files[i].c_str());
            } else {
                sprintf(text, "  %s", m_files[i].c_str());
            }
            fn_80051400(lbl_805EE170, text, 1, i - 13, 1);
        }
        break;
    case 1: {
        UnkMovieStatus_805EE0B0 *status = lbl_805EE0B0;
        if (status != 0) {
            PrintStatus("frmid", status->frmid, -13);
            PrintStatus("width", status->width, -12);
            PrintStatus("height", status->height, -11);
            PrintStatus("x_mb", status->x_mb, -10);
            PrintStatus("y_mb", status->y_mb, -9);
            PrintStatus("fps", status->fps, -8);
            PrintStatus("fno", status->fno, -7);
            PrintStatus("time", status->time, -6);
            PrintStatus("tunit", status->tunit, -5);
            PrintStatus("concat_cnt", status->concat_cnt, -4);
            PrintStatus("fno_per_file", status->fno_per_file, -3);
            PrintStatus("time_per_file", status->time_per_file, -2);
            PrintStatus("errcnt", status->errcnt, -1);
            PrintStatus("rcvcnt", status->rcvcnt, 0);
            PrintStatus("usrdatsize", status->usrdatsize, 1);
            PrintStatus("sud_size", status->sud_size, 2);
        }
        break;
    }
    }
}

void phaseImpl::LoadFiles() {
    UnkDirEntry_80044EAC *entries;
    int count = fn_80044EAC(fn_800371D8(), &entries, s_movieDirectory, 0);
    m_count = 0;
    m_cursor = 0;
    if (entries != 0) {
        for (int i = 0; i < count; i++) {
            if ((entries[i].m_flags & 2) && fn_803AC6C0(entries[i].m_name, s_movieExtension) != 0) {
                m_files[m_count] = entries[i].m_name;
                m_count++;
            }
        }
        fn_80049B34(entries);
    }
}

Phase::Phase() : m_impl(new phaseImpl) {}

Phase::~Phase() {}

bool Phase::Tick() {
    return m_impl->Tick();
}

void Phase::Skip() {
    m_impl->Retire();
}

} // namespace MovieTest
} // namespace StageSequence
