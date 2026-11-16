"""Source shapes for MoveSlide of Game/VehicleGun.cpp (wave 39): retail loads the slide's dt (f3)
before the constant it multiplies (f2)."""
SYMBOLS = {"MoveSlide__FP19_stMtnCallbackParam": "static void MoveSlide(_stMtnCallbackParam *param) {"}
XTRA = {"MoveSlide__FP19_stMtnCallbackParam": """inline float Mul_(float a, float b) { return a * b; }
inline float SlideDt_(UnkGunSlide *s) { return s->m_dt; }
"""}
def body(kick, creep, pre=""):
    return f"""    UnkGunSlide *slide = (UnkGunSlide *)param->m_data;
    Mtx34f matrix;
    fn_80016370(&matrix, param->m_frame);
{pre}    if (slide->m_kick != 0) {{
        {kick}
        if (slide->m_offset.z < -2.0f) {{
            slide->m_offset.z = -2.0f;
            slide->m_kick = 0;
        }}
    }} else {{
        {creep}
        if (slide->m_offset.z > 0.0f) {{
            slide->m_offset.z = 0.0f;
        }}
    }}
    fn_8040B59C(&matrix, &slide->m_offset);
    fn_800163F4(param->m_frame, &matrix);
"""
BODIES = {"MoveSlide__FP19_stMtnCallbackParam": {
    "dt_first": body("slide->m_offset.z += slide->m_dt * -50.0f;", "slide->m_offset.z += slide->m_dt * 15.0f;"),
    "dt_local_in": body("float dt = slide->m_dt;\n        slide->m_offset.z += dt * -50.0f;", "float dt = slide->m_dt;\n        slide->m_offset.z += dt * 15.0f;"),
    "dt_local_in_rev": body("float dt = slide->m_dt;\n        slide->m_offset.z += -50.0f * dt;", "float dt = slide->m_dt;\n        slide->m_offset.z += 15.0f * dt;"),
    "step_local": body("float step = -50.0f * slide->m_dt;\n        slide->m_offset.z += step;", "float step = 15.0f * slide->m_dt;\n        slide->m_offset.z += step;"),
    "step_local_rev": body("float step = slide->m_dt * -50.0f;\n        slide->m_offset.z += step;", "float step = slide->m_dt * 15.0f;\n        slide->m_offset.z += step;"),
    "speed_local": body("float speed = -50.0f;\n        slide->m_offset.z += slide->m_dt * speed;", "float speed = 15.0f;\n        slide->m_offset.z += slide->m_dt * speed;"),
    "assign_sum": body("slide->m_offset.z = slide->m_offset.z + slide->m_dt * -50.0f;", "slide->m_offset.z = slide->m_offset.z + slide->m_dt * 15.0f;"),
    "assign_sum2": body("slide->m_offset.z = slide->m_dt * -50.0f + slide->m_offset.z;", "slide->m_offset.z = slide->m_dt * 15.0f + slide->m_offset.z;"),
    "mul_fn": body("slide->m_offset.z += Mul_(slide->m_dt, -50.0f);", "slide->m_offset.z += Mul_(slide->m_dt, 15.0f);"),
    "dt_fn": body("slide->m_offset.z += SlideDt_(slide) * -50.0f;", "slide->m_offset.z += SlideDt_(slide) * 15.0f;"),
    "dt_fn_rev": body("slide->m_offset.z += -50.0f * SlideDt_(slide);", "slide->m_offset.z += 15.0f * SlideDt_(slide);"),
    "minus": body("slide->m_offset.z -= slide->m_dt * 50.0f;", "slide->m_offset.z += slide->m_dt * 15.0f;"),
}}
