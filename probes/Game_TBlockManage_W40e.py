"""TBlockManage::UpdateLoads (wave 40, round 5): retail keeps the block id in r31 and twice the id
(the offset both inlined IsRequested share) in r30; GC/2.7 the other way round. The id as a
compiler temporary (through an inline function or a const reference) instead of a named local."""
SCORE = ["UpdateLoads__12TBlockManageFv"]
O = """void TBlockManage::UpdateLoads() {
    int id;
    s16 i;
"""
A = "        id = i;\n"
REPL = {
  "const_ref": [(O, "void TBlockManage::UpdateLoads() {\n    s16 i;\n"), (A, "        const int &id = i;\n")],
  "const_local": [(O, "void TBlockManage::UpdateLoads() {\n    s16 i;\n"), (A, "        const int id = i;\n")],
  "inline_id": [(O, "inline int BlockId_(s16 i) {\n    return i;\n}\n\nvoid TBlockManage::UpdateLoads() {\n    int id;\n    s16 i;\n"), (A, "        id = BlockId_(i);\n")],
  "inline_id_init": [(O, "inline int BlockId_(s16 i) {\n    return i;\n}\n\nvoid TBlockManage::UpdateLoads() {\n    s16 i;\n"), (A, "        int id = BlockId_(i);\n")],
}
