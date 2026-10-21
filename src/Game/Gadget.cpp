/* Gadget (class name from RTTI): the base of the stage objects (see Game/Gadget.h). The
 * constructor stores the owner record and registers with it, the destructor unregisters, and
 * Tick runs Behave until HasEnded reports that the owner went away. The functions before
 * HasEnded forward to the owner record; units call them by their addresses. File name inferred
 * from the class. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/Gadget.h"

extern "C" {
/* The owner record's side of the gadget link (behaviour inferred): register, unregister, two
 * state tests and four notifications. */
void fn_800C9CD0(UnkPlacement_800C9CD0 *owner, Gadget *gadget);
void fn_800C9BE8(UnkPlacement_800C9CD0 *owner);
bool fn_800C9A64(UnkPlacement_800C9CD0 *owner);
bool fn_800C996C(UnkPlacement_800C9CD0 *owner);
void fn_800C9B04(UnkPlacement_800C9CD0 *owner);
void fn_800C9B34(UnkPlacement_800C9CD0 *owner);
void fn_800C9AD4(UnkPlacement_800C9CD0 *owner);
void fn_800C9690(UnkPlacement_800C9CD0 *owner);
}

extern "C" {
inline bool fn_8015A858(Gadget *gadget);
inline bool fn_8015A818(Gadget *gadget);
}

const char *Gadget::s_taskName = "Gadget";

Gadget::Gadget(Task *parent, UnkPlacement_800C9CD0 *owner) : Task(parent) {
    m_name = s_taskName;
    m_owner = 0;
    if (owner != 0) {
        m_owner = owner;
        fn_800C9CD0(m_owner, this);
    }
}

Gadget::~Gadget() {
    if (m_owner != 0) {
        fn_800C9BE8(m_owner);
        m_owner = 0;
    }
}

void Gadget::Tick(float dt) {
    if (!HasEnded()) {
        Behave(dt);
    }
}

bool Gadget::HasEnded() {
    if (fn_8015A858(this)) {
        m_flags |= 1;
        return true;
    }
    if (fn_8015A818(this)) {
        m_flags |= 1;
        return true;
    }
    return false;
}

/* True when the owner reports state fn_800C9A64 (name and meaning unknown). */
extern "C" inline bool fn_8015A858(Gadget *gadget) {
    if (gadget->m_owner != 0 && fn_800C9A64(gadget->m_owner)) {
        return true;
    }
    return false;
}

/* True when the owner reports state fn_800C996C (name and meaning unknown). */
extern "C" inline bool fn_8015A818(Gadget *gadget) {
    if (gadget->m_owner != 0 && fn_800C996C(gadget->m_owner)) {
        return true;
    }
    return false;
}

/* Not called anywhere: the linker strips it. Retail inlines the two tests above into HasEnded
 * and also has them out of line (other units call them) just below HasEnded. Taking their
 * addresses here makes GC/2.7 emit the out-of-line copies at that place and in that order
 * (inferred; how the original source produced them is unknown, and the name is a placeholder). */
static void UnkAddressUser(void **out) {
    out[0] = (void *)fn_8015A858;
    out[1] = (void *)fn_8015A818;
}

extern "C" void fn_8015A7EC(Gadget *gadget) {
    if (gadget->m_owner != 0) {
        fn_800C9690(gadget->m_owner);
    }
}

extern "C" void fn_8015A7C0(Gadget *gadget) {
    if (gadget->m_owner != 0) {
        fn_800C9AD4(gadget->m_owner);
    }
}

/* Tells the owner that the gadget is done (behaviour inferred). */
extern "C" void fn_8015A794(Gadget *gadget) {
    if (gadget->m_owner != 0) {
        fn_800C9B34(gadget->m_owner);
    }
}

extern "C" void fn_8015A768(Gadget *gadget) {
    if (gadget->m_owner != 0) {
        fn_800C9B04(gadget->m_owner);
    }
}
