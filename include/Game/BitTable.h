#ifndef GAME_BITTABLE_H
#define GAME_BITTABLE_H

#include <dolphin/types.h>
#include "size_assert.h"

/* SonicteamUSA::System::BitTable (class name from RTTI; vtable 0x8051CE78): a table of bits, one
 * row of m_columns bits per row, each row padded to whole words. Game/BitTable.cpp. Names other
 * than the class's are inferred. */
namespace SonicteamUSA {
namespace System {

class BitTable {
  public:
    BitTable(u32 columns, u32 rows, bool set);
    virtual ~BitTable();
    BitTable &operator=(const BitTable &other);
    void Set(u32 column, u32 row);
    void Clear(u32 column, u32 row);
    bool Get(u32 column, u32 row) const;

    /* 0x04 */ u32 m_columns;
    /* 0x08 */ u32 m_rows;
    /* 0x0C */ u32 m_rowWords;
    /* 0x10 */ u32 *m_bits;
};
SIZE_ASSERT(BitTable, 0x14);

} // namespace System
} // namespace SonicteamUSA

#endif
