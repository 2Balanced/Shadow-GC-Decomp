"""Wave 41: UnkFader_80044254's constructor with copy propagation off (the wave-40 lever): retail
keeps the render state's previous value straight in r26 (`mr r26,r3`)."""
SCORE = ["__ct__17UnkFader_80044254Fv"]
F = "UnkFader_80044254::UnkFader_80044254() {"
END = "UnkFader_80044254::~UnkFader_80044254() {"
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
REPL = {
  "prag": P,
}
