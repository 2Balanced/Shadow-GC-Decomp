/* RenderWare's 64-entry object pool: two slot allocators and the matching deallocator. These three
 * functions are written in assembly to preserve retail's exact countdown-loop layout. */
extern const char lbl_8051BB28[];
extern const char lbl_8051BB4C[];
extern const char lbl_8051BB5C[];
extern void *lbl_805F1404;
extern "C" void OSReport(const char *, ...);
extern "C" void fn_8043AC4C(void *, unsigned long);
extern "C" void fn_803836F4(void);

extern "C" asm void fn_8043AA7C(void *object, int index, unsigned long unused, void *a, void *b, int wait) {
    nofralloc
    stwu r1, -16(r1)
    mflr r0
    mr r12, r3
    stw r0, 20(r1)
    li r0, 64
    stmw r30, 8(r1)
    mr r31, r8
    li r30, 0
    lwz r3, lbl_805F1404
    mtctr r0
L_8043AAA4:
    addi r0, r30, 20572
    lbzx r0, r3, r0
    cmplwi r0, 0
    beq L_8043AADC
    addi r30, r30, 1
    bdnz L_8043AAA4
L_8043AABC:
    lis r3, lbl_8051BB28@ha
    lis r4, lbl_8051BB4C@ha
    addi r3, r3, lbl_8051BB28@l
    li r5, 194
    addi r4, r4, lbl_8051BB4C@l
    crclr 4*cr1+eq
    bl OSReport
L_8043AAD8:
    b L_8043AAD8
L_8043AADC:
    add r3, r3, r30
    li r0, 1
    stb r0, 20572(r3)
    lis r3, fn_8043AC4C@ha
    mr r9, r5
    slwi r8, r30, 5
    lwz r0, lbl_805F1404
    addi r10, r3, fn_8043AC4C@l
    slwi r11, r30, 2
    addi r3, r8, 18524
    mr r8, r4
    li r5, 0
    add r4, r0, r11
    stw r6, 20640(r4)
    li r6, 1
    lwz r0, lbl_805F1404
    add r4, r0, r11
    stw r7, 20896(r4)
    mr r7, r12
    lwz r11, lbl_805F1404
    lwz r4, 20636(r11)
    add r3, r11, r3
    bl fn_803836F4
    cmpwi r31, 0
    lwz r3, lbl_805F1404
    addi r4, r30, 20572
    bne L_8043AB54
L_8043AB48:
    lbzx r0, r3, r4
    cmplwi r0, 0
    bne L_8043AB48
L_8043AB54:
    mr r3, r30
    lmw r30, 8(r1)
    lwz r0, 20(r1)
    mtlr r0
    addi r1, r1, 16
    blr
}

extern "C" asm void fn_8043AB6C(void *object, int index, unsigned long unused, void *a, void *b) {
    nofralloc
    stwu r1, -16(r1)
    mflr r0
    mr r12, r3
    stw r0, 20(r1)
    li r0, 64
    stw r31, 12(r1)
    li r31, 0
    lwz r3, lbl_805F1404
    mtctr r0
L_8043AB90:
    addi r0, r31, 20572
    lbzx r0, r3, r0
    cmplwi r0, 0
    beq L_8043ABC8
    addi r31, r31, 1
    bdnz L_8043AB90
L_8043ABA8:
    lis r3, lbl_8051BB28@ha
    lis r4, lbl_8051BB4C@ha
    addi r3, r3, lbl_8051BB28@l
    li r5, 194
    addi r4, r4, lbl_8051BB4C@l
    crclr 4*cr1+eq
    bl OSReport
L_8043ABC4:
    b L_8043ABC4
L_8043ABC8:
    add r3, r3, r31
    li r0, 1
    stb r0, 20572(r3)
    lis r3, fn_8043AC4C@ha
    mr r9, r5
    slwi r8, r31, 5
    lwz r0, lbl_805F1404
    addi r10, r3, fn_8043AC4C@l
    slwi r11, r31, 2
    addi r3, r8, 18524
    mr r8, r4
    li r5, 0
    add r4, r0, r11
    stw r6, 20640(r4)
    li r6, 1
    lwz r0, lbl_805F1404
    add r4, r0, r11
    stw r7, 20896(r4)
    mr r7, r12
    lwz r11, lbl_805F1404
    lwz r4, 20636(r11)
    add r3, r11, r3
    bl fn_803836F4
    lwz r3, lbl_805F1404
    addi r4, r31, 20572
L_8043AC1C:
    lbzx r0, r3, r4
    cmplwi r0, 0
    bne L_8043AC1C
    lwz r0, 20(r1)
    lwz r31, 12(r1)
    mtlr r0
    addi r1, r1, 16
    blr
}

extern "C" asm void fn_8043AC4C(void *entry, unsigned long context) {
    nofralloc
    stwu r1, -16(r1)
    mflr r0
    li r6, 0
    li r4, 0
    stw r0, 20(r1)
    li r0, 64
    lwz r5, lbl_805F1404
    mtctr r0
L_8043AC6C:
    addi r0, r4, 18524
    add r0, r5, r0
    cmplw r0, r3
    beq L_8043ACA8
    addi r6, r6, 1
    addi r4, r4, 32
    bdnz L_8043AC6C
L_8043AC88:
    lis r3, lbl_8051BB5C@ha
    lis r4, lbl_8051BB4C@ha
    addi r3, r3, lbl_8051BB5C@l
    li r5, 174
    addi r4, r4, lbl_8051BB4C@l
    crclr 4*cr1+eq
    bl OSReport
L_8043ACA4:
    b L_8043ACA4
L_8043ACA8:
    add r3, r5, r6
    li r0, 0
    stb r0, 20572(r3)
    slwi r0, r6, 2
    lwz r3, lbl_805F1404
    add r3, r3, r0
    lwz r12, 20640(r3)
    cmplwi r12, 0
    beq L_8043ACD8
    lwz r3, 20896(r3)
    mtctr r12
    bctrl
L_8043ACD8:
    lwz r0, 20(r1)
    mtlr r0
    addi r1, r1, 16
    blr
}
