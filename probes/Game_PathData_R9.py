D = "#define MSL_BINDER_OUTLINE\n"
REPL = {}
for b in range(0, 9):
    for c in range(0, 6):
        REPL["b%dc%d" % (b, c)] = [(D, "#define MSL_BIND_V %d\n#define MSL_CT_V %d\n" % (b, c))]
SCORE = ["fn_8001B42C", ("__ct__Q23std31binder2nd<Q23std12equal_to<Pv>>FRCQ23std12equal_to<Pv>RCPv", "bind2nd<Q23std12equal_to<Pv>,Pv>__3stdFRCQ23std12equal_to<Pv>RCPv")]
