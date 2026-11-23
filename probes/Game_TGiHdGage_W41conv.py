"""Scratch: which part of TGiHdGage::Tick lets GC/2.7 share the conversion slot."""
T = "Tick__9TGiHdGageFf"
SCORE = [T]
IFS = """            if (value->m_value > m_gauges[i].m_value) {
                m_gauges[i].m_value = (m_gauges[i].m_value + value->m_value) / 2;
            } else {
                m_gauges[i].m_value = value->m_value;
            }
            if (m_gauges[i].m_value > value->m_max) {
                m_gauges[i].m_value = value->m_max;
            }
"""
REPL = {
    "base": [],
    "noifs": [(IFS, "")],
    "noloop": [("    for (i = 0; i < 2; i++) {\n        RCPtr", "    i = 0; {\n        RCPtr")],
    "nocond": [("        if (current != lbl_805EE118.Get()) {", "        {")],
    "argnamed": [("            fn_80366740(scene.Get(), frames * m_gauges[i].m_value / value->m_max);",
                  "            float f = frames * m_gauges[i].m_value / value->m_max;\n            fn_80366740(scene.Get(), f);")],
    "fnamed": [("            int frames = fn_80366728(scene.Get());", "            float ff = fn_80366728(scene.Get());\n            int frames = ff;")],
}
