#ifndef GAME_TGI_H
#define GAME_TGI_H

/* Shared types of the TGi heads-up-display family (TGiCommandWindow, TGiRingChallenge,
 * TGiScoreDisplay, ...; class names from RTTI). Each display is a Task that keeps its state in an
 * Impl drawn by a Render callback (boost::bind of the Impl's Draw), plays CSD projects and prints
 * numbers with TGiNumberDisplay. Only the TGi units include this header. */

#include "Game/CsdRCPtr.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* RCPtr's operator-> (inline in the TGi units only; it stays out of line where its result feeds an
 * inline call, as in scene->IsEnd()). */
namespace Chao {
namespace CSD {
template <class T>
inline T *RCPtr<T>::operator->() const {
    return Get();
}
} // namespace CSD
} // namespace Chao

/* An RGBA colour as the number display takes it (struct name inferred; one word per channel). */
struct TGiColor {
    /* 0x0 */ s32 r;
    /* 0x4 */ s32 g;
    /* 0x8 */ s32 b;
    /* 0xC */ s32 a;
};

/* The four colours of a number display (struct name inferred). */
struct TGiColorSet {
    /* 0x00 */ TGiColor m_colors[4];
};

/* The colours every TGi unit gives its number displays (each unit has its own copy at the start of
 * its .rodata). Units without number displays (Game/GiGameOver.cpp) define TGI_NO_NUMBER_COLORS. */
#ifndef TGI_NO_NUMBER_COLORS
static const TGiColorSet s_numberColors = {{
    {255, 255, 255, 255},
    {128, 128, 128, 255},
    {64, 64, 64, 255},
    {255, 255, 255, 255},
}};
#endif

/* TGiNumberDisplay (class name from the RTTI of its shared_ptr control block): draws a string of
 * digits from a font texture. Its code lies around 0x800BC7BC; only the constructor and the
 * destructor are named, the members are called through their fn_ names. */
class TGiNumberDisplay {
  public:
    TGiNumberDisplay();
    ~TGiNumberDisplay();

    /* Scales the digits (name inferred). */
    void SetScale(float scale) {
        m_scaleX = scale;
        m_scaleY = scale;
    }

    /* Scales the digits by x and y (name inferred; ResultTask). */
    void SetScale(float x, float y) {
        m_scaleX = x;
        m_scaleY = y;
    }

    /* Sets the word at 0x14 (name inferred; ResultTask). */
    void SetField14(float value) {
        field_0x14 = value;
    }

    /* 0x000 */ u8 field_0x0[0x8];
    /* 0x008 */ float m_scaleX;
    /* 0x00C */ float m_scaleY;
    /* 0x010 */ u8 field_0x10[0x14 - 0x10];
    /* 0x014 */ float field_0x14; /* meaning unknown */
    /* 0x018 */ u8 field_0x18[0x4E0 - 0x18];
};
SIZE_ASSERT(TGiNumberDisplay, 0x4E0);

/* The loaded CSD project (Chao::CSD::CProject, Game/csdmProject.cpp; 0x4C bytes, fn_8003E808
 * constructs it, fn_8036E114 loads a file into it; EggLastMechSlotDisp2D.cpp keeps its own copy of
 * this declaration). */
namespace Chao {
namespace CSD {
class CProject {
  public:
    CProject();
    virtual ~CProject();

    /* 0x04 */ u8 field_0x4[0x4C - 0x4];

    static void *operator new(u32 size) {
        return fn_803605AC()->Alloc(size, "csdBase.h", 40);
    }
};
} // namespace CSD
} // namespace Chao

/* The value a project returns from fn_8036D598 (class name unknown; as in
 * EggLastMechSlotDisp2D.cpp). */
class UnkCsdHandle_80042DF0 {
  public:
    /* fn_80042E5C (ResultTask makes an empty one to compare with). */
    UnkCsdHandle_80042DF0(void *object);
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* The object the handle holds (name inferred; ResultTask). */
    void *Get() const {
        return m_object != 0 ? *(void **)m_object : 0;
    }

    /* 0x0 */ void *m_object;
};

/* The game state (struct name unknown; as far as the TGi units read it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x58];
    /* 0x058 */ s32 m_language; /* indexes the suffixes _JP, _EN, ... (meaning inferred; ResultTask) */
    /* 0x05C */ u8 field_0x5C[0x84 - 0x5C];
    /* 0x084 */ u8 field_0x84; /* meaning unknown */
    /* 0x085 */ u8 field_0x85[0x90 - 0x85];
    /* 0x090 */ u32 m_frame; /* meaning inferred */
    /* 0x094 */ float m_time; /* meaning inferred */
    /* 0x098 */ u8 field_0x98[0x9C - 0x98];
    /* 0x09C */ float field_0x9C; /* meaning unknown */
    /* 0x0A0 */ float m_time2; /* meaning inferred */
    /* 0x0A4 */ u8 field_0xA4[0xAC - 0xA4];
    /* 0x0AC */ s32 m_stage; /* (name inferred; see Game/CenFile.cpp) */
    /* 0x0B0 */ u8 field_0xB0;
    /* 0x0B1 */ u8 field_0xB1; /* meaning unknown (ResultTask ranks by a lower total when set) */
    /* 0x0B2 */ u8 m_twoPlayer; /* meaning inferred */
    /* 0x0B3 */ u8 field_0xB3[0xB8 - 0xB3];
    /* 0x0B8 */ s32 m_total; /* the result's total (meaning inferred; ResultTask) */
    /* 0x0BC */ s32 m_rank;  /* the result's rank (meaning inferred; ResultTask) */
    /* 0x0C0 */ u8 field_0xC0[0x134 - 0xC0];
    /* 0x134 */ s32 m_stars[2]; /* meaning inferred */
    /* 0x13C */ u8 field_0x13C[0x148 - 0x13C];
    /* 0x148 */ u8 m_hudVisible; /* meaning inferred */
    /* 0x149 */ u8 field_0x149;
    /* 0x14A */ u8 field_0x14A; /* meaning unknown */
    /* 0x14B */ u8 field_0x14B[0x150 - 0x14B];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A player's record (struct name unknown; as far as the TGi units read it). */
struct UnkPlayerInfo_80178B5C {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ s32 m_weaponKind;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
    /* 0x18 */ s32 m_vehicleWeapon;
    /* 0x1C */ s32 m_ammo;
    /* 0x20 */ u8 m_lives[0x8]; /* read by fn_8016EF7C and fn_8016EF84 (meaning inferred) */
    /* 0x28 */ u8 m_rings[0x4]; /* read by fn_8016F280 (meaning inferred) */
    /* 0x2C */ s32 m_score;
    /* 0x30 */ s32 m_heroPoints; /* meaning inferred */
    /* 0x34 */ s32 m_darkPoints; /* meaning inferred */
    /* 0x38 */ u8 field_0x38[0x44 - 0x38];
    /* 0x44 */ float m_meter;
    /* 0x48 */ float m_meterMax;
};

/* The font archive of the number displays (.sbss pointer; meaning inferred). */
extern void *lbl_805EE68C;

extern "C" {
/* Number displays (behaviour inferred): loading the font texture name from archive, the glyph
 * layout, the colours, the spacing between digits, and drawing text at (x, y). */
bool fn_800BCEA8(TGiNumberDisplay *display, void *archive, const char *name);
void fn_800BCC54(TGiNumberDisplay *display, int width, int height, int cellWidth, int cellHeight,
                 int columns);
void fn_800BCB7C(TGiNumberDisplay *display, const TGiColorSet *colors);
void fn_800BCB2C(TGiNumberDisplay *display, float spacing);
void fn_800BC7BC(TGiNumberDisplay *display, const char *text, float x, int arg3, float y);

/* The resource manager's language-specific path of a file (behaviour inferred, see Gong.cpp). */
void *fn_8003F8D4(void);
const char *fn_800423B8(void *arg0, const char *name, int arg2);
/* Loads file into resource (behaviour inferred). */
void fn_8036E114(Chao::CSD::CProject *resource, const char *file);

/* Projects (behaviour inferred): advancing, drawing, a scene by name, and fn_8036D598, whose
 * result is dropped. */
void fn_8036DAD4(Chao::CSD::CProject *project);
void fn_8036D94C(Chao::CSD::CProject *project);
Chao::CSD::RCPtr<Chao::CSD::CScene> fn_8036D660(Chao::CSD::CProject *project, const char *name);
UnkCsdHandle_80042DF0 fn_8036D598(Chao::CSD::CProject *project, int arg1);

/* Scenes (behaviour inferred): the motion, its end frame (two variants), the motion frame, the
 * current frame, the speed, playing, two words of the scene, hiding, and a node by name. */
void fn_80367238(Chao::CSD::CScene *scene, const char *motion);
float fn_803644A0(Chao::CSD::CScene *scene, const char *motion);
float fn_80364430(Chao::CSD::CScene *scene, const char *motion);
void fn_80366740(Chao::CSD::CScene *scene, float frame);
float fn_80367138(Chao::CSD::CScene *scene);
void fn_80367140(Chao::CSD::CScene *scene, float speed);
void fn_803659E4(Chao::CSD::CScene *scene);
void fn_803659DC(Chao::CSD::CScene *scene, int arg1);
void fn_803659CC(Chao::CSD::CScene *scene, int arg1);
void fn_80365B40(Chao::CSD::CScene *scene, int hidden);
Chao::CSD::RCPtr<Chao::CSD::CNode> fn_803666B4(Chao::CSD::CScene *scene, const char *name);
/* Nodes (behaviour inferred): the pattern. */
void fn_8036AD14(Chao::CSD::CNode *node, int pattern);

/* The save data and a player's record (behaviour inferred). */
void *fn_80079808(void);
UnkPlayerInfo_80178B5C *fn_80178B5C(void *save, int player);
}

#endif
