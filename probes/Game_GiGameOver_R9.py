"""Round 9 for GiGameOver's constructor (wave 39): a comma temporary on the other strings."""
SCORE = ["__ct__10GiGameOverFP4Task"]
REPL = {
  "comma_continue": [('fn_80367238(scene.Get(), "ContinueOpen");', 'fn_80367238(scene.Get(), (0, "ContinueOpen"));')],
  "comma_open": [('fn_80367238(scene.Get(), "GameoverOpen");', 'fn_80367238(scene.Get(), (0, "GameoverOpen"));')],
  "comma_both": [('fn_80367238(scene.Get(), "ContinueOpen");', 'fn_80367238(scene.Get(), (0, "ContinueOpen"));'), ('fn_80367238(scene.Get(), "GameoverOpen");', 'fn_80367238(scene.Get(), (0, "GameoverOpen"));')],
  "comma_gameover": [('RCPtr<CScene> scene(fn_8036D660(m_project, "GameOver"));', 'RCPtr<CScene> scene(fn_8036D660(m_project, (0, "GameOver")));')],
  "comma_file1": [('fn_800422A8(m_project, fn_800C02CC(s_fileName));', 'fn_800422A8(m_project, fn_800C02CC((0, s_fileName)));')],
  "ternary_motion": [('''            if (lbl_8057D6A0.m_twoPlayer) {
                fn_80367238(scene.Get(), "ContinueOpen");
                m_state = 1;
            } else {
                fn_80367238(scene.Get(), "GameoverOpen");
                m_state = 0;
            }''', '''            if (lbl_8057D6A0.m_twoPlayer) {
                CScene *s = scene.Get();
                fn_80367238(s, "ContinueOpen");
                m_state = 1;
            } else {
                CScene *s = scene.Get();
                fn_80367238(s, "GameoverOpen");
                m_state = 0;
            }''')],
}
