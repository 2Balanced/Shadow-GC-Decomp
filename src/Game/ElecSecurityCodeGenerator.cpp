/* ElecSecurityCodeGenerator (class name from RTTI): the ELEC_SECURITY gadget of the electric circuit
 * area. It shows a model of SECURITY.BIN picked by its "type" parameter. Once a character comes
 * within its "range", it launches an ElecSecurityCode towards that character and updates the codes
 * it launched until they are gone; then it leaves the registry of security gadgets, and the last
 * one to leave sets its circuit's state to 1. File name inferred from the class. Method names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/DebugDraw.h"
#include "Game/ElecSecurityCode.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The registry of the stage's ELEC_SECURITY gadgets (name inferred): its constructor collects the
 * owners whose gadget type is typeId, keyed by their circuit id. Its unit and original name are
 * unknown. */
class UnkGadgetRegistry_80260E48 {
  public:
    UnkGadgetRegistry_80260E48(u16 typeId);
    ~UnkGadgetRegistry_80260E48();

    /* 0x0 */ u8 field_0x0[0x10];
};

extern "C" {
/* Adds owner to the registry, keyed by its circuit id (behaviour inferred). */
void fn_80260CE8(UnkGadgetRegistry_80260E48 *registry, UnkPlacement_800C9CD0 *owner);
/* Removes owner from the registry; true when the registry is then empty (behaviour inferred). */
bool fn_80260C18(UnkGadgetRegistry_80260E48 *registry, UnkPlacement_800C9CD0 *owner);
/* Copies params back into the owner's parameter block when its size equals size (behaviour
 * inferred; the counterpart of fn_800C9DBC). */
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, const void *params, int size);
/* Returns vector normalised (behaviour inferred). */
Vec3f fn_8040CEE8(const Vec3f &vector);
/* Converts a direction into two angles (behaviour inferred). */
Vec2f fn_8040F760(const Vec3f &direction);
/* Marks the gadget for removal (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
/* Acts on the character info that target holds; id selects what (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &target, int id);
}

/* The game state block in .bss; the word at 0xA8 is compared with 200 (meaning unknown). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0xA8];
    /* 0xA8 */ s32 field_0xA8;
    /* 0xAC */ u8 field_0xAC[0x150 - 0xAC];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern "C" UnkGameState_8057D6A0 lbl_8057D6A0;

/* a - b through fn_8040CD74 (name inferred). Retail calls it out of line (0x8000AD64, renamed to
 * this function's link name so the link keeps that copy); retail's copy builds the result in place,
 * which this body does not, so the body may differ from the original. */
inline Vec3f operator-(const Vec3f &a, const Vec3f &b) {
    Vec3f result;

    fn_8040CD74(&result, &a, &b);
    return result;
}

const char *ElecSecurityCodeGenerator::s_taskName = "ElecSecurityCodeGenerator";

/* Created by the load hook and destroyed by the unload hook. */
static UnkGadgetRegistry_80260E48 *lbl_805EF198;

UnkPtrListPosition_8001B674 ElecSecurityCodeList::EraseNode(UnkPtrListPosition_8001B674 where) {
    ElecSecurityCodeLink *node = where.node;

    where.node = where.node->next;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    fn_8001BDF4(this);
    fn_8001BDEC(this);
    delete node;
    --*fn_8001BDE8(this);
    return where;
}

ElecSecurityCodeList::ElecSecurityCodeList() {
    fn_8001BE04(this);
}

ElecSecurityCodeGenerator::ElecSecurityCodeGenerator(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CB88(0x57, 4, "ElecSecurityCodeGenerator");
    m_modelIndex = m_params.type;
    m_model = 0;
    ReadPlacement(m_owner);
    RebuildTransform();
    m_state = 0;
    fn_80260CE8(lbl_805EF198, owner);
}

ElecSecurityCodeList::~ElecSecurityCodeList() {
    fn_8001B5B8(this, 0);
}

/* The model is not deleted here, unlike in the other gadgets (as in retail). */
ElecSecurityCodeGenerator::~ElecSecurityCodeGenerator() {
    ElecSecurityCodeList::iterator it;

    for (it = m_codes.begin(); it != m_codes.end(); it++) {
        delete *it;
    }
    fn_80260C18(lbl_805EF198, m_owner);
}

ElecSecurityCodeList::iterator::iterator() {}

ElecSecurityCodeList::iterator ElecSecurityCodeList::begin() {
    return iterator(fn_8001B6B0(this));
}

ElecSecurityCode *&ElecSecurityCodeList::iterator::operator*() const {
    return static_cast<ElecSecurityCodeNode *>(m_node)->code;
}

ElecSecurityCodeList::iterator::iterator(const iterator &other) : m_node(other.m_node) {}

ElecSecurityCodeList::iterator::operator UnkPtrListPosition_8001B674() const {
    return UnkPtrListPosition_8001B674(m_node);
}

ElecSecurityCodeList::iterator ElecSecurityCodeList::erase(iterator where) {
    return iterator(EraseNode(where));
}

ElecSecurityCodeList::iterator ElecSecurityCodeList::iterator::operator++(int) {
    iterator old(*this);

    m_node = m_node->next;
    return old;
}

ElecSecurityCodeList::iterator::iterator(const UnkPtrListPosition_8001B674 &position)
    : m_node(position.node) {}

ElecSecurityCodeList::iterator ElecSecurityCodeList::end() {
    return iterator(fn_8001B674(this));
}

bool ElecSecurityCodeList::iterator::operator!=(const iterator &other) const {
    return m_node != other.m_node;
}

void ElecSecurityCodeGenerator::Behave(float dt) {
    switch (m_state) {
    case 0:
        if (IsWithinReach()) {
            boost::shared_ptr<CharInfo> nearest =
                fn_800A8A7C(fn_80074194(), &m_chara->m_position).lock();
            Vec2f angles =
                fn_8040F760(fn_8040CEE8(nearest->m_position - m_chara->m_position));

            m_codes.push_back(new ElecSecurityCode(m_modelIndex, m_chara.get(), this, angles, 0));
            m_state = 1;
        }
        break;
    case 1:
        if (fn_80058F00(&m_codes) == 0) {
            if (fn_80260C18(lbl_805EF198, m_owner)) {
                if (field_0x58 != 0) {
                    fn_801690D0(fn_800CB69C(), field_0x58, 1);
                }
            }
            if (field_0x3C.get() != 0) {
                if (lbl_8057D6A0.field_0xA8 == 200) {
                    fn_8020642C(field_0x3C, 0x31);
                } else {
                    fn_8020642C(field_0x3C, 0x32);
                }
            }
            fn_8015A794(this);
            m_flags |= 1;
        } else {
            ElecSecurityCodeList::iterator it;

            /* As in retail, the iterator also advances past the element after an erased one. */
            for (it = m_codes.begin(); it != m_codes.end(); it++) {
                if (!(*it)->Tick(dt)) {
                    delete *it;
                    it = m_codes.erase(it);
                }
            }
        }
        break;
    }
}

void ElecSecurityCodeGenerator::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    if (m_model != 0 && m_params.type != m_modelIndex) {
        m_model->fn_80162804();
        m_modelIndex = m_params.type;
        m_model->fn_80162A68(m_modelIndex, 0);
        m_params.type = m_modelIndex;
        fn_800C9D68(owner, &m_params, sizeof(m_params));
    }
}

void ElecSecurityCodeGenerator::fn_800CBC1C() {
    if (m_model == 0) {
        m_model = new GadgetModelInfo(1, "SECURITY.BIN");
        m_modelIndex = m_params.type;
        m_model->fn_80162A68(m_modelIndex, 0);
    }
    RebuildTransform();
    m_model->fn_80162EF4();
    fn_802E0E84(&m_chara->m_position, &lbl_805F06D8, m_params.range);
}

void ElecSecurityCodeGenerator::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E60(owner, &field_0x58);
}

void ElecSecurityCodeGenerator::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        m_model->fn_801633DC(&matrix);
    }
}

bool ElecSecurityCodeGenerator::IsWithinReach() {
    boost::shared_ptr<CharInfo> nearest =
        fn_800A8A7C(fn_80074194(), &m_chara->m_position).lock();

    if (nearest.get() != 0) {
        float reachSq = m_params.range * m_params.range;
        Vec3f offset;

        fn_8040CD74(&offset, &m_chara->m_position, &nearest->m_position);
        return fn_8040CF2C(&offset) <= reachSq;
    }

    return false;
}

extern "C" void fn_80157C2C(void) {
    fn_8006B474("SECURITY.BIN", 1);
    lbl_805EF198 = new UnkGadgetRegistry_80260E48(0x7D4);
}

extern "C" void fn_80157C04(void) {
    delete lbl_805EF198;
}

void ElecSecurityCodeGenerator::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecSecurityCodeGenerator(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_8052AA78[] = {
    {1, "type", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "range", 0, 0, 0, 50.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8052AAF0 = {
    "ELEC_SECURITY", fn_80157C2C, fn_80157C04, ElecSecurityCodeGenerator::Instantiate, 0, 0, 0x7D4, 1, 0,
    10, lbl_8052AA78,
};
