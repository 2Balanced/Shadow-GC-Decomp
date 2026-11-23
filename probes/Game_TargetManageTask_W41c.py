"""Wave 41: IsNotOfKind (retail: the owner's kind kept in r31 across both destructors, compared
afterwards): the comparison after a block that holds the shared pointers."""
SCORE = ["IsNotOfKind__FPCQ25boost17weak_ptr<6Target>i"]
O = """static bool IsNotOfKind(TargetEntry target, int kind) {
    boost::shared_ptr<Target> locked(target->lock());
    boost::shared_ptr<CharInfo> owner(locked->m_owner.lock());
    if (owner.get() != 0) {
        int ownerKind = owner->field_0x5C;
        return ownerKind != kind;
    }
    return true;
}
"""
P0 = "#pragma opt_propagation off\n"
P1 = "#pragma opt_propagation reset\n"
BLK = """static bool IsNotOfKind(TargetEntry target, int kind) {
    int ownerKind;
    {
        boost::shared_ptr<Target> locked(target->lock());
        boost::shared_ptr<CharInfo> owner(locked->m_owner.lock());
        if (owner.get() == 0) {
            return true;
        }
        ownerKind = owner->field_0x5C;
    }
    return ownerKind != kind;
}
"""
INL = """inline int OwnerKind_(TargetEntry target, bool *found) {
    boost::shared_ptr<Target> locked(target->lock());
    boost::shared_ptr<CharInfo> owner(locked->m_owner.lock());
    if (owner.get() == 0) {
        *found = false;
        return 0;
    }
    *found = true;
    return owner->field_0x5C;
}
"""
REPL = {
  "block": [(O, BLK)],
  "block_prag": [(O, P0 + BLK + P1)],
  "block_ne_first": [(O, BLK.replace("    return ownerKind != kind;", "    return kind != ownerKind;"))],
}
