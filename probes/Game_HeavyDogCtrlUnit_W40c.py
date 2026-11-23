"""HeavyDogCtrlUnit (wave 40, round 3): where kMaxLaunchers is defined, for its place in .sdata2
(retail: after every float literal of the unit). Scored on the ctor only; check .sdata2 by hand."""
SCORE = ["__ct__31HeavyDogCtrlUnit_RocketLauncherFP16HeavyDogVarParami"]
DEF = """namespace HeavyDog {
const s32 kMaxLaunchers = 7;
} // namespace HeavyDog
"""
EXT = """/* How many rocket launchers the boss can carry. */
namespace HeavyDog {
extern const s32 kMaxLaunchers;
} // namespace HeavyDog
"""
TOP = "class UnkCharaList_80074194;\n"
REPL = {
  "def_top": [(DEF, ""), (TOP, TOP + "\n" + DEF)],
  "def_at_extern": [(DEF, ""), (EXT, "/* How many rocket launchers the boss can carry. */\n" + DEF)],
}
MID = "HeavyDogCtrlUnit_Barrier::HeavyDogCtrlUnit_Barrier(HeavyDogVarParam *param, const Vec3f *position,\n"
DTOR = "HeavyDogCtrlUnit_RocketLauncher::~HeavyDogCtrlUnit_RocketLauncher() {\n"
REPL["def_mid"] = [(DEF, ""), (MID, DEF + MID)]
REPL["def_after_ctor"] = [(DEF, ""), (DTOR, DEF + DTOR)]
REPL["def_volatile_top"] = [(DEF, ""), (TOP, TOP + "\nnamespace HeavyDog {\nextern const volatile s32 kMaxLaunchers = 7;\n} // namespace HeavyDog\n"), (EXT, "")]
TPL = """/* How many rocket launchers the boss can carry. */
template <class T> struct LauncherLimit_ {
    static const s32 value;
};
template <class T> const s32 LauncherLimit_<T>::value = 7;
"""
REPL["tpl_static"] = [(DEF, ""), (EXT, TPL), ("m_owner->m_launcherCount >= HeavyDog::kMaxLaunchers", "m_owner->m_launcherCount >= LauncherLimit_<int>::value"), ("m_owner->m_launcherCount != HeavyDog::kMaxLaunchers", "m_owner->m_launcherCount != LauncherLimit_<int>::value")]
