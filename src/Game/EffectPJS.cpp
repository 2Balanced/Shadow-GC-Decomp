/* Effect::PJS::Launcher and Effect::PJS::Loader (class names from RTTI): the PJS effect launcher.
 * fn_80304F40 adds a PJS effect file to a resource slot and returns its Launcher, which a Loader task
 * (task layer 3) fills in once the slot has loaded. The fn_8030536C..fn_80305900 functions launch an
 * effect through the effect manager and set it up. The unit's file name and all function and member
 * names other than the classes' are inferred; the launch functions keep their address names.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first). */
#define BOOST_SP_DELETER_CONSTRUCTOR

#include "Effect/System/EffectElement.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/ResourceOneFile.h"
#include "Game/Task.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;

/* The effect manager fn_800A31F0 returns (class name unknown): its second virtual function
 * launches effect id (the meaning of arg is unknown). */
class UnkEffectManager_800A31F0 {
  public:
    virtual void vf08();
    virtual boost::weak_ptr<EffectElement> Launch(s32 id, s32 arg);
};

extern "C" {
UnkEffectManager_800A31F0 *fn_800A31F0(void);

/* Sets an effect's position, rotation and matrix (behaviour inferred; see Game/BulletLaser.cpp and
 * Game/Fan.cpp). */
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
void fn_80425D94(EffectElement *effect, const Vec3f *rotation);
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);

/* A direction's rotation, and that rotation's angles (behaviour inferred; see
 * Game/BulletLaser.cpp). */
Vec3f fn_80016284(const Vec3f &direction);
Vec2f fn_8040F664(const Vec3f &rotation);

/* The resource loader: adds a resource and returns its slot, the slot's state (3 when loaded) and
 * the loaded data of a slot (behaviour inferred; see Game/ResourceOneFile.h). */
s32 fn_800128D8(void *loader, ResourceOneFile *resource);
s32 fn_80012408(void *loader, s32 slot);
void *fn_8001277C(void *loader, s32 slot, int arg2);

/* The task tree and its layer n (see Game/Gadget.h). */
Task *fn_8001C4F8(void);
Task *fn_801EB304(Task *root, int layer);
}

extern "C" {

/* Launches effect id following matrix (fn_80305900). */
boost::weak_ptr<EffectElement> fn_80305900(s32 id, const Mtx34f *matrix, s32 arg,
                                           u32 owner) {
    boost::weak_ptr<EffectElement> effect;
    effect = fn_800A31F0()->Launch(id, arg);
    boost::shared_ptr<EffectElement> element(effect.lock());
    if (element.px != 0) {
        element->SetMatrix(matrix);
        element->ClearFlag4();
        if (owner != 0) {
            element->SetField14(owner);
        }
    }
    return effect;
}

/* Launches effect id with the values at 0x4C and 0x50 (fn_803057EC). */
boost::weak_ptr<EffectElement> fn_803057EC(s32 id, u32 value4C, u32 value50, s32 arg,
                                           u32 owner) {
    boost::weak_ptr<EffectElement> effect;
    effect = fn_800A31F0()->Launch(id, arg);
    boost::shared_ptr<EffectElement> element(effect.lock());
    if (element.px != 0) {
        element->SetField4C(value4C);
        element->SetField50(value50);
        element->ClearFlag4();
        if (owner != 0) {
            element->SetField14(owner);
        }
    }
    return effect;
}

/* Launches effect id (fn_803056F8). */
boost::weak_ptr<EffectElement> fn_803056F8(s32 id, s32 arg, u32 owner) {
    boost::weak_ptr<EffectElement> effect;
    effect = fn_800A31F0()->Launch(id, arg);
    boost::shared_ptr<EffectElement> element(effect.lock());
    if (element.px != 0) {
        element->ClearFlag4();
        if (owner != 0) {
            element->SetField14(owner);
        }
    }
    return effect;
}

/* Launches effect id placed by matrix (fn_803055F8). */
boost::weak_ptr<EffectElement> fn_803055F8(s32 id, const Mtx34f *matrix, s32 arg,
                                           u32 owner) {
    boost::weak_ptr<EffectElement> effect;
    effect = fn_800A31F0()->Launch(id, arg);
    boost::shared_ptr<EffectElement> element(effect.lock());
    if (element.px != 0) {
        fn_80425E68(element.px, matrix);
        element->ClearFlag4();
        if (owner != 0) {
            element->SetField14(owner);
        }
    }
    return effect;
}

/* Launches effect id at position, turned by angles (fn_803054D0). */
boost::weak_ptr<EffectElement> fn_803054D0(s32 id, const Vec3f *position,
                                           const Vec2f &angles, s32 arg, u32 owner) {
    boost::weak_ptr<EffectElement> effect;
    effect = fn_800A31F0()->Launch(id, arg);
    boost::shared_ptr<EffectElement> element(effect.lock());
    if (element.px != 0) {
        fn_80425E4C(element.px, position);
        Vec3f rotation(angles.x, angles.y, 0.0f);
        fn_80425D94(element.px, &rotation);
        element->ClearFlag4();
        if (owner != 0) {
            element->SetField14(owner);
        }
    }
    return effect;
}

/* Launches effect id at position, facing direction (fn_8030536C). */
boost::weak_ptr<EffectElement> fn_8030536C(s32 id, const Vec3f *position,
                                           const Vec3f &direction, s32 arg, u32 owner) {
    boost::weak_ptr<EffectElement> effect;
    effect = fn_800A31F0()->Launch(id, arg);
    boost::shared_ptr<EffectElement> element(effect.lock());
    if (element.px != 0) {
        Vec2f angles = fn_8040F664(fn_80016284(direction));
        fn_80425E4C(element.px, position);
        fn_80425D94(element.px, &Vec3f(angles.x, angles.y, 0.0f));
        element->ClearFlag4();
        if (owner != 0) {
            element->SetField14(owner);
        }
    }
    return effect;
}
}

namespace Effect {
namespace PJS {

Launcher::Launcher(s32 slot, ResourceOneFile *file) : EffectLaunchInfoUSA(0, file) {
    m_slot = slot;
    field_0x10 = 0;
}

Launcher::~Launcher() {}

bool Launcher::IsLoaded() const {
    return m_data != 0;
}

/* Releases the launcher's resource slot and deletes it: the shared_ptr's deleter (name
 * inferred). */
void DeleteLauncher(Launcher *launcher) {
    fn_800127F8(fn_800122D4(), launcher->m_slot);
    delete launcher;
}

/* Loader (class name from RTTI): the task that waits for a launcher's resource slot to load. */
class Loader : public Task {
  public:
    Loader(const boost::shared_ptr<Launcher> &launcher);
    virtual ~Loader();
    virtual void Tick(float dt);

    /* 0x28 */ boost::shared_ptr<Launcher> m_launcher;
};
SIZE_ASSERT(Loader, 0x30);

Loader::Loader(const boost::shared_ptr<Launcher> &launcher)
    : Task(fn_801EB304(fn_8001C4F8(), 3)), m_launcher(launcher) {
    m_name = "Launcher Loader";
}

Loader::~Loader() {}

void Loader::Tick(float dt) {
    void *loader = fn_800122D4();
    if (fn_80012408(loader, m_launcher->m_slot) == 3) {
        m_launcher->m_data = fn_8001277C(loader, m_launcher->m_slot, 2);
        m_flags |= 1;
    }
}

} // namespace PJS
} // namespace Effect

/* Adds PJS effect file fileName to a resource slot and returns its launcher (name of the function
 * unknown). */
extern "C" boost::shared_ptr<Effect::PJS::Launcher> fn_80304F40(const char *fileName) {
    void *loader = fn_800122D4();
    ResourceOneFile *file = new ResourceOneFile(fileName, false);
    s32 slot = fn_800128D8(loader, file);
    boost::shared_ptr<Effect::PJS::Launcher> launcher(new Effect::PJS::Launcher(slot, file),
                                                       Effect::PJS::DeleteLauncher);
    new Effect::PJS::Loader(launcher);
    return launcher;
}
