"""GadgetModelInfo::fn_80162FB0 (wave 40): the collider iterator declared before the arguments
(gives it r31 and `this` r30), then where the other two iterators are declared."""
A = """        ColliDetectArgs args;
        for (ColliderIter it = m_colliders.begin(); it != m_colliders.end();
             ++it) {
"""
M = "    for (MotionIter motion = m_motions.begin(); motion != m_motions.end();\n         ++motion) {\n        u32 flags = motion->m_flags;\n"
P = "    for (PartIter part = m_parts.begin(); part != m_parts.end(); ++part) {\n        if (part->m_desc->m_flags & 1) {\n"
M2 = "    for (motion = m_motions.begin(); motion != m_motions.end();\n         ++motion) {\n        u32 flags = motion->m_flags;\n"
P2 = "    for (part = m_parts.begin(); part != m_parts.end(); ++part) {\n        if (part->m_desc->m_flags & 1) {\n"
H = "void GadgetModelInfo::fn_80162FB0(float dt) {\n"
C = "            UnkColliContacts_8001EE04 *contacts =\n                (UnkColliContacts_8001EE04 *)((u8 *)it->m_colli.get() + 0xC);\n"
C2 = "            contacts =\n                (UnkColliContacts_8001EE04 *)((u8 *)it->m_colli.get() + 0xC);\n"
REPL = {
 "mp_top": [(H, H + "    MotionIter motion;\n    PartIter part;\n"), (M, M2), (P, P2)],
 "pm_top": [(H, H + "    PartIter part;\n    MotionIter motion;\n"), (M, M2), (P, P2)],
 "imp_top": [(H, H + "    ColliderIter it;\n    MotionIter motion;\n    PartIter part;\n"), (M, M2), (P, P2), (A, A.replace("for (ColliderIter it", "for (it"))],
 "c_top": [(H, H + "    UnkColliContacts_8001EE04 *contacts;\n"), (C, C2)],
 "c_blk": [(A, "        UnkColliContacts_8001EE04 *contacts;\n" + A), (C, C2)],
 "c_blk2": [(A, A.replace("        ColliDetectArgs args;\n", "        ColliDetectArgs args;\n        UnkColliContacts_8001EE04 *contacts;\n")), (C, C2)],
 "it_ctor": [(A, A.replace("ColliderIter it = m_colliders.begin()", "ColliderIter it(m_colliders.begin())"))],
 "node": [(C, "            GadgetModelCollider &c = *it;\n" + C.replace("it->", "c."))],
 "else": [("    if (!m_flags.Test(0) && !m_flags.Test(1)) {\n        ColliDetectArgs args;", "    if (m_flags.Test(0) || m_flags.Test(1)) {\n    } else {\n        ColliDetectArgs args;")],
}
SCORE = ["fn_80162FB0__15GadgetModelInfoFf"]
