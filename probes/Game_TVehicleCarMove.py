"""Wave 40: the small members whose first load retail has above the prologue's stores, as const
members; SetCamera's handling-table read without the inline accessor."""
P = "Q27Vehicle15TVehicleCarMove"
SCORE = [("GetNode__%sFi" % P, "GetNode__%sCFi" % P), ("SetLamp__%sFb" % P, "SetLamp__%sCFb" % P),
         ("GetDamage__%sFv" % P, "GetDamage__%sCFv" % P), ("UpdateSpring__%sFv" % P, "UpdateSpring__%sFv" % P),
         "SetCamera__%sFii" % P]
C = [("    int GetDamage();", "    int GetDamage() const;"), ("    void *GetNode(int index);", "    void *GetNode(int index) const;"),
     ("    void SetLamp(bool on);", "    void SetLamp(bool on) const;"),
     ("    const CarModelDesc &ModelDesc() {", "    const CarModelDesc &ModelDesc() const {"),
     ("    const CarParam &Param() {", "    const CarParam &Param() const {"),
     ("int TVehicleCarMove::GetDamage() {", "int TVehicleCarMove::GetDamage() const {"),
     ("void *TVehicleCarMove::GetNode(int index) {", "void *TVehicleCarMove::GetNode(int index) const {"),
     ("void TVehicleCarMove::SetLamp(bool on) {", "void TVehicleCarMove::SetLamp(bool on) const {")]
CAM = ("Param().m_cameraOffset));", "s_params[m_type].m_cameraOffset));")
REPL = {
  "const": C,
  "const_cam": C + [CAM],
}
