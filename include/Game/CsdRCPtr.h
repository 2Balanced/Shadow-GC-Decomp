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
};

extern "C" {
UnkCsdAllocator_803605AC *fn_803605AC(void);
/* Frees memory of the allocator (behaviour inferred). */
void fn_80360528(void *p);
/* Prints a CSD error message (behaviour inferred). */
void fn_80361D18(const char *format, ...);
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

/* CScene (class name from the RTTI of RCPtr<CScene>::RCObject<CScene>): a scene of a CSD project.
 * Only the virtual function at 0x20 and the word at 0x8C are used here. */
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

    /* Whether the scene's animation has ended (name inferred; fn_803659F0 sets the word to 1 and
     * fn_803659E4 clears it). */
    bool IsEnd() {
        return m_end != 0;
    }

    /* 0x04 */ u8 field_0x4[0x8C - 0x4];
    /* 0x8C */ s32 m_end;
};

/* CNode (class name from the RTTI of RCPtr<CNode>::RCObject<CNode>): a node of a scene. */
class CNode;

/* The tag that makes an RCObject delete its object (name inferred). */
struct RCDeleteTag {
    RCDeleteTag() {}
};

/* RCPtr (class name from RTTI): the CSD library's reference-counted pointer. */
template <class T>
class RCPtr {
  public:
    /* RCObject (class name from RTTI): the counted holder of the object. */
    template <class U>
    class RCObject {
      public:
        /* CFunctorBase (class name from RTTI): a deallocation functor (names inferred). */
        class CFunctorBase {
          public:
            virtual void operator()(U *p);
            virtual ~CFunctorBase();
        };

        template <class D>
        RCObject(U *object, D d) : m_object(object), m_count(0), m_functor(0), m_dealloc(-1) {
            SetDealloc(d);
        }
        /* 0x00 */ U *m_object;
        virtual ~RCObject();

        /* Makes LetGo delete the object (name inferred). */
        void SetDealloc(RCDeleteTag) {
            m_functor = 0;
            m_dealloc = 1;
        }

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

        static void *operator new(u32 size) {
            return fn_803605AC()->Alloc(size, "csdRCObject.h", 410);
        }

        /* 0x08 */ s32 m_count;
        /* 0x0C */ CFunctorBase *m_functor;
        /* 0x10 */ s32 m_dealloc;
    };

    RCPtr();
    RCPtr(T *object);
    RCPtr(const RCPtr &other);
    ~RCPtr();
    RCPtr &operator=(const RCPtr &other);

    RCPtr &operator=(T *object) {
        Set(object, RCDeleteTag());
        return *this;
    }

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
            m_object = new RCObject<T>(object, d);
            m_object->m_count++;
        }
    }

    T *Get() const {
        return m_object != 0 ? m_object->m_object : 0;
    }
    T *operator->() const;

    /* 0x0 */ RCObject<T> *m_object;
};

} // namespace CSD
} // namespace Chao

#endif
