"""Wave 41: fn_801E7094's final test (retail: cmplwi/bne with two li, GC/2.7: addic/subfe)."""
F = "fn_801E7094"
SCORE = [F]
HEAD = 'extern "C" bool fn_801E7094(const boost::weak_ptr<Target> *target) {\n'
OLD = """    u32 active = target->lock()->IsActive();
    if (active == 0) {
        return false;
    }
    return true;
}
"""
OFF = [(HEAD, "#pragma opt_propagation off\n" + HEAD), (OLD, OLD + "#pragma opt_propagation reset\n")]
def v(new, extra=None):
    return (extra or []) + [(OLD, new)]
REPL = {
    "switch": v("""    switch (target->lock()->IsActive()) {
    case 0:
        return false;
    }
    return true;
}
"""),
    "switch_def": v("""    switch (target->lock()->IsActive()) {
    case 0:
        return false;
    default:
        return true;
    }
}
"""),
    "goto": v("""    u32 active = target->lock()->IsActive();
    if (active != 0) {
        goto yes;
    }
    return false;
yes:
    return true;
}
"""),
    "off_result": v("""    bool result;
    u32 active = target->lock()->IsActive();
    if (active == 0) {
        result = false;
    } else {
        result = true;
    }
    return result;
}
""", OFF),
    "off_base": v(OLD, OFF),
    "off_result_init": v("""    bool result = true;
    u32 active = target->lock()->IsActive();
    if (active == 0) {
        result = false;
    }
    return result;
}
""", OFF),
    "result_init": v("""    bool result = true;
    u32 active = target->lock()->IsActive();
    if (active == 0) {
        result = false;
    }
    return result;
}
"""),
    "result_init_false": v("""    bool result = false;
    if (target->lock()->IsActive()) {
        result = true;
    }
    return result;
}
"""),
}
