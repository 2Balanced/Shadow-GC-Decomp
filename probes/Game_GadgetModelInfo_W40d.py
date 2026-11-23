"""GadgetModelInfo::AddCollider(int, weak_ptr<CharInfo>) (wave 40, round 4): fn_800C81F4's true
parameters (Game/CharaColliManager.cpp): the character info by value and a u16 last."""
SCORE = ["AddCollider__15GadgetModelInfoFiQ25boost19weak_ptr<8CharInfo>"]
D = """boost::shared_ptr<CharaColliBase> fn_800C81F4(UnkColliderManager_80021B08 *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo, void *file,
                                              int group, int arg4);
"""
REPL = {
  "u16": [(D, D.replace("int group, int arg4", "int group, u16 arg4"))],
  "byval": [(D, D.replace("const boost::weak_ptr<CharInfo> &charInfo", "boost::weak_ptr<CharInfo> charInfo"))],
  "byval_u16": [(D, D.replace("const boost::weak_ptr<CharInfo> &charInfo", "boost::weak_ptr<CharInfo> charInfo").replace("int group, int arg4", "int group, u16 arg4"))],
}
