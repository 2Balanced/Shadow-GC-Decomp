"""Wave 41: GiGameOver's constructor with copy propagation off and a named copy of this (the
wave-40 lever): retail keeps this in r31 and the .rodata base in r30."""
SCORE = ["__ct__10GiGameOverFP4Task"]
F = "GiGameOver::GiGameOver(Task *parent) : Task(parent) {"
END = "GiGameOver::~GiGameOver() {"
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
BODY_OLD = """    m_timer = 0.0f;
    m_yes = 0;
    m_decided = 0;
    m_waiting = 1;
    m_name = "GiGameOver";
    s_instance = this;
    m_project = new Chao::CSD::CProject();
    if (m_project != 0) {
        fn_8003F8D4();
        fn_800422A8(m_project, fn_800C02CC(s_fileName));
        fn_8036E114(m_project, fn_800423B8(fn_8003F8D4(), s_fileName, 0));
        fn_8036D598(m_project, 0);
        RCPtr<CScene> scene(fn_8036D660(m_project, "GameOver"));
        if (scene.Get() != 0) {
            if (lbl_8057D6A0.m_twoPlayer) {
                fn_80367238(scene.Get(), "ContinueOpen");
                m_state = 1;
            } else {
                fn_80367238(scene.Get(), "GameoverOpen");
                m_state = 0;
            }
            fn_803659DC(scene.Get(), 0);
        }
        scene = fn_8036D660(m_project, "Loop");
        if (scene.Get() != 0) {
            fn_80367238(scene.Get(), "Loop");
            fn_803659DC(scene.Get(), 1);
        }
    }
    m_render = fn_8004E24C(fn_80046178()->field_0x3C);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&GiGameOver::RednerDraw, this, _1));
"""
import re
def selfify(body, name="self"):
    b = re.sub(r"(?<![.>])\bm_", name + "->m_", body)
    b = b.replace("s_instance = this;", "s_instance = " + name + ";")
    b = b.replace("&GiGameOver::RednerDraw, this, _1", "&GiGameOver::RednerDraw, " + name + ", _1")
    return b
SELF = "    GiGameOver *self = this;\n"
CSELF = "    GiGameOver *const self = this;\n"
REPL = {
  "prag": P,
  "prag_self": P + [(BODY_OLD, SELF + selfify(BODY_OLD))],
  "prag_cself": P + [(BODY_OLD, CSELF + selfify(BODY_OLD))],
  "cself": [(BODY_OLD, CSELF + selfify(BODY_OLD))],
  "self": [(BODY_OLD, SELF + selfify(BODY_OLD))],
  "prag_self_bindthis": P + [(BODY_OLD, SELF + selfify(BODY_OLD).replace("RednerDraw, self, _1", "RednerDraw, this, _1"))],
  "prag_self_inst": P + [(BODY_OLD, SELF + selfify(BODY_OLD).replace("s_instance = self;", "s_instance = this;"))],
}

NM = '    m_name = "GiGameOver";\n'
CO = '                fn_80367238(scene.Get(), "ContinueOpen");\n'
GO = '                fn_80367238(scene.Get(), "GameoverOpen");\n'
REPL.update({
  "name_local": [(NM, '    const char *name = "GiGameOver";\n    m_name = name;\n')],
  "p_name_local": P + [(NM, '    const char *name = "GiGameOver";\n    m_name = name;\n')],
  "p_name_top": P + [(BODY_OLD, '    const char *name = "GiGameOver";\n' + BODY_OLD.replace(NM, '    m_name = name;\n'))],
  "p_co_local": P + [(CO, '                const char *motion = "ContinueOpen";\n                fn_80367238(scene.Get(), motion);\n')],
  "p_go_local": P + [(GO, '                const char *motion = "GameoverOpen";\n                fn_80367238(scene.Get(), motion);\n')],
  "p_both_local": P + [(CO, '                const char *motion = "ContinueOpen";\n                fn_80367238(scene.Get(), motion);\n'), (GO, '                const char *motion = "GameoverOpen";\n                fn_80367238(scene.Get(), motion);\n')],
})

TOP4 = "    m_timer = 0.0f;\n    m_yes = 0;\n    m_decided = 0;\n    m_waiting = 1;\n    m_name = \"GiGameOver\";\n"
REPL.update({
  "n_top": [(TOP4, '    const char *name = "GiGameOver";\n    m_timer = 0.0f;\n    m_yes = 0;\n    m_decided = 0;\n    m_waiting = 1;\n    m_name = name;\n')],
  "n_zero": [(TOP4, '    u8 zero = 0;\n    m_timer = 0.0f;\n    m_yes = zero;\n    m_decided = zero;\n    m_waiting = 1;\n    const char *name = "GiGameOver";\n    m_name = name;\n')],
  "n_first": [(TOP4, '    const char *name = "GiGameOver";\n    m_name = name;\n    m_timer = 0.0f;\n    m_yes = 0;\n    m_decided = 0;\n    m_waiting = 1;\n')],
  "n_after_timer": [(TOP4, '    m_timer = 0.0f;\n    const char *name = "GiGameOver";\n    m_yes = 0;\n    m_decided = 0;\n    m_waiting = 1;\n    m_name = name;\n')],
  "n_chain": [(TOP4, '    m_timer = 0.0f;\n    m_yes = m_decided = 0;\n    m_waiting = 1;\n    const char *name = "GiGameOver";\n    m_name = name;\n')],
  "n_const": [(TOP4, '    m_timer = 0.0f;\n    m_yes = 0;\n    m_decided = 0;\n    m_waiting = 1;\n    const char *const name = "GiGameOver";\n    m_name = name;\n')],
  "n_arr": [(TOP4, '    m_timer = 0.0f;\n    m_yes = 0;\n    m_decided = 0;\n    m_waiting = 1;\n    static const char name[] = "GiGameOver";\n    m_name = name;\n')],
})
