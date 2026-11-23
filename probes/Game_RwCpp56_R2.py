"""Wave 40 (r7): fn_80493888 (RwImageFindRasterFormat driver side): the size clamp and the OR."""

OLD = """        RwInt32 width = 1024;
        RwInt32 height;
        if (image->width <= 1024) width = image->width;
        raster->width = width;
        height = 1024;
        if (image->height <= 1024) height = image->height;
        raster->height = height;"""
OLDOR = "raster->cFormat = (RwInt32)((format & 0x9000) | fn_8049375C(image)) >> 8;"
NEWOR = "raster->cFormat = (RwInt32)(fn_8049375C(image) | (format & 0x9000)) >> 8;"

CLAMPS = {
    "tern": """        raster->width = (image->width > 1024) ? 1024 : image->width;
        raster->height = (image->height > 1024) ? 1024 : image->height;""",
    "tern2": """        raster->width = (image->width < 1024) ? image->width : 1024;
        raster->height = (image->height < 1024) ? image->height : 1024;""",
    "tern3": """        raster->width = (image->width <= 1024) ? image->width : 1024;
        raster->height = (image->height <= 1024) ? image->height : 1024;""",
    "loc_tern": """        RwInt32 width;
        RwInt32 height;
        width = (image->width > 1024) ? 1024 : image->width;
        raster->width = width;
        height = (image->height > 1024) ? 1024 : image->height;
        raster->height = height;""",
    "ifelse": """        RwInt32 width;
        RwInt32 height;
        if (image->width > 1024) width = 1024; else width = image->width;
        raster->width = width;
        if (image->height > 1024) height = 1024; else height = image->height;
        raster->height = height;""",
    "ifelse_st": """        if (image->width > 1024) raster->width = 1024; else raster->width = image->width;
        if (image->height > 1024) raster->height = 1024; else raster->height = image->height;""",
    "w_loc": """        RwInt32 w = image->width;
        RwInt32 h = image->height;
        raster->width = (w > 1024) ? 1024 : w;
        raster->height = (h > 1024) ? 1024 : h;""",
    "w_loc2": """        RwInt32 w, h;
        w = image->width;
        raster->width = (w > 1024) ? 1024 : w;
        h = image->height;
        raster->height = (h > 1024) ? 1024 : h;""",
}
REPL = {}
for k, v in CLAMPS.items():
    REPL[k] = [(OLD, v)]
    REPL[k + "_or"] = [(OLD, v), (OLDOR, NEWOR)]
REPL["orig_or"] = [(OLDOR, NEWOR)]
SCORE = ["fn_80493888"]
