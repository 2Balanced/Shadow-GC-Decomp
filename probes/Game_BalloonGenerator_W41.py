"""Wave 41: BalloonGenerator::Spawn, 360 / total before the shared pi / 180 (retail)."""
SCORE = ["Spawn__Q212ShootingGame16BalloonGeneratorFi"]
F = "void BalloonGenerator::Spawn(int count) {\n"
S = "    float step = 360.0f / total * (lbl_805FAD38 / 180.0f);\n    float offset = 13.0f * m_spawnCount++ * (lbl_805FAD38 / 180.0f);\n"
NXT = "    for (int i = 0; i < total; i++) {\n        angles->Add(step * i + offset);\n    }\n"


def prag(name):
    return [(F, "#pragma opt_propagation off\n" + F)]


REPL = {
    "d2r_both": [(S, "    float step = DegToRad(360.0f / total);\n    float offset = DegToRad(13.0f * m_spawnCount++);\n")],
    "d2r_both_prag": [(S, "    float step = DegToRad(360.0f / total);\n    float offset = DegToRad(13.0f * m_spawnCount++);\n")] + prag(0),
    "d2r_step": [(S, "    float step = DegToRad(360.0f / total);\n    float offset = 13.0f * m_spawnCount++ * (lbl_805FAD38 / 180.0f);\n")],
    "d2r_off": [(S, "    float step = 360.0f / total * (lbl_805FAD38 / 180.0f);\n    float offset = DegToRad(13.0f * m_spawnCount++);\n")],
    "prag_only": prag(0),
    "count_first": [(S, "    int spawn = m_spawnCount++;\n    float step = 360.0f / total * (lbl_805FAD38 / 180.0f);\n    float offset = 13.0f * spawn * (lbl_805FAD38 / 180.0f);\n")],
    "d2r_count_first": [(S, "    int spawn = m_spawnCount++;\n    float step = DegToRad(360.0f / total);\n    float offset = DegToRad(13.0f * spawn);\n")],
    "d2r_count_first_prag": [(S, "    int spawn = m_spawnCount++;\n    float step = DegToRad(360.0f / total);\n    float offset = DegToRad(13.0f * spawn);\n")] + prag(0),
}
REPL.update({
    "post_inc": [(S, "    float step = 360.0f / total * (lbl_805FAD38 / 180.0f);\n    float offset = 13.0f * m_spawnCount * (lbl_805FAD38 / 180.0f);\n    m_spawnCount++;\n")],
    "post_inc_d2r": [(S, "    float step = DegToRad(360.0f / total);\n    float offset = DegToRad(13.0f * m_spawnCount);\n    m_spawnCount++;\n")],
    "post_inc_d2r_step": [(S, "    float step = DegToRad(360.0f / total);\n    float offset = 13.0f * m_spawnCount * (lbl_805FAD38 / 180.0f);\n    m_spawnCount++;\n")],
    "cnt_local_inl": [(F, "inline int NextSpawn_(int &count) {\n    return count++;\n}\n\n" + F),
                      (S, "    float step = 360.0f / total * (lbl_805FAD38 / 180.0f);\n    float offset = 13.0f * NextSpawn_(m_spawnCount) * (lbl_805FAD38 / 180.0f);\n")],
    "cnt_local_inl_d2r": [(F, "inline int NextSpawn_(int &count) {\n    return count++;\n}\n\n" + F),
                      (S, "    float step = DegToRad(360.0f / total);\n    float offset = DegToRad(13.0f * NextSpawn_(m_spawnCount));\n")],
    "spawn_after_step": [(S, "    float step = 360.0f / total * (lbl_805FAD38 / 180.0f);\n    int spawn = m_spawnCount++;\n    float offset = 13.0f * spawn * (lbl_805FAD38 / 180.0f);\n")],
})
REPL.update({
    "spawn_set_late": [(S, "    int spawn = m_spawnCount;\n    float step = 360.0f / total * (lbl_805FAD38 / 180.0f);\n    float offset = 13.0f * spawn * (lbl_805FAD38 / 180.0f);\n    m_spawnCount = spawn + 1;\n")],
    "spawn_set_early": [(S, "    int spawn = m_spawnCount;\n    m_spawnCount = spawn + 1;\n    float step = 360.0f / total * (lbl_805FAD38 / 180.0f);\n    float offset = 13.0f * spawn * (lbl_805FAD38 / 180.0f);\n")],
    "spawn_first_late_store": [(S, "    int spawn = m_spawnCount++;\n    float step = 360.0f / total * (lbl_805FAD38 / 180.0f);\n    float offset = 13.0f * spawn * (lbl_805FAD38 / 180.0f);\n")],
})
REPL.update({
    "unit_first": [(S, "    float unit = 360.0f / total;\n    float step = unit * (lbl_805FAD38 / 180.0f);\n    float offset = 13.0f * m_spawnCount++ * (lbl_805FAD38 / 180.0f);\n")],
    "unit_first_d2r": [(S, "    float unit = 360.0f / total;\n    float step = DegToRad(unit);\n    float offset = DegToRad(13.0f * m_spawnCount++);\n")],
    "unit_d2r_local": [(S, "    float unit = 360.0f / total;\n    float d2r = lbl_805FAD38 / 180.0f;\n    float step = unit * d2r;\n    float offset = 13.0f * m_spawnCount++ * d2r;\n")],
    "unit_first_prag": [(S, "    float unit = 360.0f / total;\n    float step = unit * (lbl_805FAD38 / 180.0f);\n    float offset = 13.0f * m_spawnCount++ * (lbl_805FAD38 / 180.0f);\n")] + prag(0),
})
