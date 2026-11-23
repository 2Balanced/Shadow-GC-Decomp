/* csdmSceneList.cpp (file name inferred from the class and its neighbours): Chao::CSD::CSceneList,
 * the list of scenes a project hands out (see Game/CsdProject.h), split from Game/csdmProject.cpp in
 * wave 41. Retail's sections prove a translation unit of its own: .rodata holds a second copy of
 * "tree::insert length error\n" and "Memory allocation failure", 8-byte aligned at 0x805132D8 after
 * csdmProject's copies and their padding, and the map's insert_unique flags (.data 0x8055FF80) and
 * code (0x8036F410..0x8036F96C) are reached only from here. The map's end() (0x8036F410) and the
 * tree's (0x8036F43C) come first: retail emitted them behind a function the linker dropped, which
 * UnusedEnd below stands for. Definitions are in reverse address order (deferred inlining), the
 * tree's template members follow at the end of the unit (<map_impl>).
 *
 * NonMatching (16 of 17 functions; the order matches): fn_8036F470 gives the key a stack slot below
 * the end() temporary where retail has it above, between the find() result and the end()
 * temporary. Tried (probes/Game_csdmProject*.py, waves 39-41): the lookup in inline helpers, the
 * iterator initialised, the key as a char *, u8 * or void * converted at the call, the map's find
 * taking its key by value, and copy propagation off with a named key (5 rows: the key then sits
 * above the find() result). Add calls the pair's converting constructor in csdmProject's code
 * (0x8036C804), as retail does, so that symbol is global in symbols.txt. */
#define CSD_OBJECT_LAYER
#define CSD_PROJECT
#define CSD_RCPTR_DEALLOC_CTOR
#define CSD_RCOBJECT_DTOR_DECL_ONLY
#define MSL_PAIR_CONVERTING_CONSTRUCTOR
#define MSL_TREE_CONST_ITERATOR_MIXED
#define MSL_TREE_ITERATOR_ASSIGN_CALL
#define MSL_TREE_END_NODE_NAME __tree_end_node_csdmSceneList
#define MSL_MULTIMAP
#define MSL_MAKE_PAIR_BY_VALUE
#include <iterator>
#include <functional>

#include "Game/CsdProject.h"

#pragma pool_data off
#pragma auto_inline off

namespace Chao {
namespace CSD {

void CSceneList::Add(const RCPtr<CScene> &scene) {
    m_scenes.insert(std::pair<const char *, RCPtr<CScene> >((const char *)scene->GetOwner(), scene));
}

/* The scene of list named name (game code's fn_8036F470). */
extern "C" RCPtr<CScene> fn_8036F470(CSceneList *list, const char *name) {
    CSceneList::SceneMap::const_iterator it;
    it = list->m_scenes.find(name);
    if (it != list->m_scenes.end()) {
        return (*it).second;
    }
    return RCPtr<CScene>(0);
}

/* Stands for the function retail emitted first and the linker dropped: it makes the map's and the
 * tree's end() come out ahead of fn_8036F470 (name and body invented; nothing calls it). */
extern "C" void UnusedEnd_csdmSceneList(CSceneList *list) {
    list->m_scenes.end();
}

} // namespace CSD
} // namespace Chao

#include <map_impl>
