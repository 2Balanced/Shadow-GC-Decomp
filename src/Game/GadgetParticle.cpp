/* GadgetParticle::GadgetParticleCtrl (class name from the RTTI of its shared_ptr control block):
 * the particle effects of a gadget model. Each atomic of the model's clump whose geometry user data
 * names a particle (ATTR_PTCL.PTCLCATEGORY, .PTCLID, .PTCLFLAG, and ATTR_ID.ATTR_ID) becomes an
 * entry; the gadgets start, hide, show and stop the entries' effects, all of them or those with one
 * ATTR_ID, and the controller moves running effects with their atomics' frames. File name inferred
 * from the class. Names marked "(name inferred)" are not original; the gadgets call the
 * controller's operations under their addresses. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define GADGETPARTICLE_IMPL
#define MSL_VECTOR_DESTROY_ALL_LATE
#include "Game/GadgetParticle.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

#include <string.h>

using Effect::System::EffectElement;
using GadgetParticle::GadgetParticleCtrl;
using GadgetParticle::ParticleEntry;
using GadgetParticle::ParticleEntryList;

/* The RenderWare types this unit touches (layouts from the accesses; RenderWare's headers are not
 * part of this project, as in TurnOver.cpp). */
struct RwV3d {
    float x;
    float y;
    float z;
};

struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
};
SIZE_ASSERT(RwMatrix, 0x40);

struct RwObject {
    /* 0x0 */ u8 type;
    /* 0x1 */ u8 subType;
    /* 0x2 */ u8 flags; /* an atomic's bit 2: rendered */
    /* 0x3 */ u8 privateFlags;
    /* 0x4 */ void *parent; /* an atomic's frame */
};

struct RpGeometry;
struct RpUserDataArray;

struct RpAtomic {
    /* 0x00 */ RwObject object;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ RpGeometry *geometry;
};

typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);

extern "C" {
RwMatrix *RwFrameGetLTM(RwFrame *frame);
RpClump *fn_80456C54(RpClump *clump, RpAtomicCallBack callback, void *data); /* for all atomics */
/* A geometry's user data arrays: their count, one of them, and an array's format (1: integers),
 * name and integer at index. */
s32 fn_80452B90(RpGeometry *geometry);
RpUserDataArray *fn_80452BD8(RpGeometry *geometry, s32 index);
s32 fn_80452C80(RpUserDataArray *data);
const char *fn_80452C78(RpUserDataArray *data);
s32 fn_80452C88(RpUserDataArray *data, s32 index);

/* Start effect id at position, pointing along direction (two effect sets; behaviour
 * inferred). */
boost::weak_ptr<EffectElement> fn_80168C48(const Vec3f *position, const Vec3f *direction, int id);
boost::weak_ptr<EffectElement> fn_8015D7DC(const Vec3f *position, const Vec3f *direction, int id);
/* Makes effect follow matrix (behaviour inferred). */
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);
/* A singleton accessor whose result is not used (see Game/BKTank.cpp). */
void *fn_800A31F0(void);

/* The atomic flag the entries without bit 1 of their flags keep (value 8). */
extern const u32 lbl_805F1A88;

boost::shared_ptr<GadgetParticleCtrl> fn_8016690C(RpClump *clump);
boost::shared_ptr<GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166B70(GadgetParticleCtrl *ctrl);
void fn_80166C70(GadgetParticleCtrl *ctrl);
void fn_80166D14(GadgetParticleCtrl *ctrl);
void fn_80166E20(GadgetParticleCtrl *ctrl, int id);
void fn_80166E84(GadgetParticleCtrl *ctrl, int id);
void fn_80166F88(GadgetParticleCtrl *ctrl);
void fn_80166FDC(GadgetParticleCtrl *ctrl, RpClump *clump);
void fn_80167010(GadgetParticleCtrl *ctrl, int id);
void fn_80167180(GadgetParticleCtrl *ctrl, int id);
void fn_801672E4(GadgetParticleCtrl *ctrl, int id);
void fn_80167448(GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticleCtrl *ctrl, float dt);
}

/* The user data names. */
const char *const s_categoryName = "ATTR_PTCL.PTCLCATEGORY";
const char *const s_effectIdName = "ATTR_PTCL.PTCLID";
const char *const s_flagsName = "ATTR_PTCL.PTCLFLAG";
const char *const s_attrIdName = "ATTR_ID.ATTR_ID";

/* A frame's world matrix in the game's row layout (name inferred). */
inline void GetFrameMatrix(Mtx34f *out, RwFrame *frame) {
    RwMatrix *ltm = RwFrameGetLTM(frame);
    out->values[0][0] = ltm->right.x;
    out->values[0][1] = ltm->up.x;
    out->values[0][2] = ltm->at.x;
    out->values[0][3] = ltm->pos.x;
    out->values[1][0] = ltm->right.y;
    out->values[1][1] = ltm->up.y;
    out->values[1][2] = ltm->at.y;
    out->values[1][3] = ltm->pos.y;
    out->values[2][0] = ltm->right.z;
    out->values[2][1] = ltm->up.z;
    out->values[2][2] = ltm->at.z;
    out->values[2][3] = ltm->pos.z;
}

RpAtomic *GadgetParticleCtrl::AddAtomic(RpAtomic *atomic, void *data) {
    ParticleEntryList *list = static_cast<ParticleEntryList *>(data);
    if (list != 0) {
        ParticleEntry entry;
        RpGeometry *geometry = atomic->geometry;
        if (geometry != 0) {
            s32 count = fn_80452B90(geometry);
            for (s32 i = 0; i < count; i++) {
                RpUserDataArray *userData = fn_80452BD8(geometry, i);
                if (userData != 0) {
                    if (fn_80452C80(userData) == 1 &&
                        strcmp(fn_80452C78(userData), s_categoryName) == 0) {
                        entry.m_category = fn_80452C88(userData, 0);
                    } else if (fn_80452C80(userData) == 1 &&
                               strcmp(fn_80452C78(userData), s_effectIdName) == 0) {
                        entry.m_effectId = fn_80452C88(userData, 0);
                    } else if (fn_80452C80(userData) == 1 &&
                               strcmp(fn_80452C78(userData), s_flagsName) == 0) {
                        entry.m_flags = fn_80452C88(userData, 0);
                    } else if (fn_80452C80(userData) == 1 &&
                               strcmp(fn_80452C78(userData), s_attrIdName) == 0) {
                        entry.m_attrId = fn_80452C88(userData, 0);
                    }
                }
            }
            if (!entry.IsIncomplete()) {
                if (!(entry.m_flags & 2)) {
                    /* Clears the atomic's rendered bit and sets the bits of lbl_805F1A88. */
                    int atomicFlags = atomic->object.flags;
                    atomicFlags &= ~4;
                    atomicFlags |= lbl_805F1A88;
                    atomic->object.flags = atomicFlags;
                }
                RwFrame *frame = (RwFrame *)atomic->object.parent;
                GetFrameMatrix(&entry.m_matrix, frame);
                entry.m_frame = frame;
                list->push_back(entry);
            }
        }
    }
    return atomic;
}

GadgetParticleCtrl::GadgetParticleCtrl(RpClump *clump) {
    if (clump != 0) {
        fn_80456C54(clump, AddAtomic, &m_entries);
    }
}

GadgetParticleCtrl::~GadgetParticleCtrl() {
    for (ParticleEntry *entry = m_entries.begin(); entry != m_entries.end(); entry++) {
        entry->Retire();
    }
    m_entries.clear();
}

/* Moves the following effects with their frames (name inferred). */
void fn_8016761C(GadgetParticleCtrl *ctrl, float dt) {
    for (ParticleEntry *entry = ctrl->m_entries.begin(); entry != ctrl->m_entries.end(); entry++) {
        if (entry->m_flags & 4) {
            boost::shared_ptr<EffectElement> effect(entry->m_effect.lock());
            if (effect && entry->m_frame != 0) {
                GetFrameMatrix(&entry->m_matrix, entry->m_frame);
                fn_80425E68(effect.get(), &entry->m_matrix);
            }
        }
    }
}

/* Starts a one-off effect at each entry the controller does not run (all, or those with ATTR_ID
 * id; name inferred). */
void fn_80167448(GadgetParticleCtrl *ctrl, int id) {
    for (ParticleEntry *entry = ctrl->m_entries.begin(); entry != ctrl->m_entries.end(); entry++) {
        if (!(entry->m_flags & 1) && (id == -1 || id == entry->m_attrId)) {
            Vec3f position(0.0f, 0.0f, 0.0f);
            Vec3f direction(0.0f, 0.0f, -1.0f);
            boost::weak_ptr<EffectElement> started;
            switch (entry->m_category) {
            case 0:
                started = fn_80168C48(&position, &direction, entry->m_effectId);
                break;
            case 1:
                started = fn_8015D7DC(&position, &direction, entry->m_effectId);
                break;
            }
            boost::shared_ptr<EffectElement> effect(started.lock());
            if (effect) {
                GetFrameMatrix(&entry->m_matrix, entry->m_frame);
                fn_80425E68(effect.get(), &entry->m_matrix);
            } else {
                fn_800A31F0();
            }
        }
    }
}

/* Starts the effects not running, whatever their flags, and has them follow their frames (name
 * inferred). */
void fn_801672E4(GadgetParticleCtrl *ctrl, int id) {
    for (ParticleEntry *entry = ctrl->m_entries.begin(); entry != ctrl->m_entries.end(); entry++) {
        if ((id == -1 || id == entry->m_attrId) && entry->m_effect.expired()) {
            Vec3f position(0.0f, 0.0f, 0.0f);
            Vec3f direction(0.0f, 0.0f, -1.0f);
            switch (entry->m_category) {
            case 0:
                entry->m_effect = fn_80168C48(&position, &direction, entry->m_effectId);
                break;
            case 1:
                entry->m_effect = fn_8015D7DC(&position, &direction, entry->m_effectId);
                break;
            }
            boost::shared_ptr<EffectElement> effect(entry->m_effect.lock());
            if (effect.get() != 0) {
                fn_80425E68(effect.get(), &entry->m_matrix);
                entry->m_flags |= 4;
            } else {
                fn_800A31F0();
            }
        }
    }
}

/* Starts the controller's effects not running, at their places (name inferred). */
void fn_80167180(GadgetParticleCtrl *ctrl, int id) {
    for (ParticleEntry *entry = ctrl->m_entries.begin(); entry != ctrl->m_entries.end(); entry++) {
        if ((entry->m_flags & 1) && (id == -1 || id == entry->m_attrId) &&
            entry->m_effect.expired()) {
            Vec3f position(0.0f, 0.0f, 0.0f);
            Vec3f direction(0.0f, 0.0f, -1.0f);
            switch (entry->m_category) {
            case 0:
                entry->m_effect = fn_80168C48(&position, &direction, entry->m_effectId);
                break;
            case 1:
                entry->m_effect = fn_8015D7DC(&position, &direction, entry->m_effectId);
                break;
            }
            boost::shared_ptr<EffectElement> effect(entry->m_effect.lock());
            if (effect.get() != 0) {
                fn_80425E68(effect.get(), &entry->m_matrix);
            } else {
                fn_800A31F0();
            }
        }
    }
}

/* Starts the controller's effects not running, following their frames (name inferred). */
void fn_80167010(GadgetParticleCtrl *ctrl, int id) {
    for (ParticleEntry *entry = ctrl->m_entries.begin(); entry != ctrl->m_entries.end(); entry++) {
        if ((entry->m_flags & 1) && (id == -1 || id == entry->m_attrId) &&
            entry->m_effect.expired()) {
            Vec3f position(0.0f, 0.0f, 0.0f);
            Vec3f direction(0.0f, 0.0f, -1.0f);
            switch (entry->m_category) {
            case 0:
                entry->m_effect = fn_80168C48(&position, &direction, entry->m_effectId);
                break;
            case 1:
                entry->m_effect = fn_8015D7DC(&position, &direction, entry->m_effectId);
                break;
            }
            boost::shared_ptr<EffectElement> effect(entry->m_effect.lock());
            if (effect.get() != 0) {
                fn_80425E68(effect.get(), &entry->m_matrix);
                entry->m_flags |= 4;
            } else {
                fn_800A31F0();
            }
        }
    }
}

/* Adds the entries of clump's atomics (name inferred). */
void fn_80166FDC(GadgetParticleCtrl *ctrl, RpClump *clump) {
    fn_80456C54(clump, GadgetParticleCtrl::AddAtomic, &ctrl->m_entries);
}

/* Stops every effect at once (name inferred). */
void fn_80166F88(GadgetParticleCtrl *ctrl) {
    for (ParticleEntry *entry = ctrl->m_entries.begin(); entry != ctrl->m_entries.end(); entry++) {
        entry->Stop();
    }
}

/* Stops the effects of the entries with ATTR_ID id at once (name inferred). */
void fn_80166E84(GadgetParticleCtrl *ctrl, int id) {
    for (ParticleEntry *entry = ctrl->m_entries.begin(); entry != ctrl->m_entries.end(); entry++) {
        if (entry->m_attrId == id) {
            entry->Stop();
        }
    }
}

/* Lets the effects of the entries with ATTR_ID id finish (name inferred). */
void fn_80166E20(GadgetParticleCtrl *ctrl, int id) {
    for (ParticleEntry *entry = ctrl->m_entries.begin(); entry != ctrl->m_entries.end(); entry++) {
        if (entry->m_attrId == id) {
            entry->Retire();
        }
    }
}

/* Lets every effect finish and drops the entries (name inferred). */
void fn_80166D14(GadgetParticleCtrl *ctrl) {
    for (ParticleEntry *entry = ctrl->m_entries.begin(); entry != ctrl->m_entries.end(); entry++) {
        entry->Retire();
    }
    ctrl->m_entries.clear();
}

/* Hides the running effects (name inferred). */
void fn_80166C70(GadgetParticleCtrl *ctrl) {
    for (ParticleEntry *entry = ctrl->m_entries.begin(); entry != ctrl->m_entries.end(); entry++) {
        if (boost::shared_ptr<EffectElement> effect = entry->m_effect.lock()) {
            effect->Hide();
        }
    }
}

/* Shows the running effects again (name inferred). */
void fn_80166B70(GadgetParticleCtrl *ctrl) {
    for (ParticleEntry *entry = ctrl->m_entries.begin(); entry != ctrl->m_entries.end(); entry++) {
        if (boost::shared_ptr<EffectElement> effect = entry->m_effect.lock()) {
            effect->Show();
        }
    }
}

/* Creates a controller without entries (name inferred). */
boost::shared_ptr<GadgetParticleCtrl> fn_80166AF4(void) {
    boost::shared_ptr<GadgetParticleCtrl> ctrl(boost::shared_ptr<GadgetParticleCtrl>(new GadgetParticleCtrl(0)));
    return ctrl;
}

/* Creates the controller of clump (name inferred). */
boost::shared_ptr<GadgetParticleCtrl> fn_8016690C(RpClump *clump) {
    boost::shared_ptr<GadgetParticleCtrl> ctrl(boost::shared_ptr<GadgetParticleCtrl>(new GadgetParticleCtrl(clump)));
    return ctrl;
}

#include <vector_impl>
