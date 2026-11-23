"""fn_80412BFC (wave 40): retail keeps the bare name in r31, above the parameters; GC/2.7 gave it
r27. The name as a compiler temporary: the search for the last separator inlined from a function."""
SCORE = ["fn_80412BFC"]
O = """    const char *name = path;
    char *found = fn_803AC6C0(path, separator);
    while (found != 0) {
        name = found + strlen(separator);
        found = fn_803AC6C0(name, separator);
    }
"""
ANCH = "u16 fn_80412BFC(void *data, const char *path, void (*release)(void *data), int autoId) {\n"
H = """/* The file name of path, without its directories (name inferred). */
inline const char *BaseName(const char *path, const char *separator) {
    const char *name = path;
    char *found = fn_803AC6C0(path, separator);
    while (found != 0) {
        name = found + strlen(separator);
        found = fn_803AC6C0(name, separator);
    }
    return name;
}

"""
REPL = {
  "inl_base": [(ANCH, H + ANCH), (O, "    const char *name = BaseName(path, separator);\n")],
  "inl_base_assign": [(ANCH, H + ANCH), (O, "    const char *name;\n    name = BaseName(path, separator);\n")],
}
