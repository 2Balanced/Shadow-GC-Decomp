/* MaterialBlink (file and class names from RTTI: @unnamed@MaterialBlink_cpp@::MaterialBlinkTask):
 * a task that makes a model's material flash in a colour for a while, as the bosses do when they
 * are hit. While it runs it writes the colour into the material's override colour on the frames
 * the game state's blink bit (bit 0 of the word at 0x88) is set and clears it ((-1, -1, -1), no
 * override) on the others; when the time is up it ends itself and clears the colour. Member and
 * function names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "size_assert.h"

/* The game state (struct name unknown; see Game/CenFile.cpp): a frame counter whose bit 0 the
 * blink follows (meaning inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x88];
    /* 0x088 */ u32 m_blinkFrame;
    /* 0x08C */ u8 field_0x8C[0x150 - 0x8C];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* Set while the game is paused (see Game/CameraManagerUnit.cpp; meaning inferred). */
extern u8 lbl_805EE1DC;

/* Whether the colour shows this frame (name inferred). */
inline bool IsBlinkOn() {
    bool on = true;
    if (lbl_805EE1DC || !(lbl_8057D6A0.m_blinkFrame & 1)) {
        on = false;
    }
    return on;
}

/* Sets a colour (name inferred). */
inline void SetColor(Vec3f *color, float r, float g, float b) {
    color->x = r;
    color->y = g;
    color->z = b;
}

namespace {

class MaterialBlinkTask : public Task {
  public:
    MaterialBlinkTask(Task *parent, Vec3f *color, float r, float g, float b, float time);
    virtual ~MaterialBlinkTask();
    virtual void Tick(float dt);

    void Clear();
    void Apply();

    /* 0x28 */ Vec3f *m_target; /* the material's override colour */
    /* 0x2C */ Vec3f m_color;
    /* 0x38 */ float m_time; /* how long the blink still lasts */
    /* 0x3C */ u32 field_0x3C;
};
SIZE_ASSERT(MaterialBlinkTask, 0x40);

MaterialBlinkTask::MaterialBlinkTask(Task *parent, Vec3f *color, float r, float g, float b, float time)
    : Task(parent) {
    m_target = color;
    m_name = "MaterialBlink";
    m_color.x = r;
    m_color.y = g;
    m_color.z = b;
    m_time = time;
    m_flags |= 0x100;
}

MaterialBlinkTask::~MaterialBlinkTask() {
    Clear();
}

void MaterialBlinkTask::Tick(float dt) {
    if (IsBlinkOn()) {
        Apply();
    } else {
        Clear();
    }
    if (!lbl_805EE1DC) {
        m_time -= dt;
        if (m_time <= 0.0f) {
            m_flags |= 1;
        }
    }
}

void MaterialBlinkTask::Clear() {
    SetColor(m_target, -1.0f, -1.0f, -1.0f);
}

void MaterialBlinkTask::Apply() {
    SetColor(m_target, m_color.x, m_color.y, m_color.z);
}

} // namespace

/* Starts a blink of the material colour color in (r, g, b) for time seconds under parent (name
 * inferred). */
extern "C" Task *fn_80336888(Task *parent, Vec3f *color, float r, float g, float b, float time) {
    return new MaterialBlinkTask(parent, color, r, g, b, time);
}
