/* Effect::System::EffectManager and Effect::System::EffectElement (class names from RTTI): the
 * portable core of Sonic Team's effect library. The manager keeps every running element in two
 * lists of shared pointers (one in launch order for updating, one for drawing), launches elements
 * from the descriptions in an effect file ("EFFD") and relocates that file on first use. The
 * element base class keeps the element's place in both lists and builds its world matrix. File
 * name from the class; member and function names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 24 of 25 functions match. Relocate (98%) loads the "EFFD" address into another
 * register (scheduling). The unit cannot link as written either: retail has the local shared
 * pointer template instances (operator==, the shared_ptr/weak_ptr/shared_count constructors,
 * checked_deleter) at the end of the unit, while GC/2.7 emits each after its first user (their
 * extab entries are in first-use order in retail too, so the original compiler only placed the
 * code differently). */
#define EFFECT_ELEMENT_POLYMORPHIC
#define EFFECT_ELEMENT_FULL
#define EFFECT_MANAGER_FULL

#include "Effect/System/EffectDesc.h"
#include "Effect/System/EffectElement.h"
#include "Effect/System/EffectManager.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

extern "C" {
/* Compares the first n characters of two strings (behaviour inferred). */
int fn_803AC8DC(const char *a, const char *b, u32 n);
/* out = v * s (behaviour inferred). */
void fn_80093230(Vec3f *out, const Vec3f *v, float s);
void fn_8040CDA8(Vec3f *out, const Vec3f *v, float s);
/* v += add (behaviour inferred). */
void fn_8040CFF8(Vec3f *v, const Vec3f *add);
/* Applies a scale (behaviour inferred). */
void fn_8040B790(Mtx34f *m, const Vec3f *scale);

void fn_80425D94(Effect::System::EffectElement *element, const Vec3f *rotation);
void fn_80425E4C(Effect::System::EffectElement *element, const Vec3f *position);
void fn_80425E68(Effect::System::EffectElement *element, const Mtx34f *matrix);

/* Empties a shared pointer to an element (boost's shared_ptr::reset; see Game/BKTank.cpp). */
void fn_800A4210(boost::shared_ptr<Effect::System::EffectElement> *element);
}

/* Adds base to an offset that is not 0, which makes it a pointer. */
inline void RelocateWord(void *object, u32 offset, void *base) {
    u32 *word = reinterpret_cast<u32 *>(static_cast<u8 *>(object) + offset);
    if (*word != 0) {
        *word += reinterpret_cast<u32>(base);
    }
}

/* Sets the three components of v. */
inline void SetVector(Vec3f &v, float x, float y, float z) {
    v.x = x;
    v.y = y;
    v.z = z;
}

using Effect::System::EffectElement;
using Effect::System::EffectLaunchInfo;
using Effect::System::EffectManager;
using Effect::System::UnkEffectData_80425860;
using Effect::System::UnkEffectDesc_801D0AFC;

namespace Effect {
namespace System {

EffectElement::EffectElement(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                             EffectManager *manager)
    : m_loopDepth(0), m_skipLoops(0), m_size(sizeof(EffectElement)), m_time(0.0f),
      m_speed(1.0f), m_speedSource(0), m_flags(1), m_manager(manager), m_drawPass(0),
      m_desc(desc), m_matrix(0), m_position(0), m_rotation(0) {
    fn_8040C148(&m_worldMatrix, &lbl_805E0720);
    fn_8040C148(&m_localMatrix, &lbl_805E0720);
    m_lastPosition.x = 0.0f;
    m_lastPosition.y = 0.0f;
    m_lastPosition.z = 0.0f;
    m_velocity.x = 0.0f;
    m_velocity.y = 0.0f;
    m_velocity.z = 0.0f;
    m_offset.x = 0.0f;
    m_offset.y = 0.0f;
    m_offset.z = 0.0f;
    m_angles.x = 0.0f;
    m_angles.y = 0.0f;
    m_angles.z = 0.0f;
    m_scale.x = 1.0f;
    m_scale.y = 1.0f;
    m_scale.z = 1.0f;
    m_offsetSpeed.x = 0.0f;
    m_offsetSpeed.y = 0.0f;
    m_offsetSpeed.z = 0.0f;
}

} // namespace System
} // namespace Effect

namespace Effect {
namespace System {

EffectElement::~EffectElement() {
    fn_800A4210(&m_next);
    fn_800A4210(&m_prev);
    fn_800A4210(&m_drawNext);
    fn_800A4210(&m_drawPrev);
}

} // namespace System
} // namespace Effect

extern "C" {

/* Makes element follow matrix once (name inferred). */
void fn_80425E68(EffectElement *element, const Mtx34f *matrix) {
    fn_8040C148(&element->m_localMatrix, matrix);
}
}

extern "C" {

/* Moves element to position (name inferred). */
void fn_80425E4C(EffectElement *element, const Vec3f *position) {
    element->m_localMatrix.values[0][3] = position->x;
    element->m_localMatrix.values[1][3] = position->y;
    element->m_localMatrix.values[2][3] = position->z;
}
}

extern "C" {

/* Turns element to rotation, keeping its position (name inferred). */
void fn_80425D94(EffectElement *element, const Vec3f *rotation) {
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    float x;
    float y;
    float z;
    z = element->m_localMatrix.values[2][3];
    y = element->m_localMatrix.values[1][3];
    x = element->m_localMatrix.values[0][3];
    fn_8040B80C(&matrix, rotation->z);
    fn_8040B848(&matrix, rotation->y);
    fn_8040B884(&matrix, rotation->x);
    matrix.values[0][3] = x;
    matrix.values[1][3] = y;
    matrix.values[2][3] = z;
    fn_8040C148(&element->m_localMatrix, &matrix);
}
}

namespace Effect {
namespace System {

void EffectElement::UpdateTransform(float dt) {
    if (m_matrix != 0) {
        fn_80425E68(this, m_matrix);
    } else {
        const Vec3f *position = m_position;
        if (position != 0) {
            m_localMatrix.values[0][3] = position->x;
            m_localMatrix.values[1][3] = position->y;
            m_localMatrix.values[2][3] = position->z;
        }
        if (m_rotation != 0) {
            fn_80425D94(this, m_rotation);
        }
    }
    fn_8040C148(&m_worldMatrix, &m_localMatrix);
    Vec3f position;
    Vec3f delta;
    Vec3f step;
    Vec3f difference;
    Vec3f velocity;
    fn_80093230(&step, &m_offsetSpeed, dt);
    fn_8040CFF8(&m_offset, &step);
    fn_8040B59C(&m_worldMatrix, &m_offset);
    fn_8040B80C(&m_worldMatrix, m_angles.z);
    fn_8040B848(&m_worldMatrix, m_angles.y);
    fn_8040B884(&m_worldMatrix, m_angles.x);
    fn_8040B790(&m_worldMatrix, &m_scale);
    SetVector(position, m_worldMatrix.values[0][3], m_worldMatrix.values[1][3],
              m_worldMatrix.values[2][3]);
    if (m_flags & 2) {
        fn_8040CD74(&difference, &position, &m_lastPosition);
        fn_800091D0(&delta, &difference);
        fn_8040CDA8(&velocity, &delta, 1.0f / dt);
        fn_800091D0(&m_velocity, &velocity);
    }
    fn_800091D0(&m_lastPosition, &position);
}

} // namespace System
} // namespace Effect

namespace Effect {
namespace System {

EffectManager::EffectManager() : m_drawHidden(false), m_paused(false), m_error(0) {}

EffectManager::~EffectManager() {}

bool EffectManager::Relocate(UnkEffectData_80425860 *data) {
    if (data == 0) {
        return false;
    }
    if (fn_803AC8DC(data->m_magic, "EFFD", 4) != 0) {
        m_error = 2;
        return false;
    }
    if (data->m_version < 1) {
        m_error = 3;
        return false;
    }
    if (data->m_relocated != 0) {
        return true;
    }
    RelocateWord(data, 0x2C, data);
    for (UnkEffectDesc_801D0AFC *desc = data->m_descs; desc != 0; desc = desc->m_next) {
        switch (desc->m_type) {
        case 2:
            RelocateWord(desc, 0x48, data);
            RelocateWord(desc, 0x58, data);
            RelocateWord(desc, 0x70, data);
            RelocateWord(desc, 0x68, data);
            break;
        case 3:
            RelocateWord(desc, 0x38, data);
            break;
        case 4:
            RelocateWord(desc, 0x3C, data);
            RelocateWord(desc, 0x40, data);
            break;
        case 5:
            RelocateWord(desc, 0x48, data);
            RelocateWord(desc, 0x58, data);
            RelocateWord(desc, 0x5C, data);
            RelocateWord(desc, 0x74, data);
            RelocateWord(desc, 0x6C, data);
            break;
        case 6:
            RelocateWord(desc, 0x54, data);
            RelocateWord(desc, 0x6C, data);
            RelocateWord(desc, 0x64, data);
            break;
        case 7:
            RelocateWord(desc, 0x50, data);
            RelocateWord(desc, 0x54, data);
            break;
        case 8:
            RelocateWord(desc, 0x34, data);
            break;
        case 9:
            RelocateWord(desc, 0x44, data);
            RelocateWord(desc, 0x48, data);
            RelocateWord(desc, 0x4C, data);
            break;
        case 10:
            RelocateWord(desc, 0x38, data);
            RelocateWord(desc, 0x3C, data);
            break;
        }
        RelocateWord(desc, 0x28, data);
    }
    data->m_relocated = 1;
    return true;
}

} // namespace System
} // namespace Effect

namespace Effect {
namespace System {

EffectElement *EffectManager::CreateElement(const UnkEffectDesc_801D0AFC *desc,
                                            EffectLaunchInfo *info) {
    return 0;
}

} // namespace System
} // namespace Effect

namespace Effect {
namespace System {

boost::weak_ptr<EffectElement> EffectManager::Launch(EffectLaunchInfo *info, int id) {
    boost::weak_ptr<EffectElement> none;
    m_error = 0;
    if (info == 0) {
        m_error = 1;
        return none;
    }
    if (!Relocate(static_cast<UnkEffectData_80425860 *>(info->m_data))) {
        return none;
    }
    UnkEffectDesc_801D0AFC *desc;
    for (desc = static_cast<UnkEffectData_80425860 *>(info->m_data)->m_descs; desc != 0;
         desc = desc->m_next) {
        if (desc->m_id == id) {
            break;
        }
    }
    if (desc == 0) {
        m_error = 4;
        return none;
    }
    boost::shared_ptr<EffectElement> element(CreateElement(desc, info));
    if (!element || !element->IsActive()) {
        m_error = 5;
        return none;
    }
    if (!m_tail) {
        m_head = m_tail = element;
    } else {
        m_tail->m_next = element;
        element->m_prev = m_tail;
        m_tail = element;
    }
    if (!m_drawTail) {
        m_drawHead = m_drawTail = element;
    } else {
        m_drawTail->m_drawNext = element;
        element->m_drawPrev = m_drawTail;
        m_drawTail = element;
    }
    return element;
}

} // namespace System
} // namespace Effect

namespace Effect {
namespace System {

boost::weak_ptr<EffectElement> EffectManager::Launch(
    const boost::shared_ptr<EffectLaunchInfo> &info, int id) {
    return Launch(info.get(), id);
}

} // namespace System
} // namespace Effect

extern "C" {

/* Unlinks all of the manager's elements (name inferred). */
void fn_8042554C(EffectManager *manager) {
    boost::shared_ptr<EffectElement> element;
    boost::shared_ptr<EffectElement> next;
    element = manager->m_head;
    while (element) {
        next = element->m_next;
        manager->Remove(element);
        element = next;
    }
}
}

namespace Effect {
namespace System {

void EffectManager::Remove(boost::shared_ptr<EffectElement> element) {
    if (element->m_next) {
        element->m_next->m_prev = element->m_prev;
    }
    if (element->m_prev) {
        element->m_prev->m_next = element->m_next;
    }
    if (element == m_head) {
        m_head = element->m_next;
    }
    if (element == m_tail) {
        m_tail = element->m_prev;
    }
    if (element->m_drawNext) {
        element->m_drawNext->m_drawPrev = element->m_drawPrev;
    }
    if (element->m_drawPrev) {
        element->m_drawPrev->m_drawNext = element->m_drawNext;
    }
    if (element == m_drawHead) {
        m_drawHead = element->m_drawNext;
    }
    if (element == m_drawTail) {
        m_drawTail = element->m_drawPrev;
    }
}

} // namespace System
} // namespace Effect

extern "C" {

/* Drops the manager's finished elements and updates the others (name inferred). */
void fn_80425294(EffectManager *manager, float dt) {
    boost::shared_ptr<EffectElement> element(manager->m_head);
    boost::shared_ptr<EffectElement> next;
    while (element) {
        next = element->m_next;
        if (!element->IsActive()) {
            manager->Remove(element);
        }
        element = next;
    }
    if (manager->m_paused) {
        return;
    }
    element = manager->m_head;
    while (element) {
        if (element->m_speedSource != 0) {
            element->m_speed = *element->m_speedSource;
        }
        element->Update(dt * element->m_speed);
        element->m_flags |= 2;
        element = element->m_next;
    }
}
}

extern "C" {

/* Draws the manager's elements of draw pass pass (name inferred). */
void fn_80425170(EffectManager *manager, int pass) {
    boost::shared_ptr<EffectElement> element(manager->m_drawHead);
    if (manager->m_drawHidden) {
        while (element) {
            if (pass == element->m_drawPass) {
                if (!element->IsHidden()) {
                    element->Hide();
                    element->Draw();
                    element->Show();
                } else {
                    element->Draw();
                }
            }
            element = element->m_drawNext;
        }
    } else {
        while (element) {
            if (pass == element->m_drawPass) {
                element->Draw();
            }
            element = element->m_drawNext;
        }
    }
}
}
