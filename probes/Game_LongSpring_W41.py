"""Wave 41: LongSpring's HitCharacter speed interpolation. Retail shares one stack slot between
the fctiwz and the int-to-float conversion; GC/2.7 does share it when a conditional block with a
side effect stands between the two (found with TGiHdGage::Tick), so try shapes that put the
interpolation in another block structure."""
H = "HitCharacter__10LongSpringFQ25boost21shared_ptr<8CharInfo>Pv"
SCORE = [H]
OLD = """    int level = context->m_params->level;
    float speed = lbl_804BC388[level];
    if (level < 15) {
        speed += (lbl_804BC388[level + 1] - speed) * (context->m_params->level - level);
    }
"""
HELPER = """/* The speed for a level, interpolated between the table's entries (name inferred). */
inline float SpeedAt(float x) {
    int level = x;
    float speed = lbl_804BC388[level];
    if (level < 15) {
        speed += (lbl_804BC388[level + 1] - speed) * (x - level);
    }
    return speed;
}

"""
ANCH = "static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {"
REPL = {
    "inline_x": [(ANCH, HELPER + ANCH), (OLD, "    float speed = SpeedAt(context->m_params->level);\n")],
    "else_empty": [(OLD, OLD.replace("    }\n", "    } else {\n    }\n"))],
    "rev_if": [(OLD, """    int level = context->m_params->level;
    float speed = lbl_804BC388[level];
    if (level >= 15) {
    } else {
        speed += (lbl_804BC388[level + 1] - speed) * (context->m_params->level - level);
    }
""")],
    "if_else_assign": [(OLD, """    int level = context->m_params->level;
    float speed;
    if (level < 15) {
        speed = lbl_804BC388[level] + (lbl_804BC388[level + 1] - lbl_804BC388[level]) * (context->m_params->level - level);
    } else {
        speed = lbl_804BC388[level];
    }
""")],
    "flevel": [(OLD, """    float x = context->m_params->level;
    int level = x;
    float speed = lbl_804BC388[level];
    if (level < 15) {
        speed += (lbl_804BC388[level + 1] - speed) * (x - level);
    }
""")],
}

# Round 2: rev_if shares the slot (only f2/f3 numbering left); vary the interpolation's spelling.
def rv(body):
    return [(OLD, "    int level = context->m_params->level;\n    float speed = lbl_804BC388[level];\n    if (level >= 15) {\n    } else {\n" + body + "    }\n")]
REPL.update({
    "rv_a": rv("        speed += (lbl_804BC388[level + 1] - speed) * (context->m_params->level - level);\n"),
    "rv_b": rv("        speed = speed + (lbl_804BC388[level + 1] - speed) * (context->m_params->level - level);\n"),
    "rv_c": rv("        speed += (context->m_params->level - level) * (lbl_804BC388[level + 1] - speed);\n"),
    "rv_d": rv("        float frac = context->m_params->level - level;\n        speed += (lbl_804BC388[level + 1] - speed) * frac;\n"),
    "rv_e": rv("        float next = lbl_804BC388[level + 1];\n        speed += (next - speed) * (context->m_params->level - level);\n"),
    "rv_f": rv("        float diff = lbl_804BC388[level + 1] - speed;\n        speed += diff * (context->m_params->level - level);\n"),
    "rv_g": [(OLD, "    float x = context->m_params->level;\n    int level = x;\n    float speed = lbl_804BC388[level];\n    if (level >= 15) {\n    } else {\n        speed += (lbl_804BC388[level + 1] - speed) * (x - level);\n    }\n")],
    "rv_h": [(ANCH, HELPER.replace("    if (level < 15) {\n        speed", "    if (level >= 15) {\n    } else {\n        speed") + ANCH), (OLD, "    float speed = SpeedAt(context->m_params->level);\n")],
})

# Round 3: other block structures.
def whole(body):
    return [(OLD, body)]
REPL.update({
    "tern": whole("    int level = context->m_params->level;\n    float speed = lbl_804BC388[level];\n    speed = level < 15 ? speed + (lbl_804BC388[level + 1] - speed) * (context->m_params->level - level) : speed;\n"),
    "nested": whole("    int level = context->m_params->level;\n    float speed = lbl_804BC388[level];\n    if (level < 15) {\n        if (true) {\n            speed += (lbl_804BC388[level + 1] - speed) * (context->m_params->level - level);\n        }\n    }\n"),
    "rv_xfirst": whole("    float x;\n    int level = (x = context->m_params->level);\n    float speed = lbl_804BC388[level];\n    if (level >= 15) {\n    } else {\n        speed += (lbl_804BC388[level + 1] - speed) * (x - level);\n    }\n"),
    "rv_le14": whole("    int level = context->m_params->level;\n    float speed = lbl_804BC388[level];\n    if (!(level < 15)) {\n    } else {\n        speed += (lbl_804BC388[level + 1] - speed) * (context->m_params->level - level);\n    }\n"),
    "rv_semi": whole("    int level = context->m_params->level;\n    float speed = lbl_804BC388[level];\n    if (level >= 15)\n        ;\n    else\n        speed += (lbl_804BC388[level + 1] - speed) * (context->m_params->level - level);\n"),
    "rv_tbl": whole("    int level = context->m_params->level;\n    const float *table = lbl_804BC388;\n    float speed = table[level];\n    if (level >= 15) {\n    } else {\n        speed += (table[level + 1] - speed) * (context->m_params->level - level);\n    }\n"),
    "rv_params": whole("    LongSpringParams *params = context->m_params;\n    int level = params->level;\n    float speed = lbl_804BC388[level];\n    if (level >= 15) {\n    } else {\n        speed += (lbl_804BC388[level + 1] - speed) * (params->level - level);\n    }\n"),
})

# Round 4: the difference to the next entry as its own variable, reduced by a compound assignment:
# GC/2.7 then shares the conversion slot and numbers the floats as retail (build/t experiments).
REPL.update({
    "subassign": whole("    int level = context->m_params->level;\n    float speed = lbl_804BC388[level];\n    if (level < 15) {\n        float delta = lbl_804BC388[level + 1];\n        delta -= speed;\n        speed += delta * (context->m_params->level - level);\n    }\n"),
})
