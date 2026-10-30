/* SonicteamUSA::System::BitTable (class name from RTTI): a table of bits, one row of m_columns bits
 * per row, each row padded to whole words. Game/CharaColliManager.cpp keeps the matrix of collider
 * kinds that touch each other in one, and Game/DeltaMorphAnimManagerTask.cpp sets it up. Names other
 * than the class's are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/BitTable.h"
#include <string.h>

extern "C" {
/* Game heap: free, allocate and allocate cleared. */
void fn_80049B34(void *block);
void *fn_80049B90(u32 size, int heap);
void *fn_80049B60(s32 count, u32 size, int heap);
}

namespace SonicteamUSA {
namespace System {

BitTable::BitTable(u32 columns, u32 rows, bool set) {
    m_columns = columns;
    m_rows = rows;
    m_rowWords = (columns + 31) >> 5;
    int size = m_rows * (m_rowWords * 4);
    if (set) {
        m_bits = (u32 *)fn_80049B90(size, 2);
        memset(m_bits, 0xFF, size);
    } else {
        m_bits = (u32 *)fn_80049B60(1, size, 2);
    }
}

BitTable::~BitTable() {
    fn_80049B34(m_bits);
    m_bits = 0;
}

BitTable &BitTable::operator=(const BitTable &other) {
    if (this == &other) {
        return *this;
    }
    m_columns = other.m_columns;
    m_rows = other.m_rows;
    m_rowWords = other.m_rowWords;
    if (m_bits != 0) {
        fn_80049B34(m_bits);
    }
    u32 size = m_rows * (((m_columns + 31) >> 5) * sizeof(u32));
    m_bits = (u32 *)fn_80049B90(size, 2);
    memcpy(m_bits, other.m_bits, size);
    return *this;
}

void BitTable::Set(u32 column, u32 row) {
    m_bits[(column >> 5) + row * m_rowWords] |= 1 << (column & 31);
}

void BitTable::Clear(u32 column, u32 row) {
    m_bits[(column >> 5) + row * m_rowWords] &= ~(1 << (column & 31));
}

bool BitTable::Get(u32 column, u32 row) const {
    return (m_bits[(column >> 5) + row * m_rowWords] & (1 << (column & 31))) != 0;
}

} // namespace System
} // namespace SonicteamUSA
