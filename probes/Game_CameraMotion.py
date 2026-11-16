"""Source shapes for fn_8000F9E8 of Game/CameraMotion.cpp (wave 39): retail keeps count in r27,
state in r28, i in r29 and the tracks in r30.  Declaration and assignment order permutations."""
import itertools
SCORE = ["fn_8000F9E8"]
OLD = """    UnkCameraTrack_8000F9E8 *track = motion->m_tracks;
    int count = motion->m_trackCount;
    state->m_set = 0;
    for (int i = 0; i < count; i++, track++) {
"""
D = {"t": "    UnkCameraTrack_8000F9E8 *track;\n", "c": "    int count;\n", "i": "    int i;\n"}
A = {"t": "    track = motion->m_tracks;\n", "c": "    count = motion->m_trackCount;\n", "s": "    state->m_set = 0;\n"}
REPL = {}
for d in itertools.permutations("tci"):
    for a in itertools.permutations("tcs"):
        name = "d_" + "".join(d) + "_a_" + "".join(a)
        REPL[name] = [(OLD, "".join(D[x] for x in d) + "".join(A[x] for x in a) + "    for (i = 0; i < count; i++, track++) {\n")]
