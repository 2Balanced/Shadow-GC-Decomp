"""Round 6 for GiGameOver's constructor (wave 39): another string through a local."""
SCORE = ["__ct__10GiGameOverFP4Task"]
REPL = {
  "gameover_local": [('        RCPtr<CScene> scene(fn_8036D660(m_project, "GameOver"));', '        const char *sceneName = "GameOver";\n        RCPtr<CScene> scene(fn_8036D660(m_project, sceneName));')],
  "continue_local": [('                fn_80367238(scene.Get(), "ContinueOpen");', '                const char *motion = "ContinueOpen";\n                fn_80367238(scene.Get(), motion);')],
  "open_local": [('                fn_80367238(scene.Get(), "GameoverOpen");', '                const char *motion = "GameoverOpen";\n                fn_80367238(scene.Get(), motion);')],
  "motion_var": [('''            if (lbl_8057D6A0.m_twoPlayer) {
                fn_80367238(scene.Get(), "ContinueOpen");
                m_state = 1;
            } else {
                fn_80367238(scene.Get(), "GameoverOpen");
                m_state = 0;
            }''', '''            const char *motion;
            if (lbl_8057D6A0.m_twoPlayer) {
                motion = "ContinueOpen";
                fn_80367238(scene.Get(), motion);
                m_state = 1;
            } else {
                motion = "GameoverOpen";
                fn_80367238(scene.Get(), motion);
                m_state = 0;
            }''')],
  "loop_local": [('        scene = fn_8036D660(m_project, "Loop");\n        if (scene.Get() != 0) {\n            fn_80367238(scene.Get(), "Loop");', '        const char *loop = "Loop";\n        scene = fn_8036D660(m_project, loop);\n        if (scene.Get() != 0) {\n            fn_80367238(scene.Get(), "Loop");')],
  "loop_local2": [('            fn_80367238(scene.Get(), "Loop");', '            const char *loop = "Loop";\n            fn_80367238(scene.Get(), loop);')],
}
