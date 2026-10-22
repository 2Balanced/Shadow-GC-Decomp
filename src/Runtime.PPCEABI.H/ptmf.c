/* Pointer-to-member-function support. CodeWarrior represents a pointer to member function as a
 * three-word record; the compiler tests one with __ptmf_test and calls through one with
 * __ptmf_scall, passing the record in r12 and the arguments as for the member itself.
 * (__ptmf_cmpr, the comparison, is unused by the game and was dead-stripped.) */

typedef struct __ptmf {
    long this_delta; /* added to `this` before the call */
    long v_offset;   /* offset of the vtable pointer in the object, or -1 for a non-virtual */
    union {
        void *f_addr;   /* non-virtual: the function's address */
        long ve_offset; /* virtual: the entry's offset in the vtable */
    } f_data;
} __ptmf;

/* The value of a null pointer to member function. */
const __ptmf __ptmf_null = {0, 0, 0};

/* Returns nonzero if the pointer to member is not null. */
asm long __ptmf_test(register __ptmf *ptmf) {
    nofralloc
    lwz r5, __ptmf.this_delta(r3)
    lwz r6, __ptmf.v_offset(r3)
    lwz r7, __ptmf.f_data(r3)
    li r3, 1
    cmpwi r5, 0
    cmpwi cr6, r6, 0
    cmpwi cr7, r7, 0
    bnelr
    bnelr cr6
    bnelr cr7
    li r3, 0
    blr
}

/* Calls through the pointer to member in r12 with `this` in r3: adjusts `this`, then for a
 * virtual member (v_offset >= 0) looks the function up in the object's vtable. */
asm void __ptmf_scall(...) {
    nofralloc
    lwz r0, __ptmf.this_delta(r12)
    lwz r11, __ptmf.v_offset(r12)
    lwz r12, __ptmf.f_data(r12)
    add r3, r3, r0
    cmpwi r11, 0
    blt call
    lwzx r12, r3, r12
    lwzx r12, r12, r11
call:
    mtctr r12
    bctr
}

/* The same with `this` in r4, for members that return a structure (r3 holds the return slot). */
asm void __ptmf_scall4(...) {
    nofralloc
    lwz r0, __ptmf.this_delta(r12)
    lwz r11, __ptmf.v_offset(r12)
    lwz r12, __ptmf.f_data(r12)
    add r4, r4, r0
    cmpwi r11, 0
    blt call
    lwzx r12, r4, r12
    lwzx r12, r12, r11
call:
    mtctr r12
    bctr
}
