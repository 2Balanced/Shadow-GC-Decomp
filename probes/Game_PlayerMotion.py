"""Source shapes for fn_800A9E1C of Game/PlayerMotion.cpp (wave 39): retail reads the pointer out
of the shared_ptr above the store of LR."""
SCORE = ["fn_800A9E1C"]
OLD = "bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player) {\n    return fn_80014E34(&Fields(player->get())->m_motionFlags, 3);\n}"
REPL = {
  "const_ptr": [(OLD, "bool fn_800A9E1C(const boost::shared_ptr<Player::PlayerBase> *player) {\n    return fn_80014E34(&Fields(player->get())->m_motionFlags, 3);\n}")],
  "const_ref": [(OLD, "bool fn_800A9E1C(const boost::shared_ptr<Player::PlayerBase> &player) {\n    return fn_80014E34(&Fields(player.get())->m_motionFlags, 3);\n}")],
  "ref": [(OLD, "bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> &player) {\n    return fn_80014E34(&Fields(player.get())->m_motionFlags, 3);\n}")],
}
