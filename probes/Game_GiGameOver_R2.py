"""Round 2 for GiGameOver's constructor (wave 39): how the task's name is set.  Without the name
(one use of the .rodata base less) GC/2.7 gives `this` r31 as retail does."""
SCORE = ["__ct__10GiGameOverFP4Task"]
NAME = '    m_name = "GiGameOver";\n'
C = "GiGameOver::GiGameOver(Task *parent) : Task(parent) {"
HELP = """inline void SetTaskName_(Task *task, const char *name) { task->m_name = name; }
inline const char *ClassName_() { return "GiGameOver"; }
"""
REPL = {
  "setter": [(C, HELP + C), (NAME, '    SetTaskName_(this, "GiGameOver");\n')],
  "getter": [(C, HELP + C), (NAME, '    m_name = ClassName_();\n')],
  "setter_getter": [(C, HELP + C), (NAME, '    SetTaskName_(this, ClassName_());\n')],
  "local": [(NAME, '    const char *name = "GiGameOver";\n    m_name = name;\n')],
  "local_top": [(NAME, ''), ("    m_timer = 0.0f;\n", '    const char *name = "GiGameOver";\n    m_timer = 0.0f;\n'), ("    s_instance = this;\n", "    m_name = name;\n    s_instance = this;\n")],
  "this_arrow": [(NAME, '    this->m_name = "GiGameOver";\n')],
  "task_cast": [(NAME, '    static_cast<Task *>(this)->m_name = "GiGameOver";\n')],
  "task_qual": [(NAME, '    Task::m_name = "GiGameOver";\n')],
}
