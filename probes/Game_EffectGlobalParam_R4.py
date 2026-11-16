"""Round 4 for EffectGlobalParam's constructor (wave 39): the parameter stays a pointer to const (so
that the symbol and the derived class are untouched); ways to read the fields through a pointer to
non-const that GC/2.7 does not trace back to it."""
SCORE = ["__ct__Q36Effect6System17EffectGlobalParamFPCQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager"]
OLD = """    const UnkGlobalParamDesc_8042A6E4 *params =
        reinterpret_cast<const UnkGlobalParamDesc_8042A6E4 *>(desc);
    m_data = params->m_data;
    field_0x104 = params->m_duration;
    field_0xFC = params->m_flags;
"""
BODY = "    m_data = params->m_data;\n    field_0x104 = params->m_duration;\n    field_0xFC = params->m_flags;\n"
F = "EffectGlobalParam::EffectGlobalParam(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,"
HELP = """inline UnkGlobalParamDesc_8042A6E4 *AsParams_(const UnkEffectDesc_801D0AFC *desc) { return (UnkGlobalParamDesc_8042A6E4 *)desc; }
inline UnkGlobalParamDesc_8042A6E4 *AsParams2_(void *desc) { return (UnkGlobalParamDesc_8042A6E4 *)desc; }
"""
REPL = {
  "int_launder": [(OLD, "    UnkGlobalParamDesc_8042A6E4 *params = (UnkGlobalParamDesc_8042A6E4 *)(u32)desc;\n" + BODY)],
  "void_launder": [(OLD, "    void *raw = (void *)desc;\n    UnkGlobalParamDesc_8042A6E4 *params = (UnkGlobalParamDesc_8042A6E4 *)raw;\n" + BODY)],
  "inline_cast": [(F, HELP + F), (OLD, "    UnkGlobalParamDesc_8042A6E4 *params = AsParams_(desc);\n" + BODY)],
  "inline_cast2": [(F, HELP + F), (OLD, "    UnkGlobalParamDesc_8042A6E4 *params = AsParams2_((void *)desc);\n" + BODY)],
  "volatile_like": [(OLD, "    UnkGlobalParamDesc_8042A6E4 *params;\n    params = (UnkGlobalParamDesc_8042A6E4 *)desc;\n" + BODY)],
  "m_desc": [(OLD, "    UnkGlobalParamDesc_8042A6E4 *params = (UnkGlobalParamDesc_8042A6E4 *)m_desc;\n" + BODY)],
}
