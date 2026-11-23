"""Wave 41: InitProject with a named copy of project and copy propagation off (the wave 40 lever)."""
SCORE = ["InitProject__20TGiCommandWindowImplFPQ34Chao3CSD8CProject"]
SRC = open("src/Game/TGiCommandWindow.cpp", encoding="utf-8").read()
s = SRC.index("void TGiCommandWindowImpl::InitProject(Chao::CSD::CProject *project) {")
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
HEAD = "void TGiCommandWindowImpl::InitProject(Chao::CSD::CProject *project) {\n"
BODY = FN[len(HEAD):]
V = {}
V["copy_first"] = HEAD + "    Chao::CSD::CProject *p = project;\n" + BODY.replace("(project,", "(p,")
V["copy_after"] = HEAD + BODY.replace("    RCPtr<CScene> scene;\n", "    RCPtr<CScene> scene;\n    Chao::CSD::CProject *p = project;\n").replace("(project,", "(p,")
V["constcopy_first"] = HEAD + "    Chao::CSD::CProject *const p = project;\n" + BODY.replace("(project,", "(p,")
V["plain"] = FN
REPL = {}
for k, b in V.items():
    REPL["prag_" + k] = [(FN, "#pragma opt_propagation off\n" + b + "#pragma opt_propagation reset\n")]
    REPL[k] = [(FN, b)]
