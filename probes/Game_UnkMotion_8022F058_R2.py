"""Wave 40, round 2: ProcessEvents as a static member taking the Impl (this is then a parameter that
can be declared const or not)."""
_ns = {}
exec(compile(open("probes/Game_UnkMotion_8022F058.py", encoding="utf-8").read(), "r1", "exec"), _ns)
PE = _ns["PE"]; body = _ns["body"]; D = _ns["D"]; SW = _ns["SW"]; VC = _ns["VC"]
OLD_DECL = "    void ProcessEvents(_stMtnPlayWork *work);"
OLD_CALL = "            ProcessEvents(part->m_motion);"
_src = open("src/Game/UnkMotion_8022F058.cpp", encoding="utf-8").read()
_a = _src.index("void UnkMotion_8022F058::Impl::ProcessEvents(_stMtnPlayWork *work) {")
OLD_HEAD = _src[_a:_src.index("void UnkMotion_8022F058::Impl::LoadParts(")]
NEWSYM = "ProcessEvents__Q218UnkMotion_8022F0584ImplFPQ218UnkMotion_8022F0584ImplP14_stMtnPlayWork"
SCORE = [(PE, NEWSYM)]
def static(selfdecl, sw, pre=""):
    return [(OLD_DECL, "    static void ProcessEvents(%s, _stMtnPlayWork *work);" % selfdecl),
            (OLD_CALL, "            ProcessEvents(this, part->m_motion);"),
            (OLD_HEAD, "void UnkMotion_8022F058::Impl::ProcessEvents(%s, _stMtnPlayWork *work) {\n" % selfdecl + body(D, self="self", sw=sw, pre=pre) + "}\n\n")]
REPL = {
    "st_plain": static("Impl *self", SW),
    "st_plain_vc": static("Impl *self", VC),
    "st_const": static("Impl *const self", SW),
    "st_const_vc": static("Impl *const self", VC),
}
