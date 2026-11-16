"""Chao::CSD::CProject::iterator::operator* (wave 39): retail keeps the first load after the
prologue's stores, which a non-const member gives (GC/2.7 moves loads through a pointer to const
above stores)."""
R = "__ml__Q44Chao3CSD8CProject8iteratorCFv"
SCORE = [(R, "__ml__Q44Chao3CSD8CProject8iteratorFv")]
HDR = {"nonconst": {"Game/CsdProject.h": [("        CScene &operator*() const {\n            return *(*m_it).second;", "        CScene &operator*() {\n            return *(*m_it).second;")]}}
