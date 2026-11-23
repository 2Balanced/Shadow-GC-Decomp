"""Wave 41: the register-allocation functions of PjsEventActionScene with copy propagation off (the
wave-40 lever), one function at a time."""
import os
SRC = open(os.path.join(os.path.dirname(__file__), "..", "src", "Game", "PjsEventActionScene.cpp"), encoding="utf-8").read()
FUNCS = {
  "show": ("bool UnkEventModel_80170A9C::Show() {", "Show__22UnkEventModel_80170A9CFv"),
  "update": ("void UnkEventModel_80170A9C::Update(float previous, float time) {", "Update__22UnkEventModel_80170A9CFff"),
  "dtor": ("UnkEventModel_80170A9C::~UnkEventModel_80170A9C() {", "__dt__22UnkEventModel_80170A9CFv"),
  "c58": ('extern "C" void fn_80170C58(TPjsEventActionScene *scene, const char *name, bool start, int id) {', "fn_80170C58"),
  "startload": ("bool TPjsEventActionScene::StartLoad() {", "StartLoad__20TPjsEventActionSceneFv"),
  "camupdate": ("void UnkEventCamera_80171F80::Update() {", "Update__23UnkEventCamera_80171F80Fv"),
  "find": ("UnkEventCharaModel *UnkEventCharaList_80172D4C::Find(const char *name) {", "Find__26UnkEventCharaList_80172D4CFPCc"),
  "linkparts": ("void UnkEventCharaList_80172D4C::LinkParts() {", "LinkParts__26UnkEventCharaList_80172D4CFv"),
}
SCORE = [v[1] for v in FUNCS.values()]
def wrap(head):
    i = SRC.index(head)
    j = SRC.index("\n}\n", i) + 3
    body = SRC[i:j]
    return [(body, "#pragma opt_propagation off\n" + body + "#pragma opt_propagation reset\n")]
REPL = {"base": []}
for k, (head, sym) in FUNCS.items():
    REPL["p_" + k] = wrap(head)
