"""PlayerCharaColliImpl (wave 40): the callees' true parameters. fn_80093230 (Game/PlayerUtil.cpp)
scales the vector by a float, which retail passes as the dot product still in f1."""
SCORE = ["fn_800986F0", "fn_80098224", "Update__Q26Player20PlayerCharaColliImplFv", "AcceptHit__FP32UnkPlayerColliCandidate_80099088"]
D = "Vec3f fn_80093230(const Vec3f *vector);\n"
U = """                if (Dot(hitNormal, remain) <= 0.0f) {
                    SubVector(remain, fn_80093230(&hitNormal));
                }
"""
REPL = {
  "scale_dot": [(D, "Vec3f fn_80093230(const Vec3f *vector, float scale);\n"),
                (U, """                float dot = Dot(hitNormal, remain);
                if (dot <= 0.0f) {
                    SubVector(remain, fn_80093230(&hitNormal, dot));
                }
""")],
  "scale_dot2": [(D, "Vec3f fn_80093230(const Vec3f *vector, float scale);\n"),
                (U, """                float dot;
                if ((dot = Dot(hitNormal, remain)) <= 0.0f) {
                    SubVector(remain, fn_80093230(&hitNormal, dot));
                }
""")],
}
