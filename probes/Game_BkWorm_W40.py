"""BkWormBase::CreateLockons (wave 40): retail gives this r31, the strength-reduced offset r30, the
lock-on's address r29, the weapon r28 and the counter r27; GC/2.7 gave this r27. Shapes that make
this a local declared first."""
SCORE = ["CreateLockons__10BkWormBaseFv"]
OLD = """void BkWormBase::CreateLockons() {
    int i;
    const WormWeapon &weapon = lbl_804C97C0[m_params.WeaponType];
    for (i = 0; i < weapon.count; i++) {
        m_lockons[i] = boost::shared_ptr<Weapon::LockonTarget>(new Weapon::LockonTarget());
        boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
        boost::shared_ptr<CharInfo> chara = player.lock();
        if (chara) {
            boost::shared_ptr<Player::PlayerBase> base =
                boost::dynamic_pointer_cast<Player::PlayerBase>(chara);
            if (base.get() != 0 && base->m_target != 0) {
                m_lockons[i]->SetTarget(fn_80206574(base->m_target));
                m_lockons[i]->fn_80060F70();
            }
        }
    }
}
"""
BODY = """    for (i = 0; i < weapon.count; i++) {
        self->m_lockons[i] = boost::shared_ptr<Weapon::LockonTarget>(new Weapon::LockonTarget());
        boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
        boost::shared_ptr<CharInfo> chara = player.lock();
        if (chara) {
            boost::shared_ptr<Player::PlayerBase> base =
                boost::dynamic_pointer_cast<Player::PlayerBase>(chara);
            if (base.get() != 0 && base->m_target != 0) {
                self->m_lockons[i]->SetTarget(fn_80206574(base->m_target));
                self->m_lockons[i]->fn_80060F70();
            }
        }
    }
}
"""
W = "    const WormWeapon &weapon = lbl_804C97C0[self->m_params.WeaponType];\n"
def v(head):
    return [(OLD, "void BkWormBase::CreateLockons() {\n" + head + BODY)]
REPL = {
  "self_copy": v("    BkWormBase *self = this;\n    int i;\n" + W),
  "self_void": v("    void *raw = this;\n    BkWormBase *self = static_cast<BkWormBase *>(raw);\n    int i;\n" + W),
  "self_void2": v("    BkWormBase *self = static_cast<BkWormBase *>(static_cast<void *>(this));\n    int i;\n" + W),
  "self_const": v("    BkWormBase *const self = this;\n    int i;\n" + W),
  "self_i_first": v("    int i;\n    BkWormBase *self = static_cast<BkWormBase *>(static_cast<void *>(this));\n" + W),
  "self_after_w": v("    int i;\n    const WormWeapon &weapon = lbl_804C97C0[m_params.WeaponType];\n    BkWormBase *self = static_cast<BkWormBase *>(static_cast<void *>(this));\n"),
  "self_ref": v("    BkWormBase &selfr = *this;\n    BkWormBase *self = &selfr;\n    int i;\n" + W),
}
# The loop in an inline helper: if GC/2.7 inlines it, its parameter becomes a local of the caller.
HELPER = "inline void CreateLockonsOf_(BkWormBase *self) {\n    int i;\n" + W + BODY + "\nvoid BkWormBase::CreateLockons() {\n    CreateLockonsOf_(this);\n}\n"
HELPER2 = "inline void CreateLockonsOf_(BkWormBase *self, const WormWeapon &weapon) {\n    int i;\n" + BODY + "\nvoid BkWormBase::CreateLockons() {\n    CreateLockonsOf_(this, lbl_804C97C0[m_params.WeaponType]);\n}\n"
HELPER3 = "inline void CreateLockonsOf_(BkWormBase *self) {\n" + W + "    int i;\n" + BODY + "\nvoid BkWormBase::CreateLockons() {\n    CreateLockonsOf_(this);\n}\n"
REPL["inline_helper"] = [(OLD, HELPER)]
REPL["inline_helper_w"] = [(OLD, HELPER2)]
REPL["inline_helper_wfirst"] = [(OLD, HELPER3)]
REPL["always_inline"] = [(OLD, "#pragma push\n#pragma always_inline on\n" + HELPER.replace("\nvoid BkWormBase::CreateLockons", "\n#pragma pop\nvoid BkWormBase::CreateLockons"))]
REPL["inline_depth"] = [(OLD, "#pragma push\n#pragma inline_depth(4)\n" + HELPER + "#pragma pop\n")]
# Wave 40, round 2: this read through an inline function (a compiler temporary, coloured ahead of
# the named locals), with the weapon declared before the counter.
SELF = "inline BkWormBase *Self_(BkWormBase *worm) {\n    return worm;\n}\n\n"
REPL["inl_self"] = [(OLD, SELF + "void BkWormBase::CreateLockons() {\n    BkWormBase *self = Self_(this);\n    int i;\n" + W + BODY)]
REPL["inl_self_wfirst"] = [(OLD, SELF + "void BkWormBase::CreateLockons() {\n    BkWormBase *self = Self_(this);\n" + W + "    int i;\n" + BODY)]
REPL["inl_self_direct"] = [(OLD, SELF + "void BkWormBase::CreateLockons() {\n    int i;\n" + W.replace("self->m_params", "Self_(this)->m_params") + BODY.replace("self->m_lockons", "Self_(this)->m_lockons"))]
REPL["inl_self_direct_wfirst"] = [(OLD, SELF + "void BkWormBase::CreateLockons() {\n" + W.replace("self->m_params", "Self_(this)->m_params") + "    int i;\n" + BODY.replace("self->m_lockons", "Self_(this)->m_lockons"))]
