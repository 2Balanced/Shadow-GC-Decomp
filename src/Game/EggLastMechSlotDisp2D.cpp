/* EggLastMech::EggLastMechSlotDisp2D (class name from the RTTI of its shared_ptr's control block):
 * the 2D display of EggLastMech's slot machine (Game/EggLastMechSlot.cpp). It hides its state in an
 * EggLastMechSlotDisp2D::Impl (class name from the RTTI of its draw callback's bind_t) that plays the
 * CSD project "CsdFiles/EggBoss": the scenes DisplayLg and DisplaySml, and one scene per reel ("1"
 * to "3") placed on DisplaySml's node Null_display_sml. The reels' scenes follow the reels' angles;
 * a matched line shows its symbol on DisplayLg's nodes icon1 to icon3. The file name and the
 * names marked "(name inferred)" are not original; the functions other units call keep their fn_
 * names. */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CsdRCPtr.h"
#include "Game/Render.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CNode;
using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

/* The argument a Render passes to its draw callbacks, as far as this unit reads it. */
struct RednerDrawArgs {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ float m_dt;
};

/* The loaded CSD resource the projects are built from (class name unknown; 0x4C bytes, fn_8003E808
 * constructs it, fn_8036E114 loads a file into it). */
class UnkCsdResource_8003E808 {
  public:
    UnkCsdResource_8003E808();
    virtual ~UnkCsdResource_8003E808();

    /* 0x04 */ u8 field_0x4[0x4C - 0x4];

    static void *operator new(u32 size) {
        return fn_803605AC()->Alloc(size, "csdBase.h", 40);
    }
};

/* The value a project returns from fn_8036D598 (class name unknown). */
class UnkCsdHandle_80042DF0 {
  public:
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* 0x0 */ void *m_object;
};

extern "C" {
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);

/* The resource manager's language-specific path of a file (behaviour inferred, see Gong.cpp). */
void *fn_8003F8D4(void);
const char *fn_800423B8(void *arg0, const char *name, int arg2);
/* Loads file into resource (behaviour inferred). */
void fn_8036E114(UnkCsdResource_8003E808 *resource, const char *file);

/* Projects (behaviour inferred): advancing and drawing, a scene by name, and fn_8036D598, whose
 * result is dropped. */
void fn_8036DA1C(UnkCsdProject_800BFF30 *project, float dt);
void fn_8036D94C(UnkCsdProject_800BFF30 *project);
RCPtr<CScene> fn_8036D660(UnkCsdProject_800BFF30 *project, const char *name);
UnkCsdHandle_80042DF0 fn_8036D598(UnkCsdProject_800BFF30 *project, int arg1);

/* Scenes (behaviour inferred): the motion frame, the speed, stopping and playing, two words of the
 * scene, a node by name, attaching to a node, and the pattern. */
void fn_80366740(CScene *scene, float frame);
void fn_80367140(CScene *scene, float speed);
void fn_803659E4(CScene *scene);
void fn_803659F0(CScene *scene);
void fn_803659DC(CScene *scene, int arg1);
void fn_803659CC(CScene *scene, int arg1);
void fn_80367274(CScene *scene, int pattern);
RCPtr<CNode> fn_803666B4(CScene *scene, const char *name);
void fn_803663C0(CScene *scene, RCPtr<CNode> *node);
/* Nodes (behaviour inferred): the pattern. */
void fn_8036AD14(CNode *node, int pattern);
}

namespace EggLastMech {

class EggLastMechSlotDisp2D {
  public:
    class Impl {
      public:
        Impl();
        ~Impl();

        /* The Render's draw callback (name inferred). */
        void Draw(RednerDrawArgs &args);
        /* Runs DisplaySml's show and hide animations (name inferred). */
        void Tick(float dt);
        /* Plays DisplaySml backwards (name inferred). */
        void Close();
        /* Shows reel's angle (name inferred). */
        void SetAngle(int reel, float angle);
        /* Shows the matched symbol (name inferred). */
        void ShowResult(int symbol, int shotType);

        /* Opens DisplaySml on the next update (name inferred). */
        void Open() {
            m_open = 1;
        }

        /* Loads the CSD resource every Impl plays (name inferred). */
        static void Load();

        static UnkCsdResource_8003E808 *s_resource;

        /* 0x00 */ UnkCsdProject_800BFF30 *m_project;
        /* 0x04 */ RCPtr<CScene> m_large;
        /* 0x08 */ RCPtr<CScene> m_small;
        /* 0x0C */ RCPtr<CScene> m_reels[3];
        /* 0x18 */ boost::shared_ptr<Render> m_render;
        /* 0x20 */ u8 m_open;
        /* 0x24 */ float m_timer;
        /* 0x28 */ s32 m_state;
        /* 0x2C */ u8 m_hidden;
    };

    EggLastMechSlotDisp2D();
    ~EggLastMechSlotDisp2D();

    /* 0x0 */ boost::scoped_ptr<Impl> m_impl;
};

/* Deletes the Impl (name inferred; see checked_delete below). */
inline void DeleteImpl(EggLastMechSlotDisp2D::Impl *impl) {
    delete impl;
}

/* Deletes the display (name inferred; see checked_delete below). */
inline void DeleteDisp2D(EggLastMechSlotDisp2D *disp) {
    delete disp;
}

} // namespace EggLastMech

/* Retail keeps boost::checked_delete of both classes out of line and their destructors at their
 * own places; these specializations give that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<EggLastMech::EggLastMechSlotDisp2D>(EggLastMech::EggLastMechSlotDisp2D *x) {
    EggLastMech::DeleteDisp2D(x);
}
template <>
inline void checked_delete<EggLastMech::EggLastMechSlotDisp2D::Impl>(EggLastMech::EggLastMechSlotDisp2D::Impl *x) {
    EggLastMech::DeleteImpl(x);
}
} // namespace boost

SIZE_ASSERT(EggLastMech::EggLastMechSlotDisp2D::Impl, 0x30);
SIZE_ASSERT(EggLastMech::EggLastMechSlotDisp2D, 0x4);

/* The frame of each symbol's icon (values from .rodata). */
static const s32 s_symbolFrames[5] = {0, 4, 3, 2, 1};

/* Not original: an unused table that pools the unit's .rodata strings in retail's order (the
 * linker drops the table; the RTTI names and literals below reuse these copies). The kept-whole
 * .rodata also holds RTTI names of the CSD library headers' inline classes, which this unit does
 * not reconstruct (see Game/Gong.cpp). */
static const char *s_rodataPool[] = {
    "boost::_bi::bind_t<void, boost::_mfi::mf1<void, EggLastMech::EggLastMechSlotDisp2D::Impl, RednerDrawArgs &>, boost::_bi::list2<boost::_bi::value<EggLastMech::EggLastMechSlotDisp2D::Impl *>, boost::arg<1>>>",
    "Not inititalized m_eDealloctor\n",
    "Memory allocation failure",
    "csdBase.h",
    "DisplayLg",
    "DisplaySml",
    "Null_display_sml",
    "csdRCObject.h",
    "CsdFiles/EggBoss",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>::CFunctorBase",
    "Chao::CSD::CBase",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>",
    "Chao::CSD::CResourceBase<Chao::CSD::Project>",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>",
    "boost::detail::sp_counted_base_impl<EggLastMech::EggLastMechSlotDisp2D *, boost::checked_deleter<EggLastMech::EggLastMechSlotDisp2D>>",
    "boost::detail::sp_counted_base",
    "std::exception",
    "exception",
    "std::bad_alloc",
    "bad_alloc",
    "boost::checked_deleter<EggLastMech::EggLastMechSlotDisp2D>",
};

/* The nodes of DisplayLg that show a matched symbol, and the reels' scenes. */
static const char *s_iconNodes[3] = {"icon1", "icon2", "icon3"};
static const char *s_reelScenes[3] = {"1", "2", "3"};

using EggLastMech::EggLastMechSlotDisp2D;

namespace EggLastMech {

UnkCsdResource_8003E808 *EggLastMechSlotDisp2D::Impl::s_resource;

void EggLastMechSlotDisp2D::Impl::Load() {
    s_resource = new UnkCsdResource_8003E808();
    fn_8036E114(s_resource, fn_800423B8(fn_8003F8D4(), "CsdFiles/EggBoss", 0));
}

EggLastMechSlotDisp2D::Impl::Impl()
    : m_project(0), m_large(0), m_small(0), m_open(0), m_timer(0.0f), m_state(0), m_hidden(0) {
    for (int i = 0; i < 3; i++) {
        m_reels[i] = 0;
    }
    m_render = fn_8004E24C(fn_80046178()->field_0x54);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&Impl::Draw, this, _1));
    if (s_resource != 0) {
        m_project = new UnkCsdProject_800BFF30(s_resource);
        if (m_project != 0) {
            fn_8036D598(m_project, 0);
            m_large = fn_8036D660(m_project, "DisplayLg");
            fn_803659DC(m_large.Get(), 0);
            fn_803659F0(m_large.Get());
            m_small = fn_8036D660(m_project, "DisplaySml");
            fn_803659DC(m_small.Get(), 0);
            fn_803659CC(m_small.Get(), 0);
            fn_803659F0(m_small.Get());
            RCPtr<CNode> node = fn_803666B4(m_small.Get(), "Null_display_sml");
            for (int i = 0; i < 3; i++) {
                m_reels[i] = fn_8036D660(m_project, s_reelScenes[i]);
                fn_803663C0(m_reels[i].Get(), &node);
                fn_803659DC(m_reels[i].Get(), 1);
                fn_803659F0(m_reels[i].Get());
            }
        }
    }
}

EggLastMechSlotDisp2D::Impl::~Impl() {
    if (m_project != 0) {
        delete m_project;
        m_project = 0;
    }
}

void EggLastMechSlotDisp2D::Impl::ShowResult(int symbol, int shotType) {
    if (m_large.Get() != 0) {
        fn_80367274(m_large.Get(), shotType);
        fn_80366740(m_large.Get(), 0.0f);
        fn_803659E4(m_large.Get());
        for (u32 i = 0; i < 3; i++) {
            RCPtr<CNode> node = fn_803666B4(m_large.Get(), s_iconNodes[i]);
            fn_8036AD14(node.Get(), s_symbolFrames[symbol]);
        }
    }
    Close();
}

void EggLastMechSlotDisp2D::Impl::SetAngle(int reel, float angle) {
    if (m_reels[reel].Get() != 0) {
        float frame = 60.0f * angle;
        if (frame > 50.0f) {
            frame -= 50.0f;
        }
        fn_80366740(m_reels[reel].Get(), frame);
        m_reels[reel].Get()->Tick(0.001f);
    }
}

void EggLastMechSlotDisp2D::Impl::Close() {
    if (m_small.Get() != 0) {
        fn_80367140(m_small.Get(), -1.0f);
        fn_803659E4(m_small.Get());
        m_state = 3;
    }
}

void EggLastMechSlotDisp2D::Impl::Tick(float dt) {
    if (m_small.Get() != 0) {
        if (m_open) {
            m_open = 0;
            fn_80367140(m_small.Get(), 1.0f);
            fn_803659E4(m_small.Get());
            m_state = 1;
        }
        switch (m_state) {
        case 0:
            break;
        case 1:
            if (m_small->IsEnd()) {
                m_timer = 2.0f;
                m_state = 2;
            }
            break;
        case 2:
            break;
        case 3:
            if (m_small->IsEnd()) {
                fn_80366740(m_small.Get(), 0.0f);
                m_state = 0;
            }
            break;
        }
    }
}

void EggLastMechSlotDisp2D::Impl::Draw(RednerDrawArgs &args) {
    if (m_project != 0 && !m_hidden) {
        Tick(args.m_dt);
        fn_8036DA1C(m_project, args.m_dt);
        fn_8036D94C(m_project);
    }
}

EggLastMechSlotDisp2D::EggLastMechSlotDisp2D() : m_impl(new Impl) {}

EggLastMechSlotDisp2D::~EggLastMechSlotDisp2D() {}

} // namespace EggLastMech

/* Shows the matched symbol (name inferred). */
extern "C" void fn_8032DA3C(EggLastMechSlotDisp2D *disp, int symbol, int shotType) {
    disp->m_impl->ShowResult(symbol, shotType);
}

/* Shows reel's angle (name inferred). */
extern "C" void fn_8032DA18(EggLastMechSlotDisp2D *disp, int reel, float angle) {
    disp->m_impl->SetAngle(reel, angle);
}

/* Shows the display (name inferred). */
extern "C" void fn_8032D9E8(EggLastMechSlotDisp2D *disp) {
    disp->m_impl->Open();
}

/* Hides the display (name inferred). */
extern "C" void fn_8032D9D8(EggLastMechSlotDisp2D *disp) {
    disp->m_impl->m_hidden = 1;
}

/* Loads the CSD resource (name inferred). */
extern "C" void fn_8032D9B8(void) {
    EggLastMechSlotDisp2D::Impl::Load();
}

/* Frees the CSD resource (name inferred). */
extern "C" void fn_8032D970(void) {
    if (EggLastMechSlotDisp2D::Impl::s_resource != 0) {
        delete EggLastMechSlotDisp2D::Impl::s_resource;
        EggLastMechSlotDisp2D::Impl::s_resource = 0;
    }
}

/* Creates the display (name inferred). */
extern "C" boost::shared_ptr<EggLastMechSlotDisp2D> fn_8032D744(void) {
    boost::shared_ptr<EggLastMechSlotDisp2D> disp = boost::shared_ptr<EggLastMechSlotDisp2D>(new EggLastMechSlotDisp2D);
    return disp;
}
