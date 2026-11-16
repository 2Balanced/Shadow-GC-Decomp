"""Source shapes for fn_80015B60 of Game/ClumpUpdater.cpp (wave 39): retail negates the angle into
f26 (the dead `sum`) and keeps f28; GC/2.7 negates in place (fneg f28,f28)."""
SCORE = ["fn_80015B60"]
TAIL = """    fn_8040B884(b, -angle);
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, -length1);
    fn_8040B884(c, -angle);
    return distance;
}

}
"""
def t(new): return [(TAIL, new + "\n}\n")]
REPL = {
  "live_after": t("""    fn_8040B884(b, -angle);
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, -length1);
    fn_8040B884(c, -angle);
    return distance + angle;
}
"""),
  "neg_named": t("""    float neg = -angle;
    fn_8040B884(b, neg);
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, -length1);
    fn_8040B884(c, neg);
    return distance;
}
"""),
  "second_from_first": t("""    float neg = -angle;
    fn_8040B884(b, neg);
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, -length1);
    fn_8040B884(c, -angle);
    return distance;
}
"""),
  "first_expr_second_named": t("""    fn_8040B884(b, -angle);
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, -length1);
    float neg = -angle;
    fn_8040B884(c, neg);
    return distance;
}
"""),
  "sum_reuse": t("""    sum = -angle;
    fn_8040B884(b, sum);
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, -length1);
    fn_8040B884(c, sum);
    return distance;
}
"""),
  "const_angle": [("    float angle = fn_80015D68(t);", "    const float angle = fn_80015D68(t);")],
  "volatile_free": t("""    fn_8040B884(b, -angle);
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, -length1);
    fn_8040B884(c, -(float)angle);
    return distance;
}
"""),
  "zero_minus": t("""    fn_8040B884(b, -angle);
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, -length1);
    fn_8040B884(c, -1.0f * angle);
    return distance;
}
"""),
}
