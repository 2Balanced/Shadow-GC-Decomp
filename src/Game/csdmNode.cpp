/* csdmNode.cpp (file name from the allocator's records): CNode, a node of a scene: the pattern of
 * one of the scene's casts (made in a pool the node allocates), the node's font list, the
 * functions and observers it calls when it updates, and its links to other nodes. Its attribute
 * accessors convert between the layout's coordinates and the cast's. Class names other than CNode,
 * CResourceBase, RCPtr and CBase, and all member names, are inferred.
 *
 * Code generation follows Game/csdmScene.cpp: deferred inlining, so the functions are defined in
 * the reverse of their address order. With inlining one level deep, an inline call inside the
 * argument of an expanded inline call stays out of line, which is why the links go through the
 * small inline setters (and RCPtr's operator-> is called out of line there). The unit sees
 * RCObject's destructors declared only (CSD_RCOBJECT_DTOR_DECL_ONLY), as the game emits neither
 * them nor RCObject's vtables here.
 *
 * NonMatching: 96 of 97 functions match. ApplyLink (0x8036A44C) loads MakePatternFrame's float
 * argument before its two integer ones (3 instructions), and the out-of-line
 * CObserver<CNode>::RemoveSubject the destructor takes the address of comes out last instead of
 * before the tree's destroy (0x8036B9FC), which shifts the functions after it. */
#define CSD_OBJECT_LAYER
#define CSD_RCOBJECT_DTOR_DECL_ONLY
#define MSL_TREE_END_NODE_NAME __tree_end_node_csdmNode
#include <iterator>
#include <functional>

#include "Game/CsdScene.h"

#pragma pool_data off
#pragma auto_inline off

namespace Chao {
namespace CSD {

void ClearCastDone(SCast *cast);

template <class S, class O>
CSubject<S, O>::~CSubject() {}

CNode::CNode(SCastInfo *info, const RCPtr<unsigned char> *data, CMotionPalette *palette, const SNameIndex2 *entry,
             CScene *scene, CFontList *fontList)
    : m_fontList(0) {
    Set(info, *data);
    m_pool = (SNodePool *)fn_80360564(sizeof(SNodePool), "csdmNode.cpp", 0x33);
    m_pattern = palette->CreatePattern(m_pool->m_pattern, entry);
    m_pattern->m_aspect = scene->GetAspect();
    m_pattern->m_node = this;
    m_name = entry->m_name;
    m_scene = scene;
    if (fontList != 0) {
        m_fontList = new (m_pool->m_fontList) CFontList(fontList);
    }
}

CNode::~CNode() {
    Notify(&CNodeObserver::RemoveSubject, GetSubject());
    m_observers.clear();
    if (m_pattern != 0) {
        m_pattern->~CMotionPattern();
        m_pattern = 0;
    }
    if (m_fontList != 0) {
        m_fontList->~CFontList();
        m_fontList = 0;
    }
    fn_80360528(m_pool);
}

extern "C" void fn_8036AEE0(CNode *node, s32 value) {
    node->m_resource->field_0x8 = (void *)value;
}

extern "C" SVec2 fn_8036AE98(const CNode *node) {
    const u64 &value = node->m_pattern->GetPosition();
    SVec2 position = *(const SVec2 *)&value;
    FromLayout(&position.m_x, &position.m_y);
    return position;
}

extern "C" void fn_8036AE70(CNode *node, const SVec2 *position) {
    node->SetPosition(position->m_x, position->m_y);
}

void CNode::SetPosition(float x, float y) {
    ToLayout(&x, &y);
    m_pattern->OverridePosition(x, y);
}

void GetNodeAttribute(CValue *out, const CNode *node, s32 attr) {
    const SCastInfo *info = node->m_resource;
    if (info->m_attributes == GetAllAttributes()) {
        *out = info->m_values[attr];
    } else {
        GetAttribute(out, attr, info->m_attributes, info->m_values);
    }
}

s32 CNode::GetHide() const {
    return m_pattern->GetHide();
}

extern "C" void fn_8036AD78(CNode *node, s32 hide) {
    node->SetHide(hide);
}

float CNode::GetRotation() const {
    return m_pattern->GetRotation();
}

extern "C" void fn_8036AD14(CNode *node, s32 pattern) {
    node->SetPatternFrame(MakePatternFrame(0.0f, pattern, pattern));
}

u32 CNode::GetColor() const {
    return m_pattern->GetColor();
}

extern "C" void fn_8036ACCC(CNode *node, const u32 *color) {
    node->m_pattern->OverrideColor(*color);
}

extern "C" void fn_8036ACA8(CNode *node) {
    node->m_pattern->ClearColorOverride();
}

u32 CNode::GetCornerColor(s32 corner) const {
    return m_pattern->GetCornerColor(corner);
}

extern "C" void fn_8036AC60(CNode *node, s32 corner, const u32 *color) {
    node->m_pattern->OverrideCornerColor(corner, *color);
}

extern "C" void fn_8036AC3C(CNode *node, s32 corner) {
    node->m_pattern->ClearCornerColorOverride(corner);
}

s32 CNode::GetParam(s32 index) const {
    return m_pattern->GetParam(index);
}

void CNode::NotifyObservers() {
    Notify(&CNodeObserver::OnUpdate, this);
    FunctorList::iterator it = m_functors.begin();
    while (it != m_functors.end()) {
        if (!(*it->Get())(this)) {
            it = m_functors.erase(it);
        } else {
            ++it;
        }
    }
}

void CNode::SetFontList(const CFontList *fontList) {
    if (m_fontList != 0) {
        m_fontList->~CFontList();
        m_fontList = 0;
    }
    if (fontList != 0) {
        m_fontList = new (m_pool->m_fontList) CFontList(fontList);
    }
}

extern "C" void fn_8036A8BC(CNode *node) {
    node->m_pattern->ClearHideOverride();
}

/* (Not used by the game; the linker drops it.) */
void CNode::Unused(s32 attr, const RCPtr<CNode> &node) {
    LinkMap::value_type link(attr, node);
}

void CNode::Update() {
    LinkMap::iterator it;
    for (it = m_links.begin(); it != m_links.end(); ++it) {
        ApplyLink(it);
    }
}

void CNode::ApplyLink(LinkMap::iterator &link) {
    switch ((*link).first) {
    case 0:
        SetHide(link->second->GetHide());
        break;
    case 1:
        SetX((*link).second->GetX());
        break;
    case 2:
        SetY((*link).second->GetY());
        break;
    case 3:
        SetRotation(link->second->GetRotation());
        break;
    case 4:
        SetScaleX(link->second->GetScaleX());
        break;
    case 5:
        SetScaleY(link->second->GetScaleY());
        break;
    case 6:
        CopyPatternFrame(link->second->m_pattern->GetPattern());
        break;
    case 7:
        SetColor(link->second->GetColor());
        break;
    case 8:
        SetCornerColor(0, link->second->GetCornerColor(0));
        break;
    case 9:
        SetCornerColor(1, link->second->GetCornerColor(1));
        break;
    case 10:
        SetCornerColor(2, link->second->GetCornerColor(2));
        break;
    case 11:
        SetCornerColor(3, link->second->GetCornerColor(3));
        break;
    case 12:
        SetParam(0, link->second->GetParam(0));
        break;
    case 13:
        SetParam(1, link->second->GetParam(1));
        break;
    case 14:
        SetParam(2, link->second->GetParam(2));
        break;
    }
}

float CNode::GetX() const {
    const u64 &value = m_pattern->GetPosition();
    SVec2 position = *(const SVec2 *)&value;
    FromLayout(&position.m_x, &position.m_y);
    return position.m_x;
}

void CNode::SetX(float x) {
    float lx = x;
    float ly = 0.0f;
    ToLayout(&lx, &ly);
    m_pattern->OverrideX(lx);
}

float CNode::GetY() const {
    const u64 &value = m_pattern->GetPosition();
    SVec2 position = *(const SVec2 *)&value;
    FromLayout(&position.m_x, &position.m_y);
    return position.m_y;
}

void CNode::SetY(float y) {
    float x = 0.0f;
    ToLayout(&x, &y);
    m_pattern->OverrideY(y);
}

float CNode::GetScaleX() const {
    union {
        u64 m_value;
        f32 m_floats[2];
    } scale;
    scale.m_value = m_pattern->GetScale();
    return scale.m_floats[0];
}

extern "C" void fn_8036A2CC(CNode *node, float scale) {
    node->SetScaleX(scale);
}

float CNode::GetScaleY() const {
    union {
        u64 m_value;
        f32 m_floats[2];
    } scale;
    scale.m_value = m_pattern->GetScale();
    return scale.m_floats[1];
}

extern "C" void fn_8036A278(CNode *node, float scale) {
    node->SetScaleY(scale);
}

/* Rebuilds the scene's run-time state with the cast's own matrix mode, and has the pattern placed
 * again. */
void CNode::Refresh() {
    u32 mode = m_resource->m_mode;
    m_resource->m_mode = mode & ~0xC00;
    m_scene->RebuildState();
    m_resource->m_mode = mode;
    SCast *cast = m_pattern->m_resource;
    cast->m_changed &= ~0x200000;
    cast->m_changed &= ~0x80000;
    ClearCastDone(cast);
}

CNode *CNode::GetSubject() {
    return this;
}

} // namespace CSD
} // namespace Chao

#include <map_impl>
