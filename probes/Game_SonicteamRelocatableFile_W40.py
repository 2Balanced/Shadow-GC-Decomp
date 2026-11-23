"""UnkPath_8040E61C's constructor (wave 40): retail keeps the shared byte-order flag of the two
table-size reads in r5, the word in r3 and the read value in r0 (then `slwi r5,r0,3`); GC/2.7 gave
the flag r3, the word r0 and the value r5 (`slwi r5,r5,3`)."""
SCORE = ["__ct__16UnkPath_8040E61CFPv"]
OLD = """    u32 sizes[2];
    sizes[0] = 8 * Read(header[3]);
    sizes[1] = 8 * Read(header[4]);
"""
RD = """    u32 Read(u32 value) {
        return m_bigEndian ? value
                           : (u32)(((value & 0xFF) << 24) | ((value & 0xFF00) << 8) | ((value >> 8) & 0xFF00) |
                                   ((value >> 24) & 0xFF));
    }
"""
SW = "(u32)(((value & 0xFF) << 24) | ((value & 0xFF00) << 8) | ((value >> 8) & 0xFF00) | ((value >> 24) & 0xFF))"
REPL = {
  "big_local": [(OLD, """    u32 sizes[2];
    bool big = m_bigEndian;
    sizes[0] = 8 * FileWord(header[3], big);
    sizes[1] = 8 * FileWord(header[4], big);
""")],
  "loop2": [(OLD, """    u32 sizes[2];
    for (int i = 0; i < 2; i++) {
        sizes[i] = 8 * Read(header[3 + i]);
    }
""")],
  "read_ifelse": [(RD, """    u32 Read(u32 value) {
        if (m_bigEndian) {
            return value;
        }
        return """ + SW + """;
    }
""")],
  "read_const": [(RD, RD.replace("u32 Read(u32 value) {", "u32 Read(u32 value) const {"))],
  "read_ref": [(RD, RD.replace("u32 Read(u32 value) {", "u32 Read(const u32 &value) {"))],
  "read_ptr": [(RD, """    u32 Read(const u32 *p) {
        u32 value = *p;
        return m_bigEndian ? value : """ + SW + """;
    }
"""), (OLD, """    u32 sizes[2];
    sizes[0] = 8 * Read(&header[3]);
    sizes[1] = 8 * Read(&header[4]);
""")],
  "tmp_then_mul": [(OLD, """    u32 sizes[2];
    u32 n0 = Read(header[3]);
    u32 n1 = Read(header[4]);
    sizes[0] = 8 * n0;
    sizes[1] = 8 * n1;
""")],
  "fileword_member": [(OLD, """    u32 sizes[2];
    sizes[0] = 8 * FileWord(header[3], m_bigEndian);
    sizes[1] = 8 * FileWord(header[4], m_bigEndian);
""")],
  "sizes_mul_right": [(OLD, """    u32 sizes[2];
    sizes[0] = Read(header[3]) * 8;
    sizes[1] = Read(header[4]) * 8;
""")],
  "sizes_u8ptr": [(OLD, """    u32 sizes[2];
    sizes[0] = (u32)((UnkRelocatableEntry_8040E4BC *)0 + Read(header[3]));
    sizes[1] = (u32)((UnkRelocatableEntry_8040E4BC *)0 + Read(header[4]));
""")],
}
