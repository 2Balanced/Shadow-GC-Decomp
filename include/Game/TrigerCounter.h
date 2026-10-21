#ifndef SHADOW_GAME_TRIGERCOUNTER_H
#define SHADOW_GAME_TRIGERCOUNTER_H

/* The manager's std::map is declared only where TRIGERCOUNTER_MAP is defined before this header
 * (Game/TrigerCounter.cpp): <map>'s template declarations would advance CodeWarrior's @N counter
 * in the other units that include it. */
#ifdef TRIGERCOUNTER_MAP
#include <map>
#endif

#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

#ifdef TRIGERCOUNTER_MAP
/* TrigerCounter (class name from RTTI, spelling as in the game): a count towards a stage switch.
 * Member names are inferred. */
class TrigerCounter {
  public:
    TrigerCounter(int target, int start) : m_count(start), m_target(target), m_start(start) {}
    ~TrigerCounter() {}

    void Reset() {
        m_count = m_start;
    }

    /* 0x0 */ int m_count;
    /* 0x4 */ int m_target;
    /* 0x8 */ int m_start;
};
SIZE_ASSERT(TrigerCounter, 0xC);
#endif

/* The counters of the candlesticks' switches, by switch number (class name unknown; method names
 * inferred). The counters exist only between Enable (the candlestick's stage load) and Disable;
 * each lit candlestick adds one, and the switch turns on when its count reaches the number of
 * candlesticks it needs. Game/CandleStick.cpp holds the instance; Game/TrigerCounter.cpp the
 * methods. */
class UnkCandleSwitchManager_801660A0 {
  public:
    UnkCandleSwitchManager_801660A0();
    ~UnkCandleSwitchManager_801660A0();

    void Enable();
    void Disable();
    /* Sets every count back to its start (when the stage restarts, inferred). */
    void ResetCounters();
    void Register(u8 id, int target, int start);
    bool IsRegistered(u8 id) const;
    void Add(u8 id, int amount);
#ifdef TRIGERCOUNTER_MAP
    /* Not called anywhere (see its definition). */
    bool IsReached(u8 id) const;
    boost::shared_ptr<TrigerCounter> Get(u8 id) const;
#endif

    static UnkCandleSwitchManager_801660A0 *s_cached;

#ifdef TRIGERCOUNTER_MAP
    /* 0x00 */ std::map<u8, boost::shared_ptr<TrigerCounter> > m_counters;
#else
    /* 0x00 */ u32 m_counters[4];
#endif
    /* 0x10 */ bool m_enabled;
};
SIZE_ASSERT(UnkCandleSwitchManager_801660A0, 0x14);

#endif
