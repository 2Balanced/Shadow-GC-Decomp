# Probes for Game/RwCpp46: strchr's character register (lbz r0 / extsb r0,r0 twice).
A = """    char value = (char)c;
    do {
        if (*text == value) {
            found = text;
            break;
        }
    } while (*text++ != 0);
    return found;"""
def body(decl, load, cmp, test):
    return [(A, """    char value = (char)c;
    %s
    do {
        %s
        if (%s) {
            found = text;
            break;
        }
        text++;
    } while (%s);
    return found;""" % (decl, load, cmp, test))]
REPL = {
    "base": [],
    "char": body("char current;", "current = *text;", "current == value", "current != 0"),
    "int": body("int current;", "current = *text;", "(char)current == value", "(char)current != 0"),
    "int2": body("int current;", "current = *text;", "current == value", "(char)current != 0"),
    "u8": body("u8 current;", "current = *(u8 *)text;", "(char)current == value", "(char)current != 0"),
    "charc": [(A, """    char current;
    do {
        current = *text;
        if (current == (char)c) {
            found = text;
            break;
        }
        text++;
    } while (current != 0);
    return found;""")],
    "nob": [(A, """    char value = (char)c;
    do {
        if (*text == value) {
            found = text;
            break;
        }
    } while (*text++);
    return found;""")],
    "while": [(A, """    char value = (char)c;
    for (;;) {
        if (*text == value) {
            found = text;
            break;
        }
        if (*text++ == 0) {
            break;
        }
    }
    return found;""")],
}
SCORE = ["fn_80482DBC"]
