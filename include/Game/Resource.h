#ifndef GAME_RESOURCE_H
#define GAME_RESOURCE_H

#include <dolphin/types.h>

/* Resource (class name from RTTI; vtable at 0x8051C66C, destructor 0x80012890): the interface of a
 * loadable resource file. Every function but the destructor is pure. Game/ResourceOneFileReq.cpp
 * implements it; ResourceOneFile (Game/ResourceOneFile.h) is the other implementation. A file
 * holds numbered entries; index 2 is the first one. All function names are inferred from what
 * ResourceOneFileReq's versions do. */
class Resource {
  public:
    Resource() {}
    virtual ~Resource() {}
    /* Loads the file (the first call does the work). */
    virtual void Load() = 0;
    /* Frees what Load loaded. */
    virtual bool Release() = 0;
    /* The load state (3 once loaded). */
    virtual int GetState() = 0;
    /* The data of an entry. */
    virtual void *GetData(int index) = 0;
    virtual void *GetData(const char *name) = 0;
    /* The hierarchy of a bone entry (.BON, .BOP). */
    virtual void *GetHierarchy(int index) = 0;
    virtual void *GetHierarchy(const char *name) = 0;
    /* The motion number of a motion entry (.MTN, .MTP). */
    virtual u32 GetMotion(int index) = 0;
    virtual u32 GetMotion(const char *name) = 0;
    virtual void *vf_0x30(int index) = 0;
    virtual void *vf_0x34(const char *name) = 0;
    /* The index of the entry with the given name, or -1. */
    virtual int GetIndex(const char *name) = 0;
    /* The size of an entry's file. */
    virtual u32 GetSize(int index) = 0;
    virtual u32 GetSize(const char *name) = 0;
};

#endif
