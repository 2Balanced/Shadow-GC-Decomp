/* Effect::RW3::EffectObjectRW3 (class name and bases from RTTI): the RenderWare model effect
 * element. It makes its model (a clump) from the model data of the RenderWare launch description,
 * keeps the clump's frame at the element's matrix and renders it, or adds it to the world the
 * element is given. File name from the class; member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/EffectRW3.h"

using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;

extern "C" {
/* RenderWare: makes a clump from model data (see Game/EffectObjectUSA.cpp), RpClumpDestroy,
 * RpClumpRender, RpClumpGetWorld, RpWorldAddClump, RpWorldRemoveClump and RwFrameUpdateObjects. */
void *fn_80457090(void *data);
s32 fn_80457468(void *clump);
void *fn_80456BC4(void *clump);
void *fn_8046256C(void *clump);
void *fn_804622FC(void *world, void *clump);
void *fn_804623EC(void *world, void *clump);
void *fn_8048722C(RwFrame *frame);
}

namespace Effect {
namespace RW3 {

EffectObjectRW3::EffectObjectRW3(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                                 EffectManager *manager)
    : EffectObject(desc, info, manager) {
    m_clump = 0;
    m_uvAnim = 0;
    m_world = 0;
    m_inWorld = false;
    m_size -= sizeof(EffectObject);
    m_size += sizeof(EffectObjectRW3);
    if (!(m_flags & 1)) {
        return;
    }
    EffectLaunchInfoRW3 *rw3Info = dynamic_cast<EffectLaunchInfoRW3 *>(info);
    if (rw3Info != 0) {
        if (rw3Info->field_0xC != 0) {
            m_clump = fn_80457090(rw3Info->field_0xC);
        }
        if (m_clump == 0) {
            m_flags &= ~1;
            return;
        }
    }
}

EffectObjectRW3::~EffectObjectRW3() {
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
    }
}

void EffectObjectRW3::Update(float dt) {
    EffectObject::Update(dt);
    if ((m_flags & 1) && !(m_flags & 4)) {
        if (m_clump == 0) {
            m_flags &= ~1;
            return;
        }
        RwFrame *frame = static_cast<RpClump *>(m_clump)->frame;
        if (frame != 0) {
            RwMatrix *matrix = &frame->modelling;
            if (matrix != 0) {
                matrix->right.x = m_worldMatrix.values[0][0];
                matrix->right.y = m_worldMatrix.values[1][0];
                matrix->right.z = m_worldMatrix.values[2][0];
                matrix->up.x = m_worldMatrix.values[0][1];
                matrix->up.y = m_worldMatrix.values[1][1];
                matrix->up.z = m_worldMatrix.values[2][1];
                matrix->at.x = m_worldMatrix.values[0][2];
                matrix->at.y = m_worldMatrix.values[1][2];
                matrix->at.z = m_worldMatrix.values[2][2];
                matrix->pos.x = m_worldMatrix.values[0][3];
                matrix->pos.y = m_worldMatrix.values[1][3];
                matrix->pos.z = m_worldMatrix.values[2][3];
                matrix->flags &= ~0x20003;
                fn_8048722C(frame);
            }
        }
    }
}

void EffectObjectRW3::Draw() {
    if (!(m_flags & 1) || (m_flags & 8)) {
        if (m_inWorld) {
            void *world = fn_8046256C(m_clump);
            if (world != 0) {
                fn_804623EC(world, m_clump);
            }
            m_inWorld = false;
        }
        return;
    }
    if (m_world != 0 && !m_inWorld) {
        fn_804622FC(m_world, m_clump);
        m_inWorld = true;
    }
    if (!m_inWorld && m_clump != 0) {
        fn_80456BC4(m_clump);
    }
}

} // namespace RW3
} // namespace Effect
