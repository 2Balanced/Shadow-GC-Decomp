/* The PowerPC (Gekko) half of the CodeWarrior C++ exception runtime: finding the exception table
 * entry of a return address, walking a function's list of exception actions, unwinding stack
 * frames, and the entry points the compiler calls (__throw, __end__catch, __unexpected) or the
 * start-up code calls (__register_fragment, __unregister_fragment).
 *
 * The table layouts below are what the code reads; apart from the entry points, the names are
 * inferred. An exception table index entry (extabindex) gives a function's offset, its size and
 * the offset of its exception record (extab). A record starts with a 16-bit word describing the
 * function's frame, followed by a list of (code range, action list) entries and the action lists
 * themselves. An action is a byte code (the top bit ends the list) with operands.
 *
 * The file is built with deferred inlining, so the functions come out in reverse definition
 * order. */
#include "Runtime.PPCEABI.H/MWCPlusLib.h"
#include "Runtime.PPCEABI.H/NMWException.h"
#include "Runtime.PPCEABI.H/exception.h"

#define MAXFRAGMENTS 1

/* The frame word of an exception record. */
#define ET_SAVED_GPRS(field) ((field) >> 11)
#define ET_SAVED_FPRS(field) (((field) >> 6) & 0x1F)
#define ET_HAS_FRAME_POINTER(field) (((field) >> 4) & 1) /* locals are addressed from r31 */
#define ET_LARGE_RANGES(field) (((field) >> 3) & 1)      /* the range table has 32-bit starts */
#define ET_PAIRED_FPRS(field) (((field) >> 1) & 1)       /* 16-byte paired-single FPR slots */

typedef unsigned char exaction_type;

#define EXACTION_ENDBIT 0x80
#define EXACTION_MASK 0x7F

enum {
    EXACTION_ENDOFLIST = 0,
    EXACTION_BRANCH = 1,
    EXACTION_DESTROYLOCAL = 2,
    EXACTION_DESTROYLOCALCOND = 3,
    EXACTION_DESTROYLOCALPOINTER = 4,
    EXACTION_DESTROYLOCALARRAY = 5,
    EXACTION_DESTROYBASE = 6,
    EXACTION_DESTROYMEMBER = 7,
    EXACTION_DESTROYMEMBERCOND = 8,
    EXACTION_DESTROYMEMBERARRAY = 9,
    EXACTION_DELETEPOINTER = 10,
    EXACTION_DELETEPOINTERCOND = 11,
    EXACTION_CATCHBLOCK = 12,
    EXACTION_ACTIVECATCHBLOCK = 13,
    EXACTION_TERMINATE = 14,
    EXACTION_SPECIFICATION = 15,
    EXACTION_CATCHBLOCK_32 = 16
};

/* The operands of the actions. A field byte's top bit (and bit 6 for a second operand) says the
 * operand is a register number rather than a frame offset. */
struct ex_branch {
    exaction_type action;
    unsigned char unused;
    unsigned short target; /* offset of the next action in the record */
};

struct ex_destroylocal {
    exaction_type action;
    unsigned char unused;
    short local;
    void *dtor;
};

struct ex_destroylocalcond {
    exaction_type action;
    unsigned char dlc_field;
    short cond;
    short local;
    void *dtor;
};

struct ex_destroylocalpointer {
    exaction_type action;
    unsigned char dlp_field;
    short pointer;
    void *dtor;
};

struct ex_destroylocalarray {
    exaction_type action;
    unsigned char unused;
    short localarray;
    unsigned short elements;
    unsigned short element_size;
    void *dtor;
};

struct ex_destroymember {
    exaction_type action;
    unsigned char dm_field;
    short objectptr;
    long offset;
    void *dtor;
};

struct ex_destroymembercond {
    exaction_type action;
    unsigned char dmc_field;
    short cond;
    short objectptr;
    long offset;
    void *dtor;
};

struct ex_destroymemberarray {
    exaction_type action;
    unsigned char dma_field;
    short objectptr;
    long offset;
    long elements;
    long element_size;
    void *dtor;
};

struct ex_deletepointer {
    exaction_type action;
    unsigned char dp_field;
    short objectptr;
    void *deletefunc;
};

struct ex_deletepointercond {
    exaction_type action;
    unsigned char dpc_field;
    short cond;
    short objectptr;
    void *deletefunc;
};

struct ex_catchblock {
    exaction_type action;
    unsigned char unused;
    char *catch_type;
    unsigned short catch_pcoffset;
    short cinfo_ref;
};

struct ex_catchblock_32 {
    exaction_type action;
    unsigned char unused;
    char *catch_type;
    long catch_pcoffset;
    long cinfo_ref;
};

struct ex_activecatchblock {
    exaction_type action;
    unsigned char unused;
    short cinfo_ref;
};

struct ex_specification {
    exaction_type action;
    unsigned char unused;
    unsigned short specs;
    long pcoffset;
    long cinfo_ref;
    char *spec[1];
};

/* An exception table index entry. The size's top bit says the third word is the record itself
 * rather than its offset. */
struct ExceptionTableIndex {
    unsigned long functionoffset;
    unsigned long eti_field;
    unsigned long exceptionoffset;
};

/* The range tables of a record: 16-bit (start, end, action offset) or 32-bit start with a size
 * in words. */
struct ExceptionRangeSmall {
    unsigned short start;
    unsigned short end;
    unsigned short action;
};

struct ExceptionRangeLarge {
    unsigned long start;
    unsigned short size;
    unsigned short action;
};

/* A registered module. */
struct ProcessInfo {
    ExceptionTableRange *exception_info;
    char *TOC;
    int active;
};

struct FragmentInfo {
    ExceptionTableIndex *exception_start;
    ExceptionTableIndex *exception_end;
    char *code_start;
    char *code_end;
    char *data_start;
    char *data_end;
    char *TOC;
    int active;
};

/* Where a return address stands in the tables. */
struct MWExceptionInfo {
    char *exception_record;
    char *current_function;
    char *action_pointer;
    char *code_section;
    char *data_section;
    char *TOC;
};

struct ActionIterator {
    MWExceptionInfo info;
    char *current_SP;
    char *current_FP;
    long current_R31;
};

/* What a catch block keeps of the exception it handles (the compiler reserves one in the
 * function's frame). */
struct CatchInfo {
    void *location;    /* the thrown object */
    void *typeinfo;    /* its type string */
    void *dtor;        /* its destructor */
    void *sublocation; /* the caught sub-object, or pointercopy for a pointer */
    long pointercopy;
    void *stacktop;
};

/* A floating-point register as __throw saves it: the double and the paired-single view. */
struct FPRContext {
    double d;
    float ps0;
    float ps1;
};

/* The registers and state of a throw in progress (built by __throw in its own frame). */
struct ThrowContext {
    FPRContext FPR[32];
    long GPR[32];
    long CR;
    char *SP;
    char *FP;
    char *throwSP;
    char *returnaddr;
    char *throwtype;
    void *location;
    void *dtor;
    CatchInfo *catchinfo;
};

static ProcessInfo fragmentinfo[MAXFRAGMENTS];

static asm void ExPPC_LongJump(register ThrowContext *context, register void *newRTOC,
                               register void *newPC);

/* Registers a module's exception tables; returns its handle, or -1 when no slot is free. */
extern "C" int __register_fragment(ExceptionTableRange *info, char *TOC) {
    ProcessInfo *f;
    int i;

    for (i = 0, f = fragmentinfo; i < MAXFRAGMENTS; ++i, ++f) {
        if (f->active == 0) {
            f->exception_info = info;
            f->TOC = TOC;
            f->active = 1;
            return i;
        }
    }
    return -1;
}

extern "C" void __unregister_fragment(int fragmentID) {
    ProcessInfo *f;

    if (fragmentID >= 0 && fragmentID < MAXFRAGMENTS) {
        f = &fragmentinfo[fragmentID];
        f->exception_info = 0;
        f->TOC = 0;
        f->active = 0;
    }
}

/* Finds the registered module whose code holds returnaddr. */
static int ExPPC_FindExceptionFragment(char *returnaddr, FragmentInfo *frag) {
    ProcessInfo *f;
    int i;
    ExceptionTableRange *p;

    for (i = 0, f = fragmentinfo; i < MAXFRAGMENTS; ++i, ++f) {
        if (f->active) {
            p = f->exception_info;
            while (1) {
                if (p->textLen == 0) {
                    break;
                }
                if (returnaddr >= (char *)p->textBegin &&
                    returnaddr < (char *)p->textBegin + p->textLen) {
                    frag->exception_start = (ExceptionTableIndex *)p->indexBegin;
                    frag->exception_end = (ExceptionTableIndex *)p->indexEnd;
                    frag->code_start = 0;
                    frag->code_end = 0;
                    frag->data_start = 0;
                    frag->data_end = 0;
                    frag->TOC = f->TOC;
                    frag->active = f->active;
                    return 1;
                }
                p++;
            }
        }
    }
    return 0;
}

/* Fills info with the exception record of the function returnaddr is in and the action list in
 * force at that address; exception_record stays null if there is none. */
static void ExPPC_FindExceptionRecord(char *returnaddr, MWExceptionInfo *info) {
    FragmentInfo fragment;
    FragmentInfo *frag = &fragment;
    ExceptionTableIndex *exceptionindex, *p;
    unsigned long returnoffset;
    long i, m, n;

    info->exception_record = 0;
    info->action_pointer = 0;

    if (!ExPPC_FindExceptionFragment(returnaddr, frag)) {
        return;
    }

    returnoffset = returnaddr - frag->code_start;
    info->code_section = frag->code_start;
    info->data_section = frag->data_start;
    info->TOC = frag->TOC;

    exceptionindex = frag->exception_start;
    for (i = 0, n = frag->exception_end - frag->exception_start;;) {
        if (i > n) {
            return;
        }
        p = &exceptionindex[m = (i + n) / 2];
        if (returnoffset < p->functionoffset) {
            n = m - 1;
        } else if (returnoffset > p->functionoffset + (p->eti_field & 0x7FFFFFFF)) {
            i = m + 1;
        } else {
            break;
        }
    }
    info->current_function = frag->code_start + p->functionoffset;
    info->exception_record = (p->eti_field >> 31) ? (char *)&p->exceptionoffset
                                                  : frag->data_start + p->exceptionoffset;

    returnoffset -= p->functionoffset;

    if ET_LARGE_RANGES(*(unsigned short *)info->exception_record) {
        ExceptionRangeLarge *range;

        for (range = (ExceptionRangeLarge *)(info->exception_record + 4); range->start != 0;
             range++) {
            unsigned long start = range->start;
            unsigned long end = start + range->size * 4;

            if (start <= returnoffset && end >= returnoffset) {
                info->action_pointer = info->exception_record + range->action;
                break;
            }
        }
    } else {
        ExceptionRangeSmall *range;

        for (range = (ExceptionRangeSmall *)(info->exception_record + 2); range->start != 0;
             range++) {
            if (range->start <= returnoffset && range->end >= returnoffset) {
                info->action_pointer = info->exception_record + range->action;
                break;
            }
        }
    }
}

static inline exaction_type ExPPC_CurrentAction(const ActionIterator *iter) {
    if (iter->info.action_pointer == 0) {
        return EXACTION_ENDOFLIST;
    }
    return ((ex_destroylocal *)iter->info.action_pointer)->action & EXACTION_MASK;
}

/* Steps to the next action, moving to the caller's frame at the end of a list. */
static exaction_type ExPPC_NextAction(ActionIterator *iter) {
    exaction_type action;

    for (;;) {
        if (iter->info.action_pointer == 0 ||
            ((action = ((ex_destroylocal *)iter->info.action_pointer)->action) &
             EXACTION_ENDBIT) != 0) {
            char *return_addr, *callers_SP;

            callers_SP = *(char **)iter->current_SP;

            if (ET_SAVED_GPRS(*(unsigned short *)iter->info.exception_record)) {
                /* The caller's r31 is the last word of the GPR save area, below the FPRs. */
                iter->current_R31 =
                    *(long *)(callers_SP -
                              ET_SAVED_FPRS(*(unsigned short *)iter->info.exception_record) * 8 -
                              4);
            }

            return_addr = *(char **)(callers_SP + 4);

            ExPPC_FindExceptionRecord(return_addr, &iter->info);
            if (iter->info.exception_record == 0) {
                std::terminate();
            }

            iter->current_SP = callers_SP;
            iter->current_FP =
                ET_HAS_FRAME_POINTER(*(unsigned short *)iter->info.exception_record)
                    ? (char *)iter->current_R31
                    : iter->current_SP;

            if (iter->info.action_pointer == 0) {
                continue;
            }
        } else {
            switch (action) {
            case EXACTION_DESTROYLOCAL:
                iter->info.action_pointer += sizeof(ex_destroylocal);
                break;
            case EXACTION_DESTROYLOCALCOND:
                iter->info.action_pointer += sizeof(ex_destroylocalcond);
                break;
            case EXACTION_DESTROYLOCALPOINTER:
                iter->info.action_pointer += sizeof(ex_destroylocalpointer);
                break;
            case EXACTION_DESTROYLOCALARRAY:
                iter->info.action_pointer += sizeof(ex_destroylocalarray);
                break;
            case EXACTION_DESTROYBASE:
            case EXACTION_DESTROYMEMBER:
                iter->info.action_pointer += sizeof(ex_destroymember);
                break;
            case EXACTION_DESTROYMEMBERCOND:
                iter->info.action_pointer += sizeof(ex_destroymembercond);
                break;
            case EXACTION_DESTROYMEMBERARRAY:
                iter->info.action_pointer += sizeof(ex_destroymemberarray);
                break;
            case EXACTION_DELETEPOINTER:
                iter->info.action_pointer += sizeof(ex_deletepointer);
                break;
            case EXACTION_DELETEPOINTERCOND:
                iter->info.action_pointer += sizeof(ex_deletepointercond);
                break;
            case EXACTION_CATCHBLOCK:
                iter->info.action_pointer += sizeof(ex_catchblock);
                break;
            case EXACTION_CATCHBLOCK_32:
                iter->info.action_pointer += sizeof(ex_catchblock_32);
                break;
            case EXACTION_ACTIVECATCHBLOCK:
                iter->info.action_pointer += sizeof(ex_activecatchblock);
                break;
            case EXACTION_SPECIFICATION:
                iter->info.action_pointer +=
                    sizeof(ex_specification) - sizeof(char *) +
                    ((ex_specification *)iter->info.action_pointer)->specs * sizeof(char *);
                break;
            default:
                std::terminate();
                break;
            }
        }
        break;
    }

    action = ((ex_destroylocal *)iter->info.action_pointer)->action & EXACTION_MASK;
    if (action == EXACTION_BRANCH) {
        iter->info.action_pointer =
            iter->info.exception_record + ((ex_branch *)iter->info.action_pointer)->target;
        action = ((ex_destroylocal *)iter->info.action_pointer)->action & EXACTION_MASK;
    }
    return action;
}

/* Restores into context the registers the current function saved in its frame and steps
 * context to the caller's frame. Returns the address the function returns to. */
static char *ExPPC_PopStackFrame(ThrowContext *context, MWExceptionInfo *info) {
    char *SP, *callers_SP;
    int saved_GPRs, saved_FPRs;
    FPRContext *Vector_FPR_save_area;
    double *FPR_save_area;
    long *GPR_save_area;
    int i, j;

    SP = context->SP;
    callers_SP = *(char **)SP;
    saved_FPRs = ET_SAVED_FPRS(*(unsigned short *)info->exception_record);

    if ET_PAIRED_FPRS(*(unsigned short *)info->exception_record) {
        Vector_FPR_save_area = (FPRContext *)(callers_SP - saved_FPRs * 16);
        FPR_save_area = (double *)Vector_FPR_save_area;
    } else {
        FPR_save_area = (double *)(callers_SP - saved_FPRs * 8);
    }

    if ET_PAIRED_FPRS(*(unsigned short *)info->exception_record) {
        for (i = 32 - saved_FPRs, j = 0; i < 32; ++i, ++j) {
            context->FPR[i].ps0 = Vector_FPR_save_area[j].ps0;
            context->FPR[i].ps1 = Vector_FPR_save_area[j].ps1;
            context->FPR[i].d = Vector_FPR_save_area[j].d;
        }
    } else {
        for (i = 32 - saved_FPRs, j = 0; i < 32; ++i, ++j) {
            context->FPR[i].d = FPR_save_area[j];
        }
    }

    saved_GPRs = ET_SAVED_GPRS(*(unsigned short *)info->exception_record);
    GPR_save_area = (long *)FPR_save_area;
    GPR_save_area -= saved_GPRs;
    for (i = 32 - saved_GPRs, j = 0; i < 32; ++i, ++j) {
        context->GPR[i] = GPR_save_area[j];
    }

    context->SP = callers_SP;
    return *(char **)(callers_SP + 4);
}

#define DTORCALL_COMPLETE(dtor, object) (((void (*)(void *, short))(dtor))((object), -1))
#define DTORCALL_PARTIAL(dtor, object) (((void (*)(void *, short))(dtor))((object), 0))

/* Runs the cleanup actions of every frame up to the catch block or exception specification
 * `catcher`, restoring each frame's saved registers on the way. An exception that escapes a
 * destructor called here terminates the program. */
static void ExPPC_UnwindStack(ThrowContext *context, MWExceptionInfo *info, void *catcher) {
    exaction_type action;

#pragma exception_terminate

    for (;;) {
        if (info->action_pointer == 0) {
            char *return_addr;

            return_addr = ExPPC_PopStackFrame(context, info);
            ExPPC_FindExceptionRecord(return_addr, info);
            if (info->exception_record == 0) {
                std::terminate();
            }
            context->FP = ET_HAS_FRAME_POINTER(*(unsigned short *)info->exception_record)
                              ? (char *)context->GPR[31]
                              : context->SP;
            continue;
        }

        action = ((ex_destroylocal *)info->action_pointer)->action;
        switch (action & EXACTION_MASK) {
        case EXACTION_BRANCH:
            info->action_pointer =
                info->exception_record + ((ex_branch *)info->action_pointer)->target;
            break;

        case EXACTION_DESTROYLOCAL: {
            ex_destroylocal *ex = (ex_destroylocal *)info->action_pointer;

            DTORCALL_COMPLETE(ex->dtor, context->FP + ex->local);
            info->action_pointer += sizeof(ex_destroylocal);
        } break;

        case EXACTION_DESTROYLOCALCOND: {
            ex_destroylocalcond *ex = (ex_destroylocalcond *)info->action_pointer;

            if ((ex->dlc_field >> 7) ? (char)context->GPR[ex->cond]
                                     : *(char *)(context->FP + ex->cond)) {
                DTORCALL_COMPLETE(ex->dtor, context->FP + ex->local);
            }
            info->action_pointer += sizeof(ex_destroylocalcond);
        } break;

        case EXACTION_DESTROYLOCALPOINTER: {
            ex_destroylocalpointer *ex = (ex_destroylocalpointer *)info->action_pointer;

            DTORCALL_COMPLETE(ex->dtor, (ex->dlp_field >> 7)
                                            ? (void *)context->GPR[ex->pointer]
                                            : *(void **)(context->FP + ex->pointer));
            info->action_pointer += sizeof(ex_destroylocalpointer);
        } break;

        case EXACTION_DESTROYLOCALARRAY: {
            ex_destroylocalarray *ex = (ex_destroylocalarray *)info->action_pointer;
            long size, n;
            char *ptr;

            n = ex->elements;
            size = ex->element_size;
            ptr = context->FP + ex->localarray;
            for (ptr = ptr + size * n; n > 0; n--) {
                ptr -= size;
                DTORCALL_COMPLETE(ex->dtor, ptr);
            }
            info->action_pointer += sizeof(ex_destroylocalarray);
        } break;

        case EXACTION_DESTROYBASE: {
            ex_destroymember *ex = (ex_destroymember *)info->action_pointer;
            char *object = (ex->dm_field >> 7) ? (char *)context->GPR[ex->objectptr]
                                               : *(char **)(context->FP + ex->objectptr);

            DTORCALL_PARTIAL(ex->dtor, object + ex->offset);
            info->action_pointer += sizeof(ex_destroymember);
        } break;

        case EXACTION_DESTROYMEMBER: {
            ex_destroymember *ex = (ex_destroymember *)info->action_pointer;
            char *object = (ex->dm_field >> 7) ? (char *)context->GPR[ex->objectptr]
                                               : *(char **)(context->FP + ex->objectptr);

            DTORCALL_COMPLETE(ex->dtor, object + ex->offset);
            info->action_pointer += sizeof(ex_destroymember);
        } break;

        case EXACTION_DESTROYMEMBERCOND: {
            ex_destroymembercond *ex = (ex_destroymembercond *)info->action_pointer;
            char *object = ((ex->dmc_field >> 6) & 1) ? (char *)context->GPR[ex->objectptr]
                                                      : *(char **)(context->FP + ex->objectptr);

            if ((ex->dmc_field >> 7) ? (short)context->GPR[ex->cond]
                                     : *(short *)(context->FP + ex->cond)) {
                DTORCALL_PARTIAL(ex->dtor, object + ex->offset);
            }
            info->action_pointer += sizeof(ex_destroymembercond);
        } break;

        case EXACTION_DESTROYMEMBERARRAY: {
            ex_destroymemberarray *ex = (ex_destroymemberarray *)info->action_pointer;
            long size, n;
            char *ptr;

            ptr = (ex->dma_field >> 7) ? (char *)context->GPR[ex->objectptr]
                                       : *(char **)(context->FP + ex->objectptr);
            n = ex->elements;
            size = ex->element_size;
            ptr += ex->offset;
            for (ptr = ptr + size * n; n > 0; n--) {
                ptr -= size;
                DTORCALL_COMPLETE(ex->dtor, ptr);
            }
            info->action_pointer += sizeof(ex_destroymemberarray);
        } break;

        case EXACTION_DELETEPOINTER: {
            ex_deletepointer *ex = (ex_deletepointer *)info->action_pointer;
            char *object = (ex->dp_field >> 7) ? (char *)context->GPR[ex->objectptr]
                                               : *(char **)(context->FP + ex->objectptr);

            ((void (*)(void *))ex->deletefunc)(object);
            info->action_pointer += sizeof(ex_deletepointer);
        } break;

        case EXACTION_DELETEPOINTERCOND: {
            ex_deletepointercond *ex = (ex_deletepointercond *)info->action_pointer;
            char *object = ((ex->dpc_field >> 6) & 1) ? (char *)context->GPR[ex->objectptr]
                                                      : *(char **)(context->FP + ex->objectptr);

            if ((ex->dpc_field >> 7) ? (char)context->GPR[ex->cond]
                                     : *(char *)(context->FP + ex->cond)) {
                ((void (*)(void *))ex->deletefunc)(object);
            }
            info->action_pointer += sizeof(ex_deletepointercond);
        } break;

        case EXACTION_CATCHBLOCK:
            if (catcher == (void *)info->action_pointer) {
                return;
            }
            info->action_pointer += sizeof(ex_catchblock);
            break;

        case EXACTION_CATCHBLOCK_32:
            if (catcher == (void *)info->action_pointer) {
                return;
            }
            info->action_pointer += sizeof(ex_catchblock_32);
            break;

        case EXACTION_ACTIVECATCHBLOCK: {
            /* An exception caught by a frame being unwound is destroyed, unless it is the one
             * being rethrown. */
            CatchInfo *catchinfo;
            ex_activecatchblock *ex = (ex_activecatchblock *)info->action_pointer;

            catchinfo = (CatchInfo *)(context->FP + ex->cinfo_ref);
            if (catchinfo->dtor) {
                if (context->location == catchinfo->location) {
                    context->dtor = catchinfo->dtor;
                } else {
                    DTORCALL_COMPLETE(catchinfo->dtor, catchinfo->location);
                }
            }
            info->action_pointer += sizeof(ex_activecatchblock);
        } break;

        case EXACTION_SPECIFICATION:
            if (catcher == (void *)info->action_pointer) {
                return;
            }
            info->action_pointer +=
                sizeof(ex_specification) - sizeof(char *) +
                ((ex_specification *)info->action_pointer)->specs * sizeof(char *);
            break;

        default:
            std::terminate();
        }
        if (action & EXACTION_ENDBIT) {
            info->action_pointer = 0;
        }
    }
}

/* Whether a function's exception specification lets an exception of type extype through. */
static inline int ExPPC_IsInSpecification(char *extype, ex_specification *spec) {
    long i, offset;

    for (i = 0; i < spec->specs; i++) {
        if (__throw_catch_compare(extype, spec->spec[i], &offset)) {
            return 1;
        }
    }
    return 0;
}

/* Called from the handler the compiler adds to a function with an exception specification, when
 * an exception the specification does not list reaches it. */
extern "C" void __unexpected(CatchInfo *catchinfo) {
    ex_specification *unexp = (ex_specification *)catchinfo->stacktop;

#pragma exception_magic

    try {
        std::unexpected();
    } catch (...) {
        /* The unexpected handler threw: let that exception through if the specification allows
         * it, or replace it with bad_exception if it allows that. */
        if (ExPPC_IsInSpecification((char *)((CatchInfo *)&__exception_magic)->typeinfo, unexp)) {
            throw;
        }
        if (ExPPC_IsInSpecification("!bad_exception!!", unexp)) {
            throw std::bad_exception();
        }
        if (ExPPC_IsInSpecification("!std::bad_exception!!", unexp)) {
            throw std::bad_exception();
        }
    }
    std::terminate();
}

/* Unwinds to the function whose specification the exception violates and enters its handler,
 * which calls __unexpected. As in ExPPC_UnwindStack, an exception escaping from here terminates
 * the program. */
static inline void ExPPC_HandleUnexpected(ThrowContext *context, MWExceptionInfo *info,
                                          ex_specification *unexp) {
    CatchInfo *catchinfo;

#pragma exception_terminate

    ExPPC_UnwindStack(context, info, unexp);

    catchinfo = (CatchInfo *)(context->FP + unexp->cinfo_ref);
    catchinfo->location = context->location;
    catchinfo->typeinfo = context->throwtype;
    catchinfo->dtor = context->dtor;
    catchinfo->stacktop = unexp;

    ExPPC_LongJump(context, info->TOC, info->current_function + unexp->pcoffset);
}

/* The body of a throw: finds the catch block (or violated specification) that takes the
 * exception, unwinds to it and jumps into the handler. A null throwtype rethrows the exception
 * of the innermost active catch block. */
static void ExPPC_ThrowHandler(ThrowContext *context) {
    ActionIterator iter;
    MWExceptionInfo info;
    exaction_type action;
    CatchInfo *catchinfo;
    long offset;

    ExPPC_FindExceptionRecord(context->returnaddr, &info);
    if (info.exception_record == 0) {
        std::terminate();
    }
    context->FP = ET_HAS_FRAME_POINTER(*(unsigned short *)info.exception_record)
                      ? (char *)context->GPR[31]
                      : context->SP;

    if (context->throwtype == 0) {
        iter.info = info;
        iter.current_SP = context->SP;
        iter.current_FP = context->FP;
        iter.current_R31 = context->GPR[31];
        for (action = ExPPC_CurrentAction(&iter);; action = ExPPC_NextAction(&iter)) {
            switch (action) {
            case EXACTION_ACTIVECATCHBLOCK:
                break;
            case EXACTION_ENDOFLIST:
            case EXACTION_DESTROYLOCAL:
            case EXACTION_DESTROYLOCALCOND:
            case EXACTION_DESTROYLOCALPOINTER:
            case EXACTION_DESTROYLOCALARRAY:
            case EXACTION_DESTROYBASE:
            case EXACTION_DESTROYMEMBER:
            case EXACTION_DESTROYMEMBERCOND:
            case EXACTION_DESTROYMEMBERARRAY:
            case EXACTION_DELETEPOINTER:
            case EXACTION_DELETEPOINTERCOND:
            case EXACTION_CATCHBLOCK:
            case EXACTION_CATCHBLOCK_32:
            case EXACTION_SPECIFICATION:
                continue;
            case EXACTION_TERMINATE:
            default:
                std::terminate();
            }
            break;
        }
        catchinfo = (CatchInfo *)(iter.current_FP +
                                  ((ex_activecatchblock *)iter.info.action_pointer)->cinfo_ref);
        context->throwtype = (char *)catchinfo->typeinfo;
        context->location = catchinfo->location;
        context->dtor = 0;
        context->catchinfo = catchinfo;
    } else {
        context->catchinfo = 0;
    }

    iter.info = info;
    iter.current_SP = context->SP;
    iter.current_FP = context->FP;
    iter.current_R31 = context->GPR[31];
    for (action = ExPPC_CurrentAction(&iter);; action = ExPPC_NextAction(&iter)) {
        switch (action) {
        case EXACTION_CATCHBLOCK_32:
            if (__throw_catch_compare(context->throwtype,
                                      ((ex_catchblock_32 *)iter.info.action_pointer)->catch_type,
                                      &offset)) {
                break;
            }
            continue;
        case EXACTION_CATCHBLOCK:
            if (__throw_catch_compare(context->throwtype,
                                      ((ex_catchblock *)iter.info.action_pointer)->catch_type,
                                      &offset)) {
                break;
            }
            continue;
        case EXACTION_SPECIFICATION:
            if (!ExPPC_IsInSpecification(context->throwtype,
                                         (ex_specification *)iter.info.action_pointer)) {
                ExPPC_HandleUnexpected(context, &info,
                                       (ex_specification *)iter.info.action_pointer);
            }
            continue;
        case EXACTION_ENDOFLIST:
        case EXACTION_DESTROYLOCAL:
        case EXACTION_DESTROYLOCALCOND:
        case EXACTION_DESTROYLOCALPOINTER:
        case EXACTION_DESTROYLOCALARRAY:
        case EXACTION_DESTROYBASE:
        case EXACTION_DESTROYMEMBER:
        case EXACTION_DESTROYMEMBERCOND:
        case EXACTION_DESTROYMEMBERARRAY:
        case EXACTION_DELETEPOINTER:
        case EXACTION_DELETEPOINTERCOND:
        case EXACTION_ACTIVECATCHBLOCK:
            continue;
        case EXACTION_TERMINATE:
        default:
            std::terminate();
        }
        break;
    }

    if (action == EXACTION_CATCHBLOCK_32) {
        ex_catchblock_32 *catchblock_32 = (ex_catchblock_32 *)iter.info.action_pointer;

        ExPPC_UnwindStack(context, &info, catchblock_32);

        catchinfo = (CatchInfo *)(context->FP + catchblock_32->cinfo_ref);
        catchinfo->location = context->location;
        catchinfo->typeinfo = context->throwtype;
        catchinfo->dtor = context->dtor;
        if (*context->throwtype == '*') {
            catchinfo->sublocation = &catchinfo->pointercopy;
            catchinfo->pointercopy = *(long *)context->location + offset;
        } else {
            catchinfo->sublocation = (char *)context->location + offset;
        }
        ExPPC_LongJump(context, info.TOC, info.current_function + catchblock_32->catch_pcoffset);
    } else {
        ex_catchblock *catchblock = (ex_catchblock *)iter.info.action_pointer;

        ExPPC_UnwindStack(context, &info, catchblock);

        catchinfo = (CatchInfo *)(context->FP + catchblock->cinfo_ref);
        catchinfo->location = context->location;
        catchinfo->typeinfo = context->throwtype;
        catchinfo->dtor = context->dtor;
        if (*context->throwtype == '*') {
            catchinfo->sublocation = &catchinfo->pointercopy;
            catchinfo->pointercopy = *(long *)context->location + offset;
        } else {
            catchinfo->sublocation = (char *)context->location + offset;
        }
        ExPPC_LongJump(context, info.TOC, info.current_function + catchblock->catch_pcoffset);
    }
}

/* The end of a catch block: destroys the caught exception. */
extern "C" void __end__catch(CatchInfo *catchinfo) {
    if (catchinfo->location && catchinfo->dtor) {
        DTORCALL_COMPLETE(catchinfo->dtor, catchinfo->location);
    }
}

/* Leaves the throw: restores the non-volatile registers from context, switches to the stack of
 * the throw (linked to the frame that catches, so that the thrown object stays allocated) and
 * jumps to newPC with r2 set to newRTOC. Hand-written assembly in the original. */
static asm void ExPPC_LongJump(register ThrowContext *context, register void *newRTOC,
                               register void *newPC) {
    nofralloc
    mr r8, newPC
    mr r2, newRTOC
    lwz r0, ThrowContext.CR(context)
    mtcrf 255, r0
    lmw r13, ThrowContext.GPR[13](context)
    addi r7, context, ThrowContext.FPR[14].ps0
    psq_lx f14, 0, r7, 0, 0
    lfd f14, ThrowContext.FPR[14].d(context)
    addi r7, context, ThrowContext.FPR[15].ps0
    psq_lx f15, 0, r7, 0, 0
    lfd f15, ThrowContext.FPR[15].d(context)
    addi r7, context, ThrowContext.FPR[16].ps0
    psq_lx f16, 0, r7, 0, 0
    lfd f16, ThrowContext.FPR[16].d(context)
    addi r7, context, ThrowContext.FPR[17].ps0
    psq_lx f17, 0, r7, 0, 0
    lfd f17, ThrowContext.FPR[17].d(context)
    addi r7, context, ThrowContext.FPR[18].ps0
    psq_lx f18, 0, r7, 0, 0
    lfd f18, ThrowContext.FPR[18].d(context)
    addi r7, context, ThrowContext.FPR[19].ps0
    psq_lx f19, 0, r7, 0, 0
    lfd f19, ThrowContext.FPR[19].d(context)
    addi r7, context, ThrowContext.FPR[20].ps0
    psq_lx f20, 0, r7, 0, 0
    lfd f20, ThrowContext.FPR[20].d(context)
    addi r7, context, ThrowContext.FPR[21].ps0
    psq_lx f21, 0, r7, 0, 0
    lfd f21, ThrowContext.FPR[21].d(context)
    addi r7, context, ThrowContext.FPR[22].ps0
    psq_lx f22, 0, r7, 0, 0
    lfd f22, ThrowContext.FPR[22].d(context)
    addi r7, context, ThrowContext.FPR[23].ps0
    psq_lx f23, 0, r7, 0, 0
    lfd f23, ThrowContext.FPR[23].d(context)
    addi r7, context, ThrowContext.FPR[24].ps0
    psq_lx f24, 0, r7, 0, 0
    lfd f24, ThrowContext.FPR[24].d(context)
    addi r7, context, ThrowContext.FPR[25].ps0
    psq_lx f25, 0, r7, 0, 0
    lfd f25, ThrowContext.FPR[25].d(context)
    addi r7, context, ThrowContext.FPR[26].ps0
    psq_lx f26, 0, r7, 0, 0
    lfd f26, ThrowContext.FPR[26].d(context)
    addi r7, context, ThrowContext.FPR[27].ps0
    psq_lx f27, 0, r7, 0, 0
    lfd f27, ThrowContext.FPR[27].d(context)
    addi r7, context, ThrowContext.FPR[28].ps0
    psq_lx f28, 0, r7, 0, 0
    lfd f28, ThrowContext.FPR[28].d(context)
    addi r7, context, ThrowContext.FPR[29].ps0
    psq_lx f29, 0, r7, 0, 0
    lfd f29, ThrowContext.FPR[29].d(context)
    addi r7, context, ThrowContext.FPR[30].ps0
    psq_lx f30, 0, r7, 0, 0
    lfd f30, ThrowContext.FPR[30].d(context)
    addi r7, context, ThrowContext.FPR[31].ps0
    psq_lx f31, 0, r7, 0, 0
    lfd f31, ThrowContext.FPR[31].d(context)
    mtlr r8
    lwz r1, ThrowContext.throwSP(context)
    lwz r3, ThrowContext.SP(context)
    lwz r3, 0(r3)
    stw r3, 0(r1)
    blr
}

/* throw: saves the non-volatile registers (r13-r31, f14-f31 with their paired-single halves, CR)
 * in a ThrowContext in its own frame, with the caller's stack pointer and return address and the
 * thrown type, object and destructor, and hands it to ExPPC_ThrowHandler, which does not return.
 * Hand-written assembly in the original; the frame is laid out by hand here (0x2C0 bytes: the
 * three arguments at 8, the context at 0x18). */
#define THROW_FRAME 0x2C0
#define THROW_CONTEXT 0x18

asm void __throw(register char *throwtype, register void *location, register void *dtor) {
    nofralloc
    stwu r1, -THROW_FRAME(r1)
    mflr r0
    stw r0, THROW_FRAME + 4(r1)
    stw throwtype, 8(r1)
    stw location, 12(r1)
    stw dtor, 16(r1)
    stmw r13, THROW_CONTEXT + ThrowContext.GPR[13](r1)
    stfd f14, THROW_CONTEXT + ThrowContext.FPR[14].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[14].ps0
    psq_stx f14, 0, r3, 0, 0
    stfd f15, THROW_CONTEXT + ThrowContext.FPR[15].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[15].ps0
    psq_stx f15, 0, r3, 0, 0
    stfd f16, THROW_CONTEXT + ThrowContext.FPR[16].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[16].ps0
    psq_stx f16, 0, r3, 0, 0
    stfd f17, THROW_CONTEXT + ThrowContext.FPR[17].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[17].ps0
    psq_stx f17, 0, r3, 0, 0
    stfd f18, THROW_CONTEXT + ThrowContext.FPR[18].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[18].ps0
    psq_stx f18, 0, r3, 0, 0
    stfd f19, THROW_CONTEXT + ThrowContext.FPR[19].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[19].ps0
    psq_stx f19, 0, r3, 0, 0
    stfd f20, THROW_CONTEXT + ThrowContext.FPR[20].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[20].ps0
    psq_stx f20, 0, r3, 0, 0
    stfd f21, THROW_CONTEXT + ThrowContext.FPR[21].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[21].ps0
    psq_stx f21, 0, r3, 0, 0
    stfd f22, THROW_CONTEXT + ThrowContext.FPR[22].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[22].ps0
    psq_stx f22, 0, r3, 0, 0
    stfd f23, THROW_CONTEXT + ThrowContext.FPR[23].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[23].ps0
    psq_stx f23, 0, r3, 0, 0
    stfd f24, THROW_CONTEXT + ThrowContext.FPR[24].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[24].ps0
    psq_stx f24, 0, r3, 0, 0
    stfd f25, THROW_CONTEXT + ThrowContext.FPR[25].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[25].ps0
    psq_stx f25, 0, r3, 0, 0
    stfd f26, THROW_CONTEXT + ThrowContext.FPR[26].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[26].ps0
    psq_stx f26, 0, r3, 0, 0
    stfd f27, THROW_CONTEXT + ThrowContext.FPR[27].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[27].ps0
    psq_stx f27, 0, r3, 0, 0
    stfd f28, THROW_CONTEXT + ThrowContext.FPR[28].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[28].ps0
    psq_stx f28, 0, r3, 0, 0
    stfd f29, THROW_CONTEXT + ThrowContext.FPR[29].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[29].ps0
    psq_stx f29, 0, r3, 0, 0
    stfd f30, THROW_CONTEXT + ThrowContext.FPR[30].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[30].ps0
    psq_stx f30, 0, r3, 0, 0
    stfd f31, THROW_CONTEXT + ThrowContext.FPR[31].d(r1)
    addi r3, r1, THROW_CONTEXT + ThrowContext.FPR[31].ps0
    psq_stx f31, 0, r3, 0, 0
    mfcr r3
    stw r3, THROW_CONTEXT + ThrowContext.CR(r1)
    lwz r3, 0(r1)
    lwz r4, 4(r3)
    stw r3, THROW_CONTEXT + ThrowContext.SP(r1)
    stw r3, THROW_CONTEXT + ThrowContext.throwSP(r1)
    stw r4, THROW_CONTEXT + ThrowContext.returnaddr(r1)
    lwz r3, 8(r1)
    stw r3, THROW_CONTEXT + ThrowContext.throwtype(r1)
    lwz r3, 12(r1)
    stw r3, THROW_CONTEXT + ThrowContext.location(r1)
    lwz r3, 16(r1)
    stw r3, THROW_CONTEXT + ThrowContext.dtor(r1)
    addi r3, r1, THROW_CONTEXT
    bl ExPPC_ThrowHandler
    nop
    lwz r0, THROW_FRAME + 4(r1)
    mtlr r0
    addi r1, r1, THROW_FRAME
    blr
}
