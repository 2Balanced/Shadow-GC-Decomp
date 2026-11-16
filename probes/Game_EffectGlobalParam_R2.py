"""Round 2 for EffectGlobalParam's constructor (wave 39): how the description's fields are read."""
SCORE = ["__ct__Q36Effect6System17EffectGlobalParamFPCQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager"]
OLD = """    const UnkGlobalParamDesc_8042A6E4 *params =
        reinterpret_cast<const UnkGlobalParamDesc_8042A6E4 *>(desc);
    m_data = params->m_data;
    field_0x104 = params->m_duration;
    field_0xFC = params->m_flags;
"""
F = "EffectGlobalParam::EffectGlobalParam(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,"
HELP = """template <class T> inline T DescAt_(const void *desc, int offset) { return *(const T *)((const u8 *)desc + offset); }
template <class T> inline void DescRead_(const void *desc, int offset, T *out) { *out = *(const T *)((const u8 *)desc + offset); }
template <class T> inline void Copy_(T *dst, const T *src) { *dst = *src; }
"""
P = "    const UnkGlobalParamDesc_8042A6E4 *params =\n        reinterpret_cast<const UnkGlobalParamDesc_8042A6E4 *>(desc);\n"
REPL = {
  "bytes": [(OLD, "    const u8 *p = (const u8 *)desc;\n    m_data = *(u32 *)(p + 0x34);\n    field_0x104 = *(float *)(p + 0x30);\n    field_0xFC = *(u32 *)(p + 0x2C);\n")],
  "desc_at": [(F, HELP + F), (OLD, "    m_data = DescAt_<u32>(desc, 0x34);\n    field_0x104 = DescAt_<float>(desc, 0x30);\n    field_0xFC = DescAt_<u32>(desc, 0x2C);\n")],
  "desc_read": [(F, HELP + F), (OLD, "    DescRead_(desc, 0x34, &m_data);\n    DescRead_(desc, 0x30, &field_0x104);\n    DescRead_(desc, 0x2C, &field_0xFC);\n")],
  "copy_fn": [(F, HELP + F), (OLD, P + "    Copy_(&m_data, &params->m_data);\n    Copy_(&field_0x104, &params->m_duration);\n    Copy_(&field_0xFC, &params->m_flags);\n")],
  "this_arrow": [(OLD, P + "    this->m_data = params->m_data;\n    this->field_0x104 = params->m_duration;\n    this->field_0xFC = params->m_flags;\n")],
}
