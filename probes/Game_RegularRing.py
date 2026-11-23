"""Wave 40: Ring::OnCommand, the address of the command's target computed inside the ring loops
(GC/2.7 hoists it into a sixth saved register), and LayoutRings' liner step (f30, GC/2.7 f31)."""
SCORE = ["OnCommand__Q211RegularRing4RingFR11CharCommand"]
F = "void Ring::OnCommand(CharCommand &command) {"
V_OLD = "                if (m_elements[i].m_target ==\n                    static_cast<VacuumCommand &>(command).m_target.lock()) {\n"
U_OLD = "                if (m_elements[i].m_target == static_cast<UseCommand &>(command).m_target.lock()) {\n"
H = """inline const boost::weak_ptr<Target> &TargetOf_(VacuumCommand &c) { return c.m_target; }
inline const boost::weak_ptr<Target> &TargetOf_(UseCommand &c) { return c.m_target; }
inline boost::shared_ptr<Target> LockTarget_(VacuumCommand &c) { return c.m_target.lock(); }
inline boost::shared_ptr<Target> LockTarget_(UseCommand &c) { return c.m_target.lock(); }
inline bool IsTarget_(const boost::shared_ptr<Target> &t, const boost::weak_ptr<Target> &w) { return t == w.lock(); }
inline bool IsTargetOf_(const boost::shared_ptr<Target> &t, VacuumCommand &c) { return t == c.m_target.lock(); }
inline bool IsTargetOf_(const boost::shared_ptr<Target> &t, UseCommand &c) { return t == c.m_target.lock(); }
inline VacuumCommand &AsVacuum_(CharCommand &c) { return static_cast<VacuumCommand &>(c); }
inline UseCommand &AsUse_(CharCommand &c) { return static_cast<UseCommand &>(c); }
""" + F


def v(a, b):
    return [(F, H), (V_OLD, "                if (" + a + ") {\n"), (U_OLD, "                if (" + b + ") {\n")]


E = "m_elements[i].m_target"
REPL = {
    "getter": v(E + " == TargetOf_(static_cast<VacuumCommand &>(command)).lock()", E + " == TargetOf_(static_cast<UseCommand &>(command)).lock()"),
    "lock_inl": v(E + " == LockTarget_(static_cast<VacuumCommand &>(command))", E + " == LockTarget_(static_cast<UseCommand &>(command))"),
    "is_target": v("IsTarget_(" + E + ", static_cast<VacuumCommand &>(command).m_target)", "IsTarget_(" + E + ", static_cast<UseCommand &>(command).m_target)"),
    "is_target_of": v("IsTargetOf_(" + E + ", static_cast<VacuumCommand &>(command))", "IsTargetOf_(" + E + ", static_cast<UseCommand &>(command))"),
    "as_inl": v(E + " == AsVacuum_(command).m_target.lock()", E + " == AsUse_(command).m_target.lock()"),
    "ptr_cast": v(E + " == static_cast<VacuumCommand *>(&command)->m_target.lock()", E + " == static_cast<UseCommand *>(&command)->m_target.lock()"),
    "c_cast": v(E + " == ((VacuumCommand *)&command)->m_target.lock()", E + " == ((UseCommand *)&command)->m_target.lock()"),
    "swap": v("static_cast<VacuumCommand &>(command).m_target.lock() == " + E, "static_cast<UseCommand &>(command).m_target.lock() == " + E),
    "prag_licm": [(F, "#pragma opt_loop_invariants off\n" + F)],
    "prag_sr": [(F, "#pragma opt_strength_reduction off\n" + F)],
    "prag_cse": [(F, "#pragma opt_common_subs off\n" + F)],
    "prag_prop": [(F, "#pragma opt_propagation off\n" + F)],
    "prag_dead": [(F, "#pragma opt_dead_code off\n" + F)],
    "prag_life": [(F, "#pragma opt_lifetimes off\n" + F)],
    "prag_unroll": [(F, "#pragma opt_unroll_loops off\n" + F)],
    "prag_vec": [(F, "#pragma opt_vectorize_loops off\n" + F)],
    "prag_deadassign": [(F, "#pragma opt_dead_assignments off\n" + F)],
}
