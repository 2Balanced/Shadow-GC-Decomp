"""HeavyDogCtrlUnit_RocketLauncher's constructor (wave 40): loops of one pass around the collider
call's arguments."""
O = """            m_collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_owner->m_charaPtr),
                                     &shapes[0], m_owner->m_motion, 4, 0);
"""
W = "for (int n = 0; n < 1; n++) {\n"
C = "            m_collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_owner->m_charaPtr),\n                                     %s, %s, 4, 0);\n"
REPL = {
 "for_all": [(O, "            " + W + "    " + O.replace("\n      ", "\n          ") + "            }\n")],
 "for_motion": [(O, "            _stMtnPlayWork *work;\n            " + W + "                work = m_owner->m_motion;\n            }\n" + C % ("&shapes[0]", "work"))],
 "for_shapes": [(O, "            UnkBossColliShape *list;\n            " + W + "                list = &shapes[0];\n            }\n" + C % ("list", "m_owner->m_motion"))],
 "for_both": [(O, "            UnkBossColliShape *list;\n            _stMtnPlayWork *work;\n            " + W + "                list = &shapes[0];\n                work = m_owner->m_motion;\n            }\n" + C % ("list", "work"))],
 "for_set": [("            shapes[0].m_sphere = &sphere;\n            shapes[0].field_0xC = m_motion;\n", "            " + W + "                shapes[0].m_sphere = &sphere;\n                shapes[0].field_0xC = m_motion;\n            }\n")],
 "for_owner": [(O, "            HeavyDogVarParam *owner;\n            " + W + "                owner = m_owner;\n            }\n            m_collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(owner->m_charaPtr),\n                                     &shapes[0], owner->m_motion, 4, 0);\n")],
 "for_owner2": [(O, "            HeavyDogVarParam *owner;\n            " + W + "                owner = m_owner;\n            }\n            m_collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_owner->m_charaPtr),\n                                     &shapes[0], owner->m_motion, 4, 0);\n")],
}
N = "m_collider = fn_800C7F7C(manager, owner, &shapes[0], m_owner->m_motion, 4, 0);\n"
REPL.update({
 "named": [(O, "            boost::weak_ptr<CharInfo> owner(m_owner->m_charaPtr);\n            " + N)],
 "named_blk": [(O, "            {\n                boost::weak_ptr<CharInfo> owner(m_owner->m_charaPtr);\n                " + N + "            }\n")],
 "named_eq": [(O, "            {\n                boost::weak_ptr<CharInfo> owner = m_owner->m_charaPtr;\n                " + N + "            }\n")],
 "named_for": [(O, "            " + W + "                boost::weak_ptr<CharInfo> owner(m_owner->m_charaPtr);\n                " + N + "            }\n")],
 "cref": [(O, "            {\n                const boost::weak_ptr<CharInfo> &owner = boost::weak_ptr<CharInfo>(m_owner->m_charaPtr);\n                " + N + "            }\n")],
})
SCORE = ["__ct__31HeavyDogCtrlUnit_RocketLauncherFP16HeavyDogVarParami"]
