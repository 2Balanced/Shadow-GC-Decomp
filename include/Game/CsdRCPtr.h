#ifndef GAME_CSDRCPTR_H
#define GAME_CSDRCPTR_H

/* The parts of the Chao CSD 2D library (the game's csd*.h headers) that Game/EggLastMechSlotDisp2D.cpp
 * reconstructs: the memory allocator, scenes and nodes, and the reference-counted pointer RCPtr
 * (class names from RTTI: Chao::CSD::RCPtr<T>::RCObject<T>, its CFunctorBase and Chao::CSD::CBase).
 * The source file names and line numbers are the ones retail passes to the allocator (csdBase.h,
 * csdRCObject.h). Member names are inferred. Game/Gong.cpp keeps its own copy of the RCPtr
 * declaration and of UnkCsdProject_800BFF30. */

#include <dolphin/types.h>

/* The CSD memory allocator fn_803605AC returns (name inferred). Slot 0x14 allocates size bytes,
 * recording the source file and line. */
class UnkCsdAllocator_803605AC {
  public:
    virtual void vf_0x08();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void *Alloc(u32 size, const char *file, int line);
    /* Frees memory from Alloc (name inferred; the class operator delete of csdBase.h). */
    virtual void Free(void *p);
    /* Allocates size bytes for the library's own records (name inferred; fn_80360564). */
    virtual void *AllocDebug(u32 size, const char *file, int line);
    /* Frees memory from AllocDebug (name inferred; fn_80360528). */
    virtual void FreeDebug(void *p);
};

extern "C" {
UnkCsdAllocator_803605AC *fn_803605AC(void);
/* Frees memory of the allocator (behaviour inferred). */
void fn_80360528(void *p);
/* Prints a CSD error message (behaviour inferred). */
void fn_80361D18(const char *format, ...);
#ifdef CSD_OBJECT_LAYER
/* Prints a debug message (behaviour inferred; also declared in Game/CsdLib.h). */
void fn_80361DC4(const char *format, ...);
#endif
}

/* The 0x4C-byte CSD project (name inferred); fn_800BFF30 constructs it from a loaded resource. Its
 * allocation goes through the CSD allocator (the class operator new of csdBase.h, line 40). */
class UnkCsdProject_800BFF30 {
  public:
    UnkCsdProject_800BFF30(void *resource);
    virtual ~UnkCsdProject_800BFF30();

    /* 0x04 */ u8 field_0x4[0x4C - 0x4];

    static void *operator new(u32 size) {
        return fn_803605AC()->Alloc(size, "csdBase.h", 40);
    }
};

namespace Chao {
namespace CSD {

/* CBase (class name from RTTI): the empty base of the library's classes. The RTTI records of
 * RCObject<T> and its CFunctorBase list it as their base (see Game/AdvSynopsis.cpp). Its class
 * operators new and delete (csdBase.h) are declared only where CSD_OBJECT_LAYER is defined (the
 * library's object layer, Game/csdmScene.cpp and the units after it), so that other units are
 * unchanged. */
class CBase {
#ifdef CSD_OBJECT_LAYER
  public:
    static void *operator new(u32 size) {
        return fn_803605AC()->Alloc(size, "csdBase.h", 40);
    }
    static void *operator new(u32 size, const char *file, int line) {
        return fn_803605AC()->Alloc(size, file, line);
    }
    static void *operator new(u32 size, void *p) {
        return p;
    }
    static void operator delete(void *p) {
        fn_803605AC()->Free(p);
    }
#endif
};

#ifdef CSD_OBJECT_LAYER
/* The library's allocation with the source file and line it is made from (csdRCObject.h). */
#define CSD_RCOBJECT_NEW(line) new ("csdRCObject.h", line)
#else
#define CSD_RCOBJECT_NEW(line) new
#endif

/* CScene (class name from the RTTI of RCPtr<CScene>::RCObject<CScene>): a scene of a CSD project.
 * Only the virtual function at 0x20 and the word at 0x8C are used here. */
#ifdef CSD_OBJECT_LAYER
/* The library's own declaration is in Game/CsdScene.h. */
class CScene;
#else
class CScene {
  public:
    virtual ~CScene();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void vf_0x14();
    virtual void vf_0x18();
    virtual void vf_0x1C();
    /* Advances the scene by dt (name inferred). */
    virtual void Tick(float dt);
    /* Hands the scene a project's texture list, an RCPtr<CTexList> (name inferred; see
     * Game/AdvCsdEffect.cpp). */
    virtual void SetTexList(const void *texList);

    /* Whether the scene's animation has ended (name inferred; fn_803659F0 sets the word to 1 and
     * fn_803659E4 clears it). */
    bool IsEnd() {
        return m_end != 0;
    }

    /* 0x04 */ u8 field_0x4[0x8C - 0x4];
    /* 0x8C */ s32 m_end;
};
#endif

/* CNode (class name from the RTTI of RCPtr<CNode>::RCObject<CNode>): a node of a scene. */
class CNode;

#ifdef CSD_OBJECT_LAYER
/* A function an RCObject calls to free its object (name inferred). */
template <class U>
struct RCFreeFunc {
    RCFreeFunc(void (*func)(U *)) : m_func(func) {}

    /* 0x0 */ void (*m_func)(U *);
};

/* The tag that makes an RCObject free its object with the library allocator (name inferred). */
struct RCFreeTag {};
#endif

/* The tag that makes an RCObject delete its object (name inferred). */
struct RCDeleteTag {
#ifndef CSD_OBJECT_LAYER
    RCDeleteTag() {}
#endif
};

/* RCPtr (class name from RTTI): the CSD library's reference-counted pointer. */
#ifdef CSD_OBJECT_LAYER
/* Does nothing (an inline call that keeps its caller out of line). */
inline void __csd_object_nothing() {}
#endif

template <class T>
class RCPtr
#ifdef CSD_OBJECT_LAYER
    : public CBase
#endif
{
  public:
    /* RCObject (class name from RTTI): the counted holder of the object. */
    template <class U>
    class RCObject : public CBase {
      public:
        /* CFunctorBase (class name from RTTI): a deallocation functor (names inferred). */
        class CFunctorBase : public CBase {
          public:
#ifdef CSD_OBJECT_LAYER
            virtual void operator()(U *p) = 0;
#ifdef CSD_RCOBJECT_DTOR_DECL_ONLY
            virtual ~CFunctorBase();
#else
            virtual ~CFunctorBase() {}
#endif
#else
            virtual void operator()(U *p);
            virtual ~CFunctorBase();
#endif
        };

        template <class D>
        RCObject(U *object, D d) : m_object(object), m_count(0), m_functor(0), m_dealloc(-1) {
            SetDealloc(d);
        }
#ifdef CSD_OBJECT_LAYER
        /* Holds object with its deallocation left unset (Set(T *) makes it; name inferred). */
        RCObject(U *object) : m_object(object), m_count(0), m_functor(0) {
            __csd_object_nothing();
        }
#endif
        /* 0x00 */ U *m_object;
#ifdef CSD_OBJECT_LAYER
#ifdef CSD_RCOBJECT_DTOR_DECL_ONLY
        /* (Where CSD_RCOBJECT_DTOR_DECL_ONLY is defined, Game/csdmNode.cpp, the unit does not see the
         * destructors' definitions, so it emits neither them nor the vtables.) */
        virtual ~RCObject();
#else
        virtual ~RCObject() {}
#endif
#else
        virtual ~RCObject();
#endif

        /* Makes LetGo delete the object (name inferred). */
        void SetDealloc(RCDeleteTag) {
            m_functor = 0;
            m_dealloc = 1;
        }
#ifdef CSD_OBJECT_LAYER
        /* Makes LetGo free the object with the library allocator (name inferred). */
        void SetDealloc(RCFreeTag) {
            m_functor = 0;
            m_dealloc = 0;
        }
        /* Makes LetGo call func (name inferred). */
        void SetDealloc(RCFreeFunc<U> func) {
            m_functor = (CFunctorBase *)func.m_func;
            m_dealloc = 3;
            __csd_object_nothing();
        }
#endif

        /* Deletes the object as m_dealloc says (name inferred). */
        void LetGo() {
            if (m_object != 0) {
                switch (m_dealloc) {
                case 0:
                    if (m_object != 0) {
                        fn_80360528(m_object);
                        m_object = 0;
                    }
                    break;
                case 1:
                    if (m_object != 0) {
                        delete m_object;
                        m_object = 0;
                    }
                    break;
                case 2:
                    break;
                case 3:
                    ((void (*)(U *))m_functor)(m_object);
                    m_functor = 0;
                    break;
                case 4:
                    if (m_functor != 0) {
                        (*m_functor)(m_object);
                    }
                    if (m_functor != 0) {
                        delete m_functor;
                        m_functor = 0;
                    }
                    break;
                default:
                    fn_80361D18("Not inititalized m_eDealloctor\n");
                    break;
                }
            }
        }

#ifndef CSD_OBJECT_LAYER
        static void *operator new(u32 size) {
            return fn_803605AC()->Alloc(size, "csdRCObject.h", 410);
        }
#endif

        /* 0x08 */ s32 m_count;
        /* 0x0C */ CFunctorBase *m_functor;
        /* 0x10 */ s32 m_dealloc;
    };

#ifdef CSD_OBJECT_LAYER
    /* The library's inline members (csdRCObject.h); the game links one copy of each. */
    RCPtr() : m_object(0) {
        Set((T *)0);
    }
    RCPtr(T *object) : m_object(0) {
        RCDeleteTag tag;
        Set(object, tag);
    }
    RCPtr(const RCPtr &other) : m_object(0) {
        Assign(other);
    }
    ~RCPtr() {
        RCObject<T> *current = m_object;
        if (current != 0) {
            if (--current->m_count == 0) {
                current->LetGo();
                delete current;
            }
            m_object = 0;
        }
    }
    RCPtr &operator=(const RCPtr &other) {
        Assign(other);
        return *this;
    }
    /* Shares other's object (name inferred). */
    void Assign(const RCPtr &other) {
        RCObject<T> *current = m_object;
        if (current != 0) {
            if (other.m_object != 0 && current->m_object == other.m_object->m_object) {
                return;
            }
            if (--current->m_count == 0) {
                current->LetGo();
                delete current;
            }
            m_object = 0;
        }
        if (other.m_object != 0) {
            m_object = other.m_object;
            m_object->m_count++;
        }
    }
    /* Takes object with its deallocation left unset (name inferred). */
    void Set(T *object) {
        RCObject<T> *current = m_object;
        if (current != 0) {
            if (current->m_object == object) {
                return;
            }
            if (--current->m_count == 0) {
                current->LetGo();
                delete current;
            }
            m_object = 0;
        }
        if (object != 0) {
            m_object = CSD_RCOBJECT_NEW(428) RCObject<T>(object);
            m_object->m_count++;
        }
    }
#else
    RCPtr();
    RCPtr(T *object);
#endif
#ifdef CSD_OBJECT_LAYER
    /* Takes object with func to free it (name inferred). */
    RCPtr(T *object, const RCFreeFunc<T> &func) : m_object(0) {
        Set(object, func);
    }
#endif
#ifdef CSD_RCPTR_DEALLOC_CTOR
    /* Takes object with the deallocation d says (name inferred). Declared only where a unit needs
     * it (Game/csdTexListRW.cpp), so that other units' template declarations are unchanged. */
    template <class D>
    RCPtr(T *object, D d) : m_object(0) {
        Set(object, d);
    }
#endif
#ifndef CSD_OBJECT_LAYER
    RCPtr(const RCPtr &other);
    ~RCPtr();
    RCPtr &operator=(const RCPtr &other);
#endif

#ifdef CSD_OBJECT_LAYER
    RCPtr &operator=(T *object) {
        RCDeleteTag tag;
        Set(object, tag);
        return *this;
    }
#else
    RCPtr &operator=(T *object) {
        Set(object, RCDeleteTag());
        return *this;
    }
#endif

#ifdef CSD_OBJECT_LAYER
    /* Takes object with the deallocation d says (name inferred). */
    template <class D>
    void Reset(T *object, const D &d) {
        Set(object, d);
    }
    /* Takes object with the deallocation d says, d passed by value (name inferred). */
    template <class D>
    void Attach(T *object, D d) {
        Set(object, d);
    }
#endif

    template <class D>
    void Set(T *object, const D &d) {
        RCObject<T> *current = m_object;
        if (current != 0) {
            if (current->m_object == object) {
                return;
            }
            if (--current->m_count == 0) {
                current->LetGo();
                delete current;
            }
            m_object = 0;
        }
        if (object != 0) {
            m_object = CSD_RCOBJECT_NEW(410) RCObject<T>(object, d);
            m_object->m_count++;
        }
    }

    T *Get() const {
        return m_object != 0 ? m_object->m_object : 0;
    }
#ifdef CSD_OBJECT_LAYER
    T *operator->() const {
        return m_object != 0 ? m_object->m_object : 0;
    }
    /* The object, which must be there (the library stops with an error otherwise). */
    T &operator*() const {
        if (m_object != 0) {
            return *m_object->m_object;
        }
        for (;;) {
            fn_80361DC4("CSD Error:Reference to NULL\n");
        }
    }
#else
    T *operator->() const;
#endif

    /* 0x0 */ RCObject<T> *m_object;
};

} // namespace CSD
} // namespace Chao

#endif
