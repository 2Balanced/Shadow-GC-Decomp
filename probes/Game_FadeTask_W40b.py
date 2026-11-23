"""FadeTask (wave 40, round 2): where GC/2.7 creates FadeTask's RTTI name ("SonicteamUSA::...
::FadeTask", @169, and "Task" in .sdata2): ahead of the string literals, where retail has both
after them. Scored on the fader constructor; the data order is checked by hand."""
SCORE = ["__ct__17UnkFader_80044254Fv"]
CLS = """class FadeTask : public Task {
  public:
    FadeTask(Task *parent);
    virtual ~FadeTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(FadeTask, 0x28);

class FadeDrawTask : public Task {
  public:
    FadeDrawTask(Task *parent);
    virtual ~FadeDrawTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(FadeDrawTask, 0x28);
"""
SW = """class FadeDrawTask : public Task {
  public:
    FadeDrawTask(Task *parent);
    virtual ~FadeDrawTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(FadeDrawTask, 0x28);

class FadeTask : public Task {
  public:
    FadeTask(Task *parent);
    virtual ~FadeTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(FadeTask, 0x28);
"""
NOSZ = CLS.replace("SIZE_ASSERT(FadeTask, 0x28);\n", "").replace("SIZE_ASSERT(FadeDrawTask, 0x28);\n", "")
REPL = {
  "base": [],
  "swap_cls": [(CLS, SW)],
  "no_size": [(CLS, NOSZ)],
}
DT = "FadeTask::~FadeTask() {}\n"
TK = "void FadeTask::Tick(float dt) {\n    fn_80044AD8(fn_80044254(), dt);\n}\n"
CT = "FadeTask::FadeTask(Task *parent) : Task(parent) {\n    m_name = \"FadeTask\";\n    m_flags |= 0x100;\n}\n"
REPL["no_dtor"] = [(DT, "")]
REPL["no_tick"] = [(TK, "")]
REPL["no_ctor"] = [(CT, "")]
REPL["new_draw_first"] = [("    new FadeTask(updateParent);\n    new FadeDrawTask(drawParent);\n", "    new FadeDrawTask(drawParent);\n    new FadeTask(updateParent);\n")]
