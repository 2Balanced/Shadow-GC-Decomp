"""Wave 41: IsNotOfKind: the comparison after the block, with the found path first."""
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
B1 = """static bool IsNotOfKind(TargetEntry target, int kind) {
    int ownerKind;
    {
        boost::shared_ptr<Target> locked(target->lock());
        boost::shared_ptr<CharInfo> owner(locked->m_owner.lock());
        if (owner.get() != 0) {
            ownerKind = owner->field_0x5C;
        } else {
            return true;
        }
    }
    return ownerKind != kind;
}
"""
B2 = """static bool IsNotOfKind(TargetEntry target, int kind) {
    int ownerKind;
    bool found;
    {
        boost::shared_ptr<Target> locked(target->lock());
        boost::shared_ptr<CharInfo> owner(locked->m_owner.lock());
        found = owner.get() != 0;
        if (found) {
            ownerKind = owner->field_0x5C;
        }
    }
    if (found) {
        return ownerKind != kind;
    }
    return true;
}
"""
B3 = """static bool IsNotOfKind(TargetEntry target, int kind) {
    int ownerKind;
    {
        boost::shared_ptr<Target> locked(target->lock());
        boost::shared_ptr<CharInfo> owner(locked->m_owner.lock());
        if (!owner) {
            return true;
        }
        ownerKind = owner->field_0x5C;
    }
    return ownerKind != kind;
}
"""
REPL = {"b1": [(O, B1)], "b2": [(O, B2)], "b3": [(O, B3)]}
