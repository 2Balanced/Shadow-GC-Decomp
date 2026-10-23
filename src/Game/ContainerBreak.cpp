/* What a broken container leaves behind of itself (fn_8033E644): the pieces of its model, its
 * break sound and effect 0xB6. The containers (Game/ContainerHard.cpp, ContainerNormal.cpp,
 * ContainerSecret.cpp, ContainerWeapon.cpp) and the thrown ones (Game/ThrowContainer.cpp) call it;
 * Game/ContainerContent.cpp releases what they held. The unit has no RTTI; its extent comes from
 * the float pool (retail keeps 1.0 and 10.0 apart from the next unit's). The file and function
 * names are ours. */
#include "Effect/System/EffectElement.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The parameters of a model's break into pieces (as Game/GadgetCrash3D.cpp declares them; there
 * the first word of an entry is not named). */
struct UnkCrash3DParam_8015F4AC {
    UnkCrash3DParam_8015F4AC(int index);
    /* Drops ground onto the ground below position. */
    void FindGround();

    /* An entry of the piece table: the number of pieces of a model. */
    struct Entry {
        const char *name;
        s32 count;
    };

    /* 0x00 */ Task *parent;
    /* 0x04 */ Vec3f position;
    /* 0x10 */ Vec3f rotation;
    /* 0x1C */ float speed;
    /* 0x20 */ s32 type;
    /* 0x24 */ float height;
    /* 0x28 */ float ground;
    /* 0x2C */ void **models;
    /* 0x30 */ Entry *entries;
    /* 0x34 */ s16 modelCount;
    /* 0x36 */ s16 field_0x36;
    /* 0x38 */ s32 order;
};
SIZE_ASSERT(UnkCrash3DParam_8015F4AC, 0x3C);

/* A part of a model (layout from Game/BKTank.cpp). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ const char *name;
};

/* A sound of a model's data (layout from Game/Coaster.cpp). */
struct UnkSoundEntry_80208888 {
    /* 0x00 */ const char *m_name;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 m_soundId; /* passed to the sound calls as 16 bits */
};

extern "C" {
/* The model's part id, and the model file of a part (behaviour inferred; see Game/BKTank.cpp). */
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
void *fn_800122D4(void);
void *fn_80012700(void *manager, int arg1, const char *fileName);
/* Starts the pieces (behaviour inferred). */
void fn_8015D97C(UnkCrash3DParam_8015F4AC *param);
/* The sound kind of a model's data (behaviour inferred). */
UnkSoundEntry_80208888 *fn_80208848(void *data, int kind);
/* Plays sound id at position (behaviour inferred). */
void fn_801D54F4(void *manager, u16 id, const Vec3f *position, int arg3, int arg4);
/* Starts effect id at position (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D794(const Vec3f *position, int id);
}

/* The piece table of a broken container: three models (names inferred). */
static UnkCrash3DParam_8015F4AC::Entry lbl_8055DB78[3] = {
    {"", 3},
    {"", 2},
    {"", 2},
};

/* The model's data (GadgetModelInfo::m_data, at 0x30). */
inline void *GetModelData(GadgetModelInfo *model) {
    return *reinterpret_cast<void **>(model->field_0x4 + (0x30 - 0x4));
}

/* Breaks model into the pieces of kind (parts kind + 1000, 2000 and 3000), and plays its break
 * sound and effect 0xB6 at position. */
extern "C" void fn_8033E644(boost::shared_ptr<GadgetModelInfo> model, int kind,
                            const Vec3f *position, const Vec3f *rotation) {
    void *models[3] = {0, 0, 0};
    if (model.get() != 0) {
        UnkModelPart_8015F7BC *part = fn_8015F7BC(model.get(), kind + 1000);
        if (part != 0) {
            models[0] = fn_80012700(fn_800122D4(), 0, part->name);
        }
        part = fn_8015F7BC(model.get(), kind + 2000);
        if (part != 0) {
            models[1] = fn_80012700(fn_800122D4(), 0, part->name);
        }
        part = fn_8015F7BC(model.get(), kind + 3000);
        if (part != 0) {
            models[2] = fn_80012700(fn_800122D4(), 0, part->name);
        }
    }
    int count = 0;
    for (int i = 0; i < 3; i++) {
        if (models[i] != 0) {
            count++;
        }
    }
    if (count > 0) {
        UnkCrash3DParam_8015F4AC param(0);
        AssignVector(param.position, *position);
        param.rotation = *rotation;
        param.order = 2;
        param.models = models;
        param.entries = lbl_8055DB78;
        param.modelCount = 3;
        param.speed = 1.5f;
        param.height = 1.0f;
        param.ground = position->y - 10.0f;
        param.FindGround();
        fn_8015D97C(&param);
    }
    if (GetModelData(model.get()) != 0) {
        UnkSoundEntry_80208888 *sound = fn_80208848(GetModelData(model.get()), kind);
        if (sound != 0) {
            fn_801D54F4(fn_80053164(), sound->m_soundId, position, 0, 1);
        }
    }
    fn_8015D794(position, 0xB6);
}
