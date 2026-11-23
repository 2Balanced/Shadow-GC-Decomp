"""Wave 40: the slots as const members (the base pointer of a const object may be loaded above
the store of LR)."""
N = ["0x0C", "0x10", "0x14", "0x18", "0x38", "0x24", "0x28", "0x2C", "0x34", "0x68"]
SCORE = [("vfPara_%s__16EnemyParamCommonFv" % n, "vfPara_%s__16EnemyParamCommonCFv" % n) for n in N]
def cpp(n):
    return [("EnemyParamCommon::vfPara_%s() {" % n, "EnemyParamCommon::vfPara_%s() const {" % n) for n in N]
def hdr():
    out = []
    for n in N:
        t = "int" if n in ("0x0C", "0x10", "0x14", "0x18", "0x38") else "float"
        out.append(("virtual %s vfPara_%s();" % (t, n), "virtual %s vfPara_%s() const;" % (t, n)))
    return out
REPL = {"const": cpp(0)}
HDR = {"const": {"Game/Enemy.h": hdr() + hdr()}}

# const plus a spelling of the record access (scored on 0x10 and 0x68 only)
SCORE = [("vfPara_%s__16EnemyParamCommonFv" % n, "vfPara_%s__16EnemyParamCommonCFv" % n) for n in ("0x10", "0x68")]
A10 = "    return reinterpret_cast<int>(lbl_804CBF2C[EnemyTypeOf(m_enemy)][1]);\n"
A68 = "    return lbl_804CC424[m_enemy->m_type].m_gravity;\n"
F = "int EnemyParamCommon::vfPara_0x08() {"
HELP = """inline const EnemyMoveParam &MoveOf_(s32 type) { return lbl_804CC424[type]; }
inline const EnemyMoveParam *MoveOfP_(s32 type) { return &lbl_804CC424[type]; }
inline const EnemyMoveParam *MoveTable_() { return lbl_804CC424; }
inline const char *const *FilesOf_(s32 type) { return lbl_804CBF2C[type]; }
"""
H = (F, HELP + F)
V = {
  "move_ref": [H, (A68, "    return MoveOf_(m_enemy->m_type).m_gravity;\n")],
  "move_ptr": [H, (A68, "    return MoveOfP_(m_enemy->m_type)->m_gravity;\n")],
  "move_local": [(A68, "    const EnemyMoveParam *move = &lbl_804CC424[m_enemy->m_type];\n    return move->m_gravity;\n"),
                 (A10, "    const char *const *files = lbl_804CBF2C[EnemyTypeOf(m_enemy)];\n    return reinterpret_cast<int>(files[1]);\n")],
  "move_ref_local": [(A68, "    const EnemyMoveParam &move = lbl_804CC424[m_enemy->m_type];\n    return move.m_gravity;\n")],
  "table_local": [(A68, "    const EnemyMoveParam *table = lbl_804CC424;\n    return table[m_enemy->m_type].m_gravity;\n"),
                  (A10, "    const char *const (*table)[4] = lbl_804CBF2C;\n    return reinterpret_cast<int>(table[EnemyTypeOf(m_enemy)][1]);\n")],
  "table_inl": [H, (A68, "    return MoveTable_()[m_enemy->m_type].m_gravity;\n"),
                (A10, "    return reinterpret_cast<int>(FilesOf_(EnemyTypeOf(m_enemy))[1]);\n")],
  "type_local": [(A68, "    s32 type = m_enemy->m_type;\n    return lbl_804CC424[type].m_gravity;\n"),
                 (A10, "    s32 type = EnemyTypeOf(m_enemy);\n    return reinterpret_cast<int>(lbl_804CBF2C[type][1]);\n")],
  "ptr_add": [(A68, "    return (lbl_804CC424 + m_enemy->m_type)->m_gravity;\n"),
              (A10, "    return reinterpret_cast<int>((*(lbl_804CBF2C + EnemyTypeOf(m_enemy)))[1]);\n")],
  "enemy_local": [(A68, "    const Enemy *enemy = m_enemy;\n    return lbl_804CC424[enemy->m_type].m_gravity;\n")],
}
for k, v in V.items():
    REPL["c_" + k] = cpp(0) + v
    HDR["c_" + k] = HDR["const"]

T10 = (A10, "    s32 type = EnemyTypeOf(m_enemy);\n    return reinterpret_cast<int>(lbl_804CBF2C[type][1]);\n")
W = {
  "t_move_local": [(A68, "    s32 type = m_enemy->m_type;\n    const EnemyMoveParam *move = &lbl_804CC424[type];\n    return move->m_gravity;\n")],
  "t_move_ref": [H, (A68, "    s32 type = m_enemy->m_type;\n    return MoveOf_(type).m_gravity;\n")],
  "t_inl_type": [H, (A68, "    return MoveOf_(EnemyTypeOf(m_enemy)).m_gravity;\n")],
  "t_inl_type2": [H, (A68, "    return lbl_804CC424[EnemyTypeOf(m_enemy)].m_gravity;\n")],
  "t_inl_type3": [H, (A68, "    s32 type = EnemyTypeOf(m_enemy);\n    return lbl_804CC424[type].m_gravity;\n")],
  "t_float_local": [(A68, "    float value = lbl_804CC424[m_enemy->m_type].m_gravity;\n    return value;\n")],
  "t_fieldptr": [(A68, "    const float *value = &lbl_804CC424[m_enemy->m_type].m_gravity;\n    return *value;\n")],
  "t_tablefirst": [(A68, "    const EnemyMoveParam *move = lbl_804CC424;\n    move += m_enemy->m_type;\n    return move->m_gravity;\n")],
  "t_tablefirst2": [(A68, "    const EnemyMoveParam *move = lbl_804CC424;\n    s32 type = m_enemy->m_type;\n    return move[type].m_gravity;\n")],
  "t_typefirst": [(A68, "    s32 type = m_enemy->m_type;\n    const EnemyMoveParam *move = lbl_804CC424;\n    return move[type].m_gravity;\n")],
}
REPL = {}; HDR2 = {}
for k, v in W.items():
    REPL[k] = cpp(0) + [T10] + v
    HDR2[k] = HDR["const"]
HDR = HDR2

ALL = ["0x08","0x0C","0x10","0x14","0x18","0x1C","0x20","0x24","0x28","0x2C","0x30","0x34","0x38","0x3C","0x68","0x6C","0x70","0x74"]
SCORE = [("vfPara_%s__16EnemyParamCommonFv" % n, "vfPara_%s__16EnemyParamCommon%sFv" % (n, "C" if n in N else "")) for n in ALL]
X = {
  "arr2": [(A68, "    return reinterpret_cast<const float (*)[7]>(lbl_804CC424)[m_enemy->m_type][6];\n")],
  "arr2_t": [(A68, "    s32 type = m_enemy->m_type;\n    return reinterpret_cast<const float (*)[7]>(lbl_804CC424)[type][6];\n")],
  "base": [],
}
REPL = {}; HDR2 = {}
for k, v in X.items():
    REPL[k] = cpp(0) + [T10] + v
    HDR2[k] = HDR["t_move_ref"]
HDR = HDR2

SCORE = [("vfPara_%s__16EnemyParamCommonFv" % n, "vfPara_%s__16EnemyParamCommon%sFv" % (n, "C" if n in N else "")) for n in ("0x68",)]
SV = "struct MoveV_ { float v[7]; };\nstruct MoveN_ { float a; struct { float b[6]; } n; };\ninline float MoveField_(const EnemyMoveParam &m) { return m.m_gravity; }\ninline float MoveField2_(const EnemyMoveParam *m) { return m->m_gravity; }\ninline float MoveIdx_(const EnemyMoveParam *t, s32 i) { return t[i].m_gravity; }\n"
H2 = (F, SV + F)
Y = {
  "v_arr": [H2, (A68, "    return reinterpret_cast<const MoveV_ *>(lbl_804CC424)[m_enemy->m_type].v[6];\n")],
  "v_nest": [H2, (A68, "    return reinterpret_cast<const MoveN_ *>(lbl_804CC424)[m_enemy->m_type].n.b[5];\n")],
  "v_deref": [(A68, "    return *(&lbl_804CC424[m_enemy->m_type].field_0x0 + 6);\n")],
  "v_field": [H2, (A68, "    return MoveField_(lbl_804CC424[m_enemy->m_type]);\n")],
  "v_field2": [H2, (A68, "    return MoveField2_(&lbl_804CC424[m_enemy->m_type]);\n")],
  "v_idx": [H2, (A68, "    return MoveIdx_(lbl_804CC424, m_enemy->m_type);\n")],
  "v_charp": [(A68, "    return *reinterpret_cast<const float *>(reinterpret_cast<const char *>(lbl_804CC424) + m_enemy->m_type * 28 + 24);\n")],
  "v_charp2": [(A68, "    return reinterpret_cast<const EnemyMoveParam *>(reinterpret_cast<const char *>(lbl_804CC424) + m_enemy->m_type * 28)->m_gravity;\n")],
  "v_u32": [(A68, "    return lbl_804CC424[static_cast<u32>(m_enemy->m_type)].m_gravity;\n")],
  "v_copy": [(A68, "    EnemyMoveParam move = lbl_804CC424[m_enemy->m_type];\n    return move.m_gravity;\n")],
}
REPL = {}; HDR2 = {}
h = list(HDR.values())[0]
for k, v in Y.items():
    REPL[k] = cpp(0) + [T10] + v
    HDR2[k] = h
    REPL["n" + k] = [(a, b) for a, b in cpp(0) if "0x68" not in a] + [T10] + v
    HDR2["n" + k] = {"Game/Enemy.h": [(a, b) for a, b in h["Game/Enemy.h"] if "0x68" not in a]}
HDR = HDR2
SCORE = ["vfPara_0x68__16EnemyParamCommonFv", ("vfPara_0x68__16EnemyParamCommonFv", "vfPara_0x68__16EnemyParamCommonCFv")]

Z = {
  "z_tf": [(A68, "    const EnemyMoveParam *move = lbl_804CC424;\n    move += m_enemy->m_type;\n    return move->m_gravity;\n")],
  "z_tf_vol": [(A68, "    const EnemyMoveParam *move;\n    s32 type;\n    move = lbl_804CC424;\n    type = m_enemy->m_type;\n    move = move + type;\n    return move->m_gravity;\n")],
  "z_fp": [(A68, "    const float *p = &lbl_804CC424[0].m_gravity;\n    return *reinterpret_cast<const float *>(reinterpret_cast<const char *>(p) + m_enemy->m_type * 28);\n")],
  "z_static": [(A68, "    static const EnemyMoveParam *const table = lbl_804CC424;\n    return table[m_enemy->m_type].m_gravity;\n")],
  "z_enemyc": [(A68, "    const Enemy &enemy = *m_enemy;\n    return lbl_804CC424[enemy.m_type].m_gravity;\n")],
}
REPL = {}; HDR2 = {}
for k, v in Z.items():
    REPL[k] = cpp(0) + [T10] + v
    HDR2[k] = h
HDR = HDR2
SCORE = [("vfPara_0x68__16EnemyParamCommonFv", "vfPara_0x68__16EnemyParamCommonCFv")]
