"""Wave 40: fn_80038780's second half (retail forms fade + 0x10, the alpha factors' base, first)."""
SCORE = ["fn_80038780"]
B = """    color.red = color.red * fade->m_scaleRed + 0.5f;
    color.green = color.green * fade->m_scaleGreen + 0.5f;
    color.blue = color.blue * fade->m_scaleBlue + 0.5f;
    color.alpha = color.alpha * fade->m_alpha[lbl_805F0170] + 0.5f;
    material->color = color;
"""
RGB = """    color.red = color.red * fade->m_scaleRed + 0.5f;
    color.green = color.green * fade->m_scaleGreen + 0.5f;
    color.blue = color.blue * fade->m_scaleBlue + 0.5f;
"""
def v(s): return [(B, s)]
REPL = {
  "alpha_ptr": v("    const float *alphas = fade->m_alpha;\n" + RGB + "    color.alpha = color.alpha * alphas[lbl_805F0170] + 0.5f;\n    material->color = color;\n"),
  "alpha_val": v("    float alpha = fade->m_alpha[lbl_805F0170];\n" + RGB + "    color.alpha = color.alpha * alpha + 0.5f;\n    material->color = color;\n"),
  "alpha_first": v("    color.alpha = color.alpha * fade->m_alpha[lbl_805F0170] + 0.5f;\n" + RGB + "    material->color = color;\n"),
  "idx": v("    int which = lbl_805F0170;\n" + RGB + "    color.alpha = color.alpha * fade->m_alpha[which] + 0.5f;\n    material->color = color;\n"),
  "locals": v("""    u8 red = color.red * fade->m_scaleRed + 0.5f;
    u8 green = color.green * fade->m_scaleGreen + 0.5f;
    u8 blue = color.blue * fade->m_scaleBlue + 0.5f;
    u8 alpha = color.alpha * fade->m_alpha[lbl_805F0170] + 0.5f;
    color.red = red;
    color.green = green;
    color.blue = blue;
    color.alpha = alpha;
    material->color = color;
"""),
  "floats": v("""    float red = color.red * fade->m_scaleRed + 0.5f;
    float green = color.green * fade->m_scaleGreen + 0.5f;
    float blue = color.blue * fade->m_scaleBlue + 0.5f;
    float alpha = color.alpha * fade->m_alpha[lbl_805F0170] + 0.5f;
    color.red = red;
    color.green = green;
    color.blue = blue;
    color.alpha = alpha;
    material->color = color;
"""),
  "direct": v("""    material->color.red = color.red = color.red * fade->m_scaleRed + 0.5f;
    material->color.green = color.green = color.green * fade->m_scaleGreen + 0.5f;
    material->color.blue = color.blue = color.blue * fade->m_scaleBlue + 0.5f;
    material->color.alpha = color.alpha = color.alpha * fade->m_alpha[lbl_805F0170] + 0.5f;
"""),
  "fmul": v("""    color.red = fade->m_scaleRed * color.red + 0.5f;
    color.green = fade->m_scaleGreen * color.green + 0.5f;
    color.blue = fade->m_scaleBlue * color.blue + 0.5f;
    color.alpha = fade->m_alpha[lbl_805F0170] * color.alpha + 0.5f;
    material->color = color;
"""),
}
