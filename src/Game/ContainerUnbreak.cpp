/* Container::ContainerUnbreak (class name from RTTI): the UNBRK_CONTAINER gadget, a container that
 * cannot be broken. It shows the model of file 0xCD picked by its "kind" parameter. On its first
 * update, and again after each reset, it hands its position to a ContainerBehavior (class name
 * from RTTI) that then moves it every frame; the character info's command handler passes each
 * SetTransparentCommand on to the model. File name inferred from the class. Method names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* SetTransparentCommand (command type 0x17) is declared in Game/CharCommand.h. */

/* The command type CharCommand's constructor stores at 0x4 (inferred). */
inline u16 GetCommandType(CharCommand &command) {
    return *(u16 *)command.field_0x4;
}

/* The command handler of charInfo (name inferred). */
inline CharCommandUnit *GetCommandHandler(GadgetCharInfo *charInfo) {
    return charInfo->m_commandUnit.px;
}

/* ContainerBehavior (class name from RTTI): moves a container while its owner carries it (meaning
 * inferred). Its unit precedes this one; only the members used here are declared. */
class ContainerBehavior {
  public:
    ContainerBehavior(GadgetCharInfo *charInfo, boost::shared_ptr<GadgetModelInfo> model);
    virtual ~ContainerBehavior();

    /* 0x04 */ u8 field_0x4[0x40];
};
SIZE_ASSERT(ContainerBehavior, 0x44);

extern "C" {
/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* Fades the model to alpha, and makes it opaque again (behaviour inferred). */
void fn_8015FDD8(GadgetModelInfo *model, float alpha);
void fn_8015FD10(GadgetModelInfo *model);
/* Switches the model to number index, clamped to the models it has, and returns the number shown
 * (behaviour inferred; arg2 unknown). */
int fn_801625D8(GadgetModelInfo *model, int index, int arg2);
/* Copies params back into the owner's parameter block when its size equals size (behaviour
 * inferred; the counterpart of fn_800C9DBC). */
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, const void *params, int size);
/* Tells the owner the gadget is done (behaviour inferred). */
void fn_800C9B34(UnkPlacement_800C9CD0 *owner);
/* Starts the behavior at position and rotation; moves it there; advances it; copies its position
 * out (behaviours inferred). */
void fn_800F8FC4(ContainerBehavior *behavior, const Vec3f *position,
                 const Vec3f *rotation);
void fn_800EF474(ContainerBehavior *behavior, const Vec3f *position,
                 const Vec3f *rotation);
void fn_800F9000(ContainerBehavior *behavior);
void fn_800EF448(ContainerBehavior *behavior, Vec3f *position);
}

namespace Container {

/* The parameter the owner supplies (name from the parameter table). */
struct ContainerUnbreakParams {
    ContainerUnbreakParams() : kind(0) {}

    /* 0x0 */ int kind;
};
SIZE_ASSERT(ContainerUnbreakParams, 0x4);

class ContainerUnbreak : public Gadget {
  public:
    ContainerUnbreak(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ContainerUnbreak();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the container under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The character info's command callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* Places the model at the character info's translation, turned about the Y axis (name
     * inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation and parameter, raised by 10 (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ ContainerUnbreakParams m_params;
    /* 0x44 */ int m_kind; /* the model shown (name inferred) */
    /* 0x48 */ boost::shared_ptr<ContainerBehavior> m_behavior;
    /* 0x50 */ int m_state; /* 0 start, 1 moving, 3 done; nothing here sets 2 or 3 (inferred) */
    /* 0x54 */ u32 field_0x54; /* not used by this unit */
};
SIZE_ASSERT(ContainerUnbreak, 0x58);

const char *ContainerUnbreak::s_taskName = "ContainerUnbreak";

ContainerUnbreak::ContainerUnbreak(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CD90(4, 4);
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&GetCommandHandler(m_chara.get())->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&ContainerUnbreak::OnCommand, this, _1)));
    m_model = fn_8015F9D8(0, 0xCD);
    m_kind = m_params.kind;
    m_model->fn_80162A68(m_kind, 1);
    m_model->fn_80161F08("col", m_chara);
    m_state = 0;
    LoadFromOwner(m_owner);
    RebuildTransform();
}

ContainerUnbreak::~ContainerUnbreak() {}

void ContainerUnbreak::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        if (m_behavior.get() == 0) {
            m_behavior = boost::shared_ptr<ContainerBehavior>(
                new ContainerBehavior(m_chara.get(), m_model));
        } else {
            fn_800F8FC4(m_behavior.get(), &m_chara->m_position, &m_chara->m_rotation);
        }
        m_state = 1;
        break;
    case 1:
        if (m_behavior.get() != 0) {
            m_chara->m_position.y -= 10.0f;
            fn_800EF474(m_behavior.get(), &m_chara->m_position, &m_chara->m_rotation);
            fn_800F9000(m_behavior.get());
            fn_800EF448(m_behavior.get(), &m_chara->m_position);
            m_chara->m_position.y += 10.0f;
        }
        break;
    case 2:
        break;
    case 3:
        fn_800C9B34(m_owner);
        m_flags |= 1;
        break;
    }

    RebuildTransform();
}

void ContainerUnbreak::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    if (m_model.get() != 0) {
        if (m_params.kind != m_kind) {
            m_kind = fn_801625D8(m_model.get(), m_params.kind, 0);
            m_params.kind = m_kind;
            fn_800C9D68(owner, &m_params, sizeof(m_params));
        }
        RebuildTransform();
        m_model->fn_8016108C();
    }
}

void ContainerUnbreak::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara->m_position.y += 10.0f;
    m_state = 0;
}

void ContainerUnbreak::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

void ContainerUnbreak::OnCommand(CharCommand &command) {
    switch (GetCommandType(command)) {
    case 0x17:
        if (static_cast<SetTransparentCommand &>(command).m_alpha < 1.0f) {
            if (m_model.get() != 0) {
                fn_8015FDD8(m_model.get(), static_cast<SetTransparentCommand &>(command).m_alpha);
            }
        } else {
            if (m_model.get() != 0) {
                fn_8015FD10(m_model.get());
            }
        }
        break;
    }
}

} // namespace Container

extern "C" void fn_800FBDEC(void) {
    fn_8006B50C(0xCD, 1);
}

extern "C" void fn_800FBDE8(void) {}

void Container::ContainerUnbreak::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ContainerUnbreak(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_80525754[] = {
    {1, "kind", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_805257A4 = {
    "UNBRK_CONTAINER", fn_800FBDEC, fn_800FBDE8, Container::ContainerUnbreak::Instantiate, 0, 0, 0xB, 1,
    0, 10, lbl_80525754,
};
