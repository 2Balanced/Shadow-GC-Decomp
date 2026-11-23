"""Wave 40: Behave keeps this in r30 and the contacts in r29 in retail (GC/2.7: the other way
round with the contacts as a declared reference)."""
SCORE = ["Behave__10LightColliFf"]
F = "void LightColli::Behave(float dt) {"
C = """    UnkColliContacts_8001EE04 &contacts = *reinterpret_cast<UnkColliContacts_8001EE04 *>(
        reinterpret_cast<u8 *>(m_collision->m_collider.get()) + 0xC);
"""
HELP = """inline UnkColliContacts_8001EE04 *ContactsOf_(CharaColliBase *c) { return reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(c) + 0xC); }
inline UnkColliContacts_8001EE04 &ContactsRef_(CharaColliBase *c) { return *reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(c) + 0xC); }
inline UnkColliContacts_8001EE04 *ContactsOfG_(GadgetCharCollision *g) { return reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(g->m_collider.get()) + 0xC); }
"""
H = (F, HELP + F)
REPL = {
  "inl_ref": [H, (C, "    UnkColliContacts_8001EE04 &contacts = *ContactsOf_(m_collision->m_collider.get());\n")],
  "inl_ref2": [H, (C, "    UnkColliContacts_8001EE04 &contacts = ContactsRef_(m_collision->m_collider.get());\n")],
  "inl_ptr": [H, (C, "    UnkColliContacts_8001EE04 *contactsP = ContactsOf_(m_collision->m_collider.get());\n#define contacts (*contactsP)\n")],
  "inl_g": [H, (C, "    UnkColliContacts_8001EE04 &contacts = *ContactsOfG_(m_collision.get());\n")],
  "ptr": [(C, "    UnkColliContacts_8001EE04 *contactsP = reinterpret_cast<UnkColliContacts_8001EE04 *>(\n        reinterpret_cast<u8 *>(m_collision->m_collider.get()) + 0xC);\n#define contacts (*contactsP)\n")],
  "const_ref": [(C, C.replace("UnkColliContacts_8001EE04 &contacts", "UnkColliContacts_8001EE04 &contacts").replace("    UnkColliContacts", "    CharaColliBase *base = m_collision->m_collider.get();\n    UnkColliContacts").replace("m_collision->m_collider.get()) + 0xC", "base) + 0xC"))],
  "self": [(C, "    LightColli *self = this;\n" + C.replace("m_collision->", "self->m_collision->"))],
}
