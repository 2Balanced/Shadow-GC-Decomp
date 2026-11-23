"""Wave 40: UpdateSpring's loop (retail loads the pull factor, then the spring, then the old value
and the damping factor, and keeps the spring in f4)."""
SCORE = ["UpdateSpring__Q27Vehicle15TVehicleCarMoveFv"]
L = """        float spring = state.m_wheels[i].m_spring;
        float old = state.m_wheels[i].m_springOld;
        state.m_wheels[i].m_springOld = spring;
        state.m_wheels[i].m_spring = (spring - old) * s_springDamp + -spring * s_springPull;
"""
X = "state.m_wheels[i].m_spring = (spring - old) * s_springDamp + -spring * s_springPull;"
def e(x): return [(X, "state.m_wheels[i].m_spring = " + x + ";")]
REPL = {
  "A": e("-spring * s_springPull + (spring - old) * s_springDamp"),
  "B": e("s_springDamp * (spring - old) + s_springPull * -spring"),
  "C": e("s_springPull * -spring + s_springDamp * (spring - old)"),
  "D": e("(spring - old) * s_springDamp - spring * s_springPull"),
  "E": e("(spring - old) * s_springDamp + s_springPull * -spring"),
  "F": e("s_springDamp * (spring - old) + -spring * s_springPull"),
  "G": [(L, """        float pull = s_springPull;
        float spring = state.m_wheels[i].m_spring;
        float old = state.m_wheels[i].m_springOld;
        state.m_wheels[i].m_springOld = spring;
        state.m_wheels[i].m_spring = (spring - old) * s_springDamp + -spring * pull;
""")],
  "H": [(L, """        float spring = state.m_wheels[i].m_spring;
        float force = -spring * s_springPull;
        float old = state.m_wheels[i].m_springOld;
        state.m_wheels[i].m_springOld = spring;
        state.m_wheels[i].m_spring = (spring - old) * s_springDamp + force;
""")],
  "I": [(L, """        float spring = state.m_wheels[i].m_spring;
        float speed = spring - state.m_wheels[i].m_springOld;
        state.m_wheels[i].m_springOld = spring;
        state.m_wheels[i].m_spring = speed * s_springDamp + -spring * s_springPull;
""")],
  "J": [(L, """        float spring = state.m_wheels[i].m_spring;
        float force = -spring * s_springPull + (spring - state.m_wheels[i].m_springOld) * s_springDamp;
        state.m_wheels[i].m_springOld = spring;
        state.m_wheels[i].m_spring = force;
""")],
  "K": [(L, """        float spring = state.m_wheels[i].m_spring;
        float force = (spring - state.m_wheels[i].m_springOld) * s_springDamp + -spring * s_springPull;
        state.m_wheels[i].m_springOld = spring;
        state.m_wheels[i].m_spring = force;
""")],
}

def k(decl, expr):
    return [(L, "        float spring = state.m_wheels[i].m_spring;\n" + decl + "        float force = " + expr + ";\n        state.m_wheels[i].m_springOld = spring;\n        state.m_wheels[i].m_spring = force;\n")]
O = "state.m_wheels[i].m_springOld"
OL = "        float old = state.m_wheels[i].m_springOld;\n"
REPL = {
  "K1": k("", "s_springDamp * (spring - %s) + -spring * s_springPull" % O),
  "K2": k(OL, "(spring - old) * s_springDamp + -spring * s_springPull"),
  "K3": k(OL, "s_springDamp * (spring - old) + -spring * s_springPull"),
  "K4": k("", "(spring - %s) * s_springDamp + s_springPull * -spring" % O),
  "K5": k("", "(spring - %s) * s_springDamp - spring * s_springPull" % O),
  "K6": k("        float speed = spring - %s;\n" % O, "speed * s_springDamp + -spring * s_springPull"),
  "K7": k("        float pull = -spring * s_springPull;\n", "(spring - %s) * s_springDamp + pull" % O),
  "K8": k("        float pull = -spring * s_springPull;\n" + OL, "(spring - old) * s_springDamp + pull"),
  "K9": k(OL + "        float pull = -spring * s_springPull;\n", "(spring - old) * s_springDamp + pull"),
}
