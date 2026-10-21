/* The GadgetCommonDataStart and GadgetCommonDataEnd gadget types (names from their records), whose
 * load and unload hooks set up and release the data all gadgets share (the stg_cmn_gdt.one file and
 * the managers the hooks call), and the effect spawning helpers gadget units call. File name
 * inferred from the records. Names of the free functions are placeholders; their behaviour is
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/GadgetClassDesc.h"
#include "Game/ResourceOneFile.h"
#include "Game/ResourceOneFileReq.h"
#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"

namespace Effect {
namespace System {
class EffectElement;
} // namespace System
} // namespace Effect

extern "C" {
/* Spawns effect id (the other arguments' meanings are unknown). */
boost::weak_ptr<Effect::System::EffectElement> fn_801D2B74(int arg0, const Vec3f *position,
                                                           int id, int arg3);
boost::weak_ptr<Effect::System::EffectElement> fn_801D2AFC(int arg0, const Vec3f *position,
                                                           const Vec3f &up, int id, int arg4);
/* The common gadget data's managers and their load and release calls (names unknown). */
void fn_8006B5A0(void);
void fn_8015F018(void);
void fn_800CB69C(void);
void fn_801692A0(void);
void fn_8015FA48(void);
void fn_80169258(void);
void fn_8015EF54(void);
}

/* What the common data's file request is handed (meaning unknown). */
extern u8 lbl_80575EE4[];

/* GadgetCommonDataStart's hooks. */
extern "C" void fn_8015D8B4(void) {
    fn_8006B5A0();
    fn_8015F018();
    fn_800CB69C();
    fn_801692A0();
    fn_8015FA48();
}

extern "C" void fn_8015D8B0(void) {}

/* GadgetCommonDataEnd's load hook: puts the common gadget data file in loader slot 0. */
extern "C" void fn_8015D854(void) {
    ResourceOneFileReq *resource = new ResourceOneFileReq("stg_cmn_gdt.one", lbl_80575EE4);
    fn_8001299C(fn_800122D4(), 0, reinterpret_cast<ResourceOneFile *>(resource));
}

/* GadgetCommonDataEnd's unload hook (names of the managers unknown). */
extern "C" void fn_8015D820(void) {
    fn_800CB69C();
    fn_80169258();
    fn_8015EF54();
    fn_800127F8(fn_800122D4(), 0);
}

/* Spawns effect id at position, turned to up. */
extern "C" boost::weak_ptr<Effect::System::EffectElement> fn_8015D7DC(const Vec3f *position,
                                                                     const Vec3f &up, int id) {
    return fn_801D2AFC(0, position, up, id, 0);
}

/* Spawns effect id at position, upright. */
extern "C" boost::weak_ptr<Effect::System::EffectElement> fn_8015D794(const Vec3f *position,
                                                                     int id) {
    return fn_8015D7DC(position, Vec3f(0.0f, 1.0f, 0.0f), id);
}

/* Spawns effect id at position. */
extern "C" boost::weak_ptr<Effect::System::EffectElement> fn_8015D754(const Vec3f *position,
                                                                     int id) {
    return fn_801D2B74(0, position, id, 0);
}

GadgetClassDesc lbl_8052AFC8 = {
    "GadgetCommonDataStart", fn_8015D8B4, fn_8015D8B0, 0, 0, 0x80, 0x2580, 1, 0, 0, 0,
};

GadgetClassDesc lbl_8052AFEC = {
    "GadgetCommonDataEnd", fn_8015D854, fn_8015D820, 0, 0, 0x80, 0x2581, 1, 0, 0, 0,
};
