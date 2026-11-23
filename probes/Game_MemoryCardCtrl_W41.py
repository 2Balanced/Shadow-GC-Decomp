"""Wave 41: MemoryCard::PollOpen's request switch with the failure after the switch (the cases
2, 3, 6 breaking out, 4 and 5 returning)."""
F = "PollOpen__10MemoryCardFv"
SCORE = [F]
OLD = """        switch (m_request) {
        case 2:
        case 3:
        case 6:
        default:
            m_result = 2;
            Fail();
            break;
        case 4:
        case 5:
            SetCommand(CMD_GET_STATUS);
            break;
        }
"""
REPL = {
    "after_break": [(OLD, """        switch (m_request) {
        case 2:
        case 3:
        case 6:
            break;
        case 4:
        case 5:
            SetCommand(CMD_GET_STATUS);
            return;
        }
        m_result = 2;
        Fail();
""")],
    "after_break_def": [(OLD, """        switch (m_request) {
        case 2:
        case 3:
        case 6:
        default:
            break;
        case 4:
        case 5:
            SetCommand(CMD_GET_STATUS);
            return;
        }
        m_result = 2;
        Fail();
""")],
    "after_sep": [(OLD, """        switch (m_request) {
        case 2:
            break;
        case 3:
            break;
        case 6:
            break;
        case 4:
        case 5:
            SetCommand(CMD_GET_STATUS);
            return;
        }
        m_result = 2;
        Fail();
""")],
    "after_4first": [(OLD, """        switch (m_request) {
        case 4:
        case 5:
            SetCommand(CMD_GET_STATUS);
            return;
        case 2:
        case 3:
        case 6:
            break;
        }
        m_result = 2;
        Fail();
""")],
}
