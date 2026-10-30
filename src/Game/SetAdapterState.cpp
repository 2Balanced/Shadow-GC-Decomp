/* SetAdapterState (class name from RTTI; its destructor and vtable are here), the functions of the
 * gadgets' placement owner (UnkPlacement_800C9CD0: the placement record, the task spawned from it,
 * its adapter state and character) and the out-of-line code of MotionSequenceModule. File name
 * from the class; member and function names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define MOTION_SEQUENCE_MODULE_OUT_OF_LINE
#define SET_ADAPTER_STATE_DTOR_OUT_OF_LINE
#define UNKPLACEMENT_800C9CD0_MEMBERS
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/MotionSequenceModule.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "Game/SetAdapterState.h"
#include <string.h>

class UnkObject_800C9B84;

extern "C" {
/* The motion functions (see Game/MotionSequenceModule.h). */
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int arg1, float arg2);
void fn_80416198(_stMtnPlayWork *work, int arg1);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
void fn_80416ED0(_stMtnPlayWork *work, int arg1, float frame);
void fn_80416E78(_stMtnPlayWork *work, int arg1, float frame);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);

/* A 32-bit flag word (see Game/BASwitch.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Whether a sphere around position is in view (behaviour inferred). */
bool fn_80169BE4(const Vec3f *position, float radiusSquared);
/* Frees a block of memory (behaviour inferred). */
void fn_80049B34(void *data);

/* The set manager (the next unit's class; constructor and destructor by their addresses), and
 * telling it a switch's object was removed (behaviour inferred). */
void *fn_800CBA08(void *object);
void fn_800CB9A0(void *object, int flags);
void fn_800CB534(UnkObject_800C9B84 *manager, u8 id, UnkPlacement_800C9CD0 *owner);
void *__register_global_object(void *object, void *destructor, void *record);
}

/* The gadget types (Game/GadgetModelInfo.cpp). */
extern GadgetClassDesc *lbl_8052B080[];

/* The game sets these in __sinit rather than in initial values, as an initializer that is not a
 * constant expression does; the inline call reproduces that (as in Game/CameraModeFixLook2.cpp). */
inline float DynamicFloat(float value) {
    return value;
}

/* A 32-bit flag word (struct name unknown; see Game/BASwitch.cpp). */
struct UnkFlags_80014E04 {
    UnkFlags_80014E04() {
        fn_80014E04(&m_bits);
    }

    u32 m_bits;
};

/* How far beyond its range a gadget stays in view (name inferred), and the gadgets' global flags
 * (bit 0: placements are being shown; meaning inferred). */
static float s_viewMargin = DynamicFloat(50.0f);
static UnkFlags_80014E04 s_flags;

/* The set manager (a function-local static object). */
static s8 s_setManagerInitialized;
static u32 s_setManagerObject[2];
static UnkObject_800C9B84 *s_setManager;
static u32 s_setManagerRecord[3];

extern "C" {

void fn_800C9F14(UnkPlacement_800C9CD0 *owner, Vec3f *position) {
    position->x = owner->m_record->m_position.x;
    position->y = owner->m_record->m_position.y;
    position->z = owner->m_record->m_position.z;
}

void fn_800C9ED4(UnkPlacement_800C9CD0 *owner, Vec3f *rotation) {
    float scale = lbl_805FAD38 / 180.0f;
    rotation->x = owner->m_record->m_rotation.x * scale;
    rotation->y = owner->m_record->m_rotation.y * scale;
    rotation->z = owner->m_record->m_rotation.z * scale;
}

void fn_800C9EC4(UnkPlacement_800C9CD0 *owner, u32 *flags) {
    *flags = owner->m_record->m_flags;
}

void fn_800C9EB0(UnkPlacement_800C9CD0 *owner, u32 flags) {
    owner->m_record->m_flags |= flags;
}

void fn_800C9E9C(UnkPlacement_800C9CD0 *owner, u32 flags) {
    owner->m_record->m_flags &= ~flags;
}

bool fn_800C9E80(UnkPlacement_800C9CD0 *owner, u32 flags) {
    return (owner->m_record->m_flags & flags) == flags;
}

void fn_800C9E70(UnkPlacement_800C9CD0 *owner, u32 *value) {
    *value = owner->m_record->field_0x1C;
}

void fn_800C9E60(UnkPlacement_800C9CD0 *owner, u8 *id) {
    *id = owner->m_record->m_switch;
}

void fn_800C9E50(UnkPlacement_800C9CD0 *owner, u32 *id) {
    *id = owner->m_record->m_switch;
}

void fn_800C9E18(UnkPlacement_800C9CD0 *owner, float *range) {
    *range = 100.0f * owner->m_record->m_range;
}

void fn_800C9DBC(UnkPlacement_800C9CD0 *owner, void *params, int size) {
    if (size == owner->m_record->m_paramSize) {
        owner->m_record->m_flags &= ~0x20000000;
        memcpy(params, owner->m_record->m_params, size);
    } else {
        owner->m_record->m_flags |= 0x20000000;
    }
}

void fn_800C9D68(UnkPlacement_800C9CD0 *owner, const void *params, int size) {
    if (size == owner->m_record->m_paramSize) {
        owner->m_record->m_flags &= ~0x20000000;
        memcpy(owner->m_record->m_params, params, size);
    } else {
        owner->m_record->m_flags |= 0x20000000;
    }
}

void *fn_800C9D30(UnkPlacement_800C9CD0 *owner) {
    void *params;
    if (lbl_8052B080[owner->m_type]->paramTable != 0 && (params = owner->m_record->m_params) != 0) {
        return params;
    }
    return 0;
}

u16 fn_800C9D24(UnkPlacement_800C9CD0 *owner) {
    return owner->m_record->field_0x20;
}

bool fn_800C9D14(UnkPlacement_800C9CD0 *owner) {
    return (owner->m_record->m_flags >> 8) & 1;
}

bool fn_800C9D0C(UnkPlacement_800C9CD0 *owner) {
    return false;
}

void fn_800C9CF8(UnkPlacement_800C9CD0 *owner) {
    owner->m_record->m_flags &= ~0x100;
}

void fn_800C9CD0(UnkPlacement_800C9CD0 *owner, Task *task) {
    owner->m_record->m_flags |= 2;
    owner->m_record->m_flags &= ~4;
    owner->m_task = task;
}

void fn_800C9BE8(UnkPlacement_800C9CD0 *owner) {
    owner->m_record->m_flags &= ~2;
    owner->m_record->m_flags &= ~4;
    if (owner->m_record->m_flags & 0x40000000) {
        if (owner->m_adapterState) {
            delete owner->m_adapterState;
            owner->m_adapterState = 0;
        }
        if (owner->m_record->m_params) {
            if (owner->m_record->field_0x1C & 0x200000) {
                fn_80049B34(owner->m_record->m_params);
                owner->m_record->field_0x1C &= ~0x200000;
            }
            owner->m_record->m_paramSize = 0;
            owner->m_record->m_params = 0;
        }
        owner->m_record->m_flags &= ~0x40000000;
    }
    owner->m_task = 0;
    owner->m_chara.reset();
}

UnkObject_800C9B84 *fn_800C9B84(void) {
    if (s_setManager == 0) {
        if (!s_setManagerInitialized) {
            __register_global_object(fn_800CBA08(s_setManagerObject), fn_800CB9A0, s_setManagerRecord);
            s_setManagerInitialized = 1;
        }
        s_setManager = (UnkObject_800C9B84 *)s_setManagerObject;
    }
    return s_setManager;
}

void fn_800C9B34(UnkPlacement_800C9CD0 *owner) {
    owner->m_record->m_flags &= ~1;
    fn_800C9BE8(owner);
    fn_800CB534(fn_800C9B84(), owner->m_record->m_switch, owner);
}

void fn_800C9B04(UnkPlacement_800C9CD0 *owner) {
    owner->m_record->m_flags |= 8;
    fn_800C9B34(owner);
}

void fn_800C9AD4(UnkPlacement_800C9CD0 *owner) {
    owner->m_record->m_flags |= 0x10000;
    fn_800C9B34(owner);
}

bool fn_800C9A64(UnkPlacement_800C9CD0 *owner) {
    if (fn_80014E34(&s_flags.m_bits, 0) && !(owner->m_record->m_flags & 0x8000)) {
        return true;
    }
    if (owner->m_record->m_flags & 4) {
        return true;
    }
    return false;
}

void fn_800C99EC(UnkPlacement_800C9CD0 *owner) {
    owner->m_record = 0;
    owner->field_0x4 = 0;
    owner->field_0x8 = 0;
    owner->m_task = 0;
    owner->m_type = -1;
    owner->m_chara.reset();
    if (owner->m_adapterState) {
        delete owner->m_adapterState;
        owner->m_adapterState = 0;
    }
}

bool fn_800C996C(UnkPlacement_800C9CD0 *owner) {
    UnkPlacementRecord_800C9CD0 *record = owner->m_record;
    if (record->m_flags & 0x800) {
        return false;
    }
    Vec3f position(record->m_position.x, record->m_position.y, record->m_position.z);
    float radius = 100.0f * record->m_range + s_viewMargin;
    radius *= radius;
    return fn_80169BE4(&position, radius);
}

float fn_800C9938(UnkPlacement_800C9CD0 *owner) {
    return 100.0f * owner->m_record->m_range + 100.0f;
}

void fn_800C9930(UnkPlacement_800C9CD0 *owner, SetAdapterState *state) {
    owner->m_adapterState = state;
}

} /* extern "C" */

SetAdapterState::~SetAdapterState() {}

extern "C" {

void fn_800C9894(UnkPlacement_800C9CD0 *owner) {
    if (owner->m_adapterState) {
        delete owner->m_adapterState;
        owner->m_adapterState = 0;
    }
}

bool fn_800C9870(UnkPlacement_800C9CD0 *owner) {
    u32 flags = owner->m_record->m_flags;
    if (flags & 0x10000) {
        return false;
    }
    return (flags & 1) ? false : true;
}

void fn_800C985C(UnkPlacement_800C9CD0 *owner) {
    owner->m_record->m_flags |= 8;
}

void fn_800C97F8(UnkPlacement_800C9CD0 *owner, const boost::shared_ptr<CharInfo> &chara) {
    owner->m_chara = chara;
}

void fn_800C973C(UnkPlacement_800C9CD0 *owner, CharCommand *command) {
    if (command) {
        if (boost::shared_ptr<CharInfo> chara = owner->m_chara.lock()) {
            chara->PushCommand(command);
            return;
        }
        delete command;
    }
}

void fn_800C9718(void) {
    fn_80014E10(&s_flags.m_bits);
}

void fn_800C96F4(void) {
    fn_80014E10(&s_flags.m_bits);
}

void fn_800C96CC(void) {
    fn_80014E1C(&s_flags.m_bits, 0);
}

void fn_800C96A4(void) {
    fn_8006127C(&s_flags.m_bits, 0);
}

void fn_800C9690(UnkPlacement_800C9CD0 *owner) {
    owner->m_record->m_flags |= 0x20000000;
}

bool fn_800C961C(MotionSequenceModule *module, const MotionSequence *sequence) {
    bool started = false;
    if (sequence && sequence->steps) {
        module->m_previous = module->m_sequence;
        module->m_sequence = sequence;
        module->m_stepCount = sequence->count;
        module->m_started = 1;
        module->m_next = sequence->next;
        started = true;
    }
    module->m_motion = -1;
    module->m_step = 0;
    module->m_time = 0.0f;
    module->m_stepTime = 0.0f;
    module->field_0x30 = 0;
    module->field_0x1E = 0;
    return started;
}

void fn_800C9554(MotionSequenceModule *module, _stMtnPlayWork *work) {
    if (work) {
        bool ended = false;
        if (fn_80416F28(work, 0, 3, 1) != 0 && fn_80416F28(work, 0, 5, 1) == 0) {
            ended = true;
        }
        module->m_ended = ended;
        if (module->m_ended) {
            if (module->m_looping) {
                module->field_0x30++;
            } else {
                if (module->m_step + 1 >= module->m_sequence->count) {
                    module->m_finished = true;
                }
                module->field_0x1C = 1;
            }
        }
    }
}

} /* extern "C" */

bool MotionSequenceModule::vf_0x0C() {
    return false;
}

void MotionSequenceModule::Reset() {
    fn_800C9484(this, m_defaultIndex);
}

/* Starts sequence (the body of fn_800C961C, which fn_800C9484 inlines). */
inline bool StartSequence(MotionSequenceModule *module, const MotionSequence *sequence) {
    bool started = false;
    if (sequence && sequence->steps) {
        module->m_previous = module->m_sequence;
        module->m_sequence = sequence;
        module->m_stepCount = sequence->count;
        module->m_started = 1;
        module->m_next = sequence->next;
        started = true;
    }
    module->m_motion = -1;
    module->m_step = 0;
    module->m_time = 0.0f;
    module->m_stepTime = 0.0f;
    module->field_0x30 = 0;
    module->field_0x1E = 0;
    return started;
}

extern "C" {

bool fn_800C9484(MotionSequenceModule *module, int index) {
    bool started = false;
    if (module->m_table != 0 && index < module->m_table->count) {
        module->m_index = index;
        started = StartSequence(module, &module->m_table->sequences[index]);
    }
    return started;
}

void fn_800C9178(MotionSequenceModule *module, _stMtnPlayWork *work, int base, float dt) {
    if (work) {
        bool changed = false;
        module->field_0x1E = 0;
        if (module->m_started) {
            module->m_started = 0;
            changed = true;
            module->field_0x1C = 0;
        } else if (module->m_looping) {
            s16 index = module->m_index;
            bool result = module->vf_0x0C();
            if (index != module->m_index) {
                changed = true;
                module->field_0x1C = 0;
            }
            module->field_0x1E = result;
        }
        if (module->field_0x1C || module->field_0x1E) {
            if (module->m_step + 1 >= module->m_sequence->count) {
                module->m_finished = true;
                if (module->m_next < 0) {
                    switch (module->m_next) {
                    case -3:
                        module->Reset();
                        changed = true;
                        module->field_0x1C = 0;
                        break;
                    case -2:
                        fn_800C9484(module, module->m_index);
                        changed = true;
                        module->field_0x1C = 0;
                        break;
                    }
                } else {
                    fn_800C9484(module, module->m_next);
                    changed = true;
                }
            } else {
                module->m_step = module->m_step + 1;
                changed = true;
            }
        }
        if (changed) {
            const MotionSequenceStep *step = &module->m_sequence->steps[module->m_step];
            module->m_motion = step->motion;
            if (module->m_motion < 0) {
                module->m_motion = module->m_table->sequences[module->m_defaultIndex].steps[0].motion;
            }
            if (step->blend > 0 && !module->field_0x1D) {
                fn_804165D8(work, 0, -1, -1);
                fn_80416148(work, 0, step->blend);
            } else {
                fn_80416198(work, 0);
            }
            fn_80417568(work, module->m_motion + base - 1);
            fn_80416ED0(work, 0, step->field_0x2);
            fn_80416E78(work, 0, step->field_0x4);
            if (step->flags & 3) {
                module->m_looping = 1;
                fn_80416F28(work, 0, 1, 0);
            } else {
                module->m_looping = 0;
            }
            module->field_0x30 = 0;
            module->m_stepTime = 0.0f;
        } else {
            module->m_stepTime += dt;
        }
        module->m_time += dt;
        module->field_0x1D = 0;
        module->field_0x1C = 0;
        if (!module->field_0x1E) {
            module->m_finished = false;
        }
    }
}

} /* extern "C" */
