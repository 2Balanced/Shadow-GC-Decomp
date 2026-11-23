"""Wave 41: SetMaterialAlpha's alpha in r6 (GC/2.7: r0), with copy propagation off."""
SCORE = ["SetMaterialAlpha__FP10RpMaterialPv"]
F = "static RpMaterial *SetMaterialAlpha(RpMaterial *material, void *data) {\n"
OLD = F + "    if (data != 0) {\n        material->color.alpha = *(u8 *)data;\n    }\n    return material;\n}\n"
P0 = "#pragma opt_propagation off\n"
P1 = "#pragma opt_propagation reset\n"
REPL = {
    "prag": [(OLD, P0 + OLD + P1)],
    "prag_alpha": [(OLD, P0 + F + "    u8 *alpha = (u8 *)data;\n    if (alpha != 0) {\n        material->color.alpha = *alpha;\n    }\n    return material;\n}\n" + P1)],
    "prag_alpha2": [(OLD, P0 + F + "    if (data != 0) {\n        u8 *alpha = (u8 *)data;\n        material->color.alpha = *alpha;\n    }\n    return material;\n}\n" + P1)],
    "prag_mat": [(OLD, P0 + F + "    RpMaterial *m = material;\n    if (data != 0) {\n        m->color.alpha = *(u8 *)data;\n    }\n    return material;\n}\n" + P1)],
    "prag_both": [(OLD, P0 + F + "    RpMaterial *m = material;\n    u8 *alpha = (u8 *)data;\n    if (alpha != 0) {\n        m->color.alpha = *alpha;\n    }\n    return m;\n}\n" + P1)],
    "prag_val": [(OLD, P0 + F + "    if (data != 0) {\n        u8 alpha = *(u8 *)data;\n        material->color.alpha = alpha;\n    }\n    return material;\n}\n" + P1)],
    "prag_col": [(OLD, P0 + F + "    if (data != 0) {\n        RwRGBA *color = &material->color;\n        color->alpha = *(u8 *)data;\n    }\n    return material;\n}\n" + P1)],
    "col": [(OLD, F + "    if (data != 0) {\n        RwRGBA *color = &material->color;\n        color->alpha = *(u8 *)data;\n    }\n    return material;\n}\n")],
}
