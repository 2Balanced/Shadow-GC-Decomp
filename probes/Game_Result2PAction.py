"""Result2PAction's constructor (wave 39, second pass): the round's winner is counted with the
constant 1 in r0 and the win count incremented in place."""
SCORE = ["__ct__Q211StageAction14Result2PActionFv"]
def blk(ind, p):
    return ("%slbl_8057D6A0.m_lastWinner = %d;\n%slbl_8057D6A0.m_roundsPlayed++;\n%slbl_8057D6A0.m_wins[%d]++;\n" % (ind, p, ind, ind, p))
I3 = " " * 16
I2 = " " * 12
def rep(fn):
    return [(blk(I3, 1), I3 + fn(1)), (blk(I3, 0), I3 + fn(0)), (blk(I2, 1), I2 + fn(1)), (blk(I2, 0), I2 + fn(0))]
ANCH = "namespace StageAction {\n"
REPL = {
 "free": rep(lambda p: "CountWin(%d);\n" % p) + [(ANCH, """inline void CountWin(int player) {
    lbl_8057D6A0.m_lastWinner = player;
    lbl_8057D6A0.m_roundsPlayed++;
    lbl_8057D6A0.m_wins[player]++;
}
""" + ANCH)],
 "free_ref": rep(lambda p: "CountWin(lbl_8057D6A0, %d);\n" % p) + [(ANCH, """inline void CountWin(UnkGameState_8057D6A0 &state, int player) {
    state.m_lastWinner = player;
    state.m_roundsPlayed++;
    state.m_wins[player]++;
}
""" + ANCH)],
 "free_ptr": rep(lambda p: "CountWin(&lbl_8057D6A0, %d);\n" % p) + [(ANCH, """inline void CountWin(UnkGameState_8057D6A0 *state, int player) {
    state->m_lastWinner = player;
    state->m_roundsPlayed++;
    state->m_wins[player]++;
}
""" + ANCH)],
 "local_ptr": rep(lambda p: "{\n    UnkGameState_8057D6A0 *state = &lbl_8057D6A0;\n    state->m_lastWinner = %d;\n    state->m_roundsPlayed++;\n    state->m_wins[%d]++;\n}\n" % (p, p)),
 "plus_eq": rep(lambda p: "lbl_8057D6A0.m_lastWinner = %d;\nlbl_8057D6A0.m_roundsPlayed += 1;\nlbl_8057D6A0.m_wins[%d] += 1;\n" % (p, p)),
 "two_inl": rep(lambda p: "SetWinner(%d);\nAddWin(%d);\n" % (p, p)) + [(ANCH, """inline void SetWinner(int player) {
    lbl_8057D6A0.m_lastWinner = player;
}
inline void AddWin(int player) {
    lbl_8057D6A0.m_roundsPlayed++;
    lbl_8057D6A0.m_wins[player]++;
}
""" + ANCH)],
}
import itertools
REPL = {}
def rep2(fn):
    out = []
    for ind in (I3, I2):
        for p in (1, 0):
            out.append((blk(ind, p) + ind + "m_winner = 1;\n", fn(p)))
    return out
ST = {"L": "lbl_8057D6A0.m_lastWinner = %d;\n", "R": "lbl_8057D6A0.m_roundsPlayed++;\n", "W": "lbl_8057D6A0.m_wins[%d]++;\n", "M": "m_winner = 1;\n"}
for perm in itertools.permutations("LRWM"):
    name = "".join(perm)
    def fn(p, perm=perm):
        return "".join((ST[c] % p) if "%d" in ST[c] else ST[c] for c in perm)
    REPL[name] = rep2(fn)
