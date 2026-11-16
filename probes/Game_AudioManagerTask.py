"""FindFreeChannel of Game/AudioManagerTask.cpp (wave 39, second pass)."""
SCORE = ["FindFreeChannel__17UnkSound_80053164FP17UnkSoundBankEntryUc"]
TAIL = [("""                if (m_audio != 0) {
                    m_audio->vf_0x48(left, right);
                }
""", "                Audio48(m_audio, left, right);\n"),
        ("""            if (m_audio != 0) {
                m_audio->vf_0x30(left, right);
            }
        }
    }
    return found;
""", "            Audio30(m_audio, left, right);\n        }\n    }\n    return found;\n"),
 ("s8 UnkSound_80053164::FindFreeChannel(", """inline void Audio48(IAudio *audio, s8 group, u8 voice) {
    if (audio != 0) {
        audio->vf_0x48(group, voice);
    }
}
inline void Audio30(IAudio *audio, s8 group, u8 voice) {
    if (audio != 0) {
        audio->vf_0x30(group, voice);
    }
}
s8 UnkSound_80053164::FindFreeChannel(""")]
DECL = """        s8 count = 0x2B;
        UnkSoundChannel_801D98FC *channel = &m_channels[index];
        s8 oldest = -1;
        s8 lower = -1;
        u16 age;
"""
CMP = """            if (channel->m_priority < priority) {
                lower = index;
            } else if (priority == channel->m_priority) {
"""
D1 = (DECL, """        UnkSoundChannel_801D98FC *channel = &m_channels[index];
        s8 oldest = -1;
        s8 lower = -1;
        s8 count = 0x2B;
        u16 age;
""")
D2 = (DECL, """        s8 oldest = -1;
        s8 lower = -1;
        s8 count = 0x2B;
        UnkSoundChannel_801D98FC *channel = &m_channels[index];
        u16 age;
""")
D3 = (DECL, """        s8 oldest = -1;
        s8 lower = -1;
        UnkSoundChannel_801D98FC *channel = &m_channels[index];
        s8 count = 0x2B;
        u16 age;
""")
CA = (CMP, """            u32 p = channel->m_priority;
            if (p < priority) {
                lower = index;
            } else if (priority == (u8)p) {
""")
CB = (CMP, """            int p = channel->m_priority;
            if (p < priority) {
                lower = index;
            } else if (priority == (u8)p) {
""")
CC = (CMP, """            if (channel->m_priority < priority) {
                lower = index;
            } else if (priority == (u8)channel->m_priority) {
""")

GET = ("inline void Audio48(", """inline u8 Priority(const UnkSoundChannel_801D98FC *c) {
    return c->m_priority;
}
inline bool SamePriority(u8 a, u8 b) {
    return a == b;
}
inline bool SameAs(const UnkSoundChannel_801D98FC *c, u8 b) {
    return c->m_priority == b;
}
inline void Audio48(""")
def cmp(a, b):
    return (CMP, """            if (%s) {
                lower = index;
            } else if (%s) {
""" % (a, b))
F = "channel->m_priority"
G = "Priority(channel)"

BASE = TAIL + [GET, cmp(G + " < priority", "priority == " + G)]
L1 = """        s8 index = m_nextVoice;
""" + DECL
L2 = """        s8 index = m_nextChannel;
        s8 lower = -1;
        s8 oldest = -1;
        u16 age = 0;
        s8 count = 0x20;
        UnkSoundChannel_801D98FC *channels = m_channels;
        UnkSoundChannel_801D98FC *channel = &channels[index];
"""
C = "UnkSoundChannel_801D98FC"
L1V = {
 "a": """        s8 index = m_nextVoice;
        s8 count;
        %s *channel = &m_channels[index];
        s8 oldest = -1;
        s8 lower = -1;
        u16 age;
        count = 0x2B;
""" % C,
 "b": """        %s *channels;
        s8 index;
        s8 count;
        %s *channel;
        s8 oldest;
        s8 lower;
        u16 age;
        index = m_nextVoice;
        channels = m_channels;
        oldest = -1;
        lower = -1;
        count = 0x2B;
        channel = &channels[index];
""" % (C, C),
 "c": """        %s *channels;
        s8 index;
        s8 count;
        %s *channel;
        s8 oldest;
        s8 lower;
        u16 age;
        index = m_nextVoice;
        channels = m_channels;
        oldest = -1;
        channel = &channels[index];
        lower = -1;
        count = 0x2B;
""" % (C, C),
 "d": """        s8 index;
        s8 count;
        %s *channel;
        s8 oldest;
        s8 lower;
        u16 age;
        index = m_nextVoice;
        oldest = -1;
        lower = -1;
        count = 0x2B;
        channel = &m_channels[index];
""" % (C),
}
L2V = {
 "a": """        %s *channels;
        s8 index;
        %s *channel;
        s8 count;
        s8 lower;
        s8 oldest;
        u16 age;
        index = m_nextChannel;
        lower = -1;
        oldest = -1;
        age = 0;
        count = 0x20;
        channels = m_channels;
        channel = &channels[index];
""" % (C, C),
 "b": """        %s *channels = m_channels;
        s8 index = m_nextChannel;
        %s *channel = &channels[index];
        s8 count = 0x20;
        s8 lower = -1;
        s8 oldest = -1;
        u16 age = 0;
""" % (C, C),
}
REPL = {"base": BASE}
for k, v in L1V.items():
    REPL["l1" + k] = BASE + [(L1, v)]
for k, v in L2V.items():
    REPL["l2" + k] = BASE + [(L2, v)]
for k, v in L1V.items():
    REPL["l1" + k + "2a"] = BASE + [(L1, v), (L2, L2V["a"])]
REPL = {}
BASE2 = TAIL + [GET, (L2, L2V["a"])]
for ty in ("u32", "int", "u8"):
    for pos in ("first", "second"):
        l1 = L1V["a"]
        decl = "        %s prio = priority;\n" % ty
        if pos == "first":
            l1 = decl + l1
        else:
            l1 = l1.replace("        s8 count;\n", decl + "        s8 count;\n")
        for cn, (a, b) in {"ff": (F, F), "gg": (G, G), "fg": (F, G), "fu": (F, "(u8)" + F)}.items():
            REPL["p_%s_%s_%s" % (ty, pos, cn)] = BASE2 + [(L1, l1), cmp(a + " < prio", "prio == " + b)]
REPL = {}
HEAD = """    s8 found = -1;
    u16 id = entry->m_id;
    if (entry->m_flags & 2) {
"""
GG = cmp(G + " < prio", "prio == " + G)
HV = {
 "top": "    u8 prio = priority;\n" + HEAD,
 "mid": HEAD.replace("    u16 id", "    u8 prio = priority;\n    u16 id"),
 "aft": HEAD.replace("    if (entry", "    u8 prio = priority;\n    if (entry"),
 "top_u32": "    u32 prio = priority;\n" + HEAD,
 "aft_u32": HEAD.replace("    if (entry", "    u32 prio = priority;\n    if (entry"),
 "aft_int": HEAD.replace("    if (entry", "    int prio = priority;\n    if (entry"),
}
for k, v in HV.items():
    REPL["h_" + k] = BASE2 + [(L1, L1V["a"]), GG, (HEAD, v)]
    REPL["h_" + k + "_ff"] = BASE2 + [(L1, L1V["a"]), cmp(F + " < prio", "prio == " + F), (HEAD, v)]
    REPL["h_" + k + "_fu"] = BASE2 + [(L1, L1V["a"]), cmp(F + " < prio", "prio == (u8)" + F), (HEAD, v)]
REPL = {}
AGE = """                } else if (age < channel->m_age) {
                    oldest = index;
                    age = channel->m_age;
                }
"""
B3 = BASE2 + [(L1, L1V["a"]), cmp(G + " < priority", "priority == " + G)]
AV = {
 "named": """                } else {
                    u16 a = channel->m_age;
                    if (age < a) {
                        oldest = index;
                        age = a;
                    }
                }
""",
 "swap": """                } else if (channel->m_age > age) {
                    oldest = index;
                    age = channel->m_age;
                }
""",
 "ageFirst": """                } else if (age < channel->m_age) {
                    age = channel->m_age;
                    oldest = index;
                }
""",
 "getter": """                } else if (age < Age(channel)) {
                    oldest = index;
                    age = Age(channel);
                }
""",
 "u32": """                } else if ((u32)age < channel->m_age) {
                    oldest = index;
                    age = channel->m_age;
                }
""",
}
for k, v in AV.items():
    REPL["age_" + k] = B3 + [(AGE, v), ("inline bool SamePriority", "inline u16 Age(const UnkSoundChannel_801D98FC *c) {\n    return c->m_age;\n}\ninline bool SamePriority")]
