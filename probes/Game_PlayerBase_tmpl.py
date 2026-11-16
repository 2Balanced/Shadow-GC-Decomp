"""Game/PlayerBase.cpp (wave 39): UnkWords_8007A4D4 as an instance of a class template, to see where
GC/2.7 emits its constructor then."""
SCORE = ["vf_0x0C__8CharInfoFv"]
import copy
H = "Game/UnkWords_8007A4D4.h"
HDR = {
 "tmpl": {H: [("struct UnkWords_8007A4D4 {", "template <int N> struct UnkWordsT_ {"),
              ("    UnkWords_8007A4D4() {", "    UnkWordsT_() {"),
              ("    UnkWords_8007A4D4(const UnkWords_8007A4D4 &other) {", "    UnkWordsT_(const UnkWordsT_ &other) {"),
              ("    UnkWords_8007A4D4 &operator=(const UnkWords_8007A4D4 &other) {", "    UnkWordsT_ &operator=(const UnkWordsT_ &other) {"),
              ("    void Copy(const UnkWords_8007A4D4 &other) {", "    void Copy(const UnkWordsT_ &other) {"),
              ("    /* 0x0 */ u32 m_words[3];\n};", "    /* 0x0 */ u32 m_words[N];\n};\ntypedef UnkWordsT_<3> UnkWords_8007A4D4;"),
              ]},
 "tmpl_out": {H: [("struct UnkWords_8007A4D4 {", "template <int N> struct UnkWordsT_ {"),
              ("""    UnkWords_8007A4D4() {
        for (int i = 0; i < 3; i++) {
            Word(i) = 0;
        }
    }""", "    UnkWordsT_();"),
              ("    UnkWords_8007A4D4(const UnkWords_8007A4D4 &other) {", "    UnkWordsT_(const UnkWordsT_ &other) {"),
              ("    UnkWords_8007A4D4 &operator=(const UnkWords_8007A4D4 &other) {", "    UnkWordsT_ &operator=(const UnkWordsT_ &other) {"),
              ("    void Copy(const UnkWords_8007A4D4 &other) {", "    void Copy(const UnkWordsT_ &other) {"),
              ("    /* 0x0 */ u32 m_words[3];\n};", """    /* 0x0 */ u32 m_words[N];\n};
template <int N> inline UnkWordsT_<N>::UnkWordsT_() {
    for (int i = 0; i < N; i++) {
        Word(i) = 0;
    }
}
typedef UnkWordsT_<3> UnkWords_8007A4D4;"""),
              ]},
}

HDR["tmpl_noinline"] = {H: [(a, b.replace("template <int N> inline UnkWordsT_<N>::UnkWordsT_() {", "template <int N> UnkWordsT_<N>::UnkWordsT_() {").replace("Word(i) = 0;", "m_words[i] = 0;")) for a, b in HDR["tmpl_out"][H]]}
