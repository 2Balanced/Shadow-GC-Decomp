/* TrigerCounter (class name from RTTI, spelling as in the game): a count towards a stage switch.
 * The candlesticks' switch manager keeps one per switch number in a std::map; each lit
 * candlestick adds one, and the switch turns on when the count reaches the number the switch
 * needs. File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#define MSL_PAIR_CONVERTING_CONSTRUCTOR
#define MSL_TREE_BOOL_OBJECTS
#define TRIGERCOUNTER_MAP
#include "Game/TrigerCounter.h"
#include "Game/UnkSwitchBank_800CB69C.h"

/* Not called anywhere: the linker strips it from the game. Retail's .sdata holds the RTTI records
 * of the counter's shared-pointer control block, then insert_unique's result flags, then the
 * deleter's RTTI record. GC/2.7 creates all three records together after the source is read, so
 * this typeid creates the first two up front and the flags are named objects defined where
 * <map_impl> is included (see MSL_TREE_BOOL_OBJECTS there). */
const std::type_info &ControlBlockType() {
    return typeid(boost::detail::sp_counted_base_impl<TrigerCounter *, boost::checked_deleter<TrigerCounter> >);
}

UnkCandleSwitchManager_801660A0::UnkCandleSwitchManager_801660A0() {
    m_enabled = false;
}

UnkCandleSwitchManager_801660A0::~UnkCandleSwitchManager_801660A0() {}

void UnkCandleSwitchManager_801660A0::Enable() {
    if (!m_enabled) {
        m_counters.clear();
        m_enabled = true;
    }
}

void UnkCandleSwitchManager_801660A0::Disable() {
    if (m_enabled) {
        m_counters.clear();
        m_enabled = false;
    }
}

void UnkCandleSwitchManager_801660A0::ResetCounters() {
    if (m_enabled) {
        std::map<u8, boost::shared_ptr<TrigerCounter> >::iterator it;
        for (it = m_counters.begin(); it != m_counters.end(); it++) {
            it->second->Reset();
        }
    }
}

void UnkCandleSwitchManager_801660A0::Register(u8 id, int target, int start) {
    boost::shared_ptr<TrigerCounter> counter(new TrigerCounter(target, start));
    m_counters.insert(std::pair<u8, boost::shared_ptr<TrigerCounter> >(id, counter));
}

bool UnkCandleSwitchManager_801660A0::IsRegistered(u8 id) const {
    std::map<u8, boost::shared_ptr<TrigerCounter> >::const_iterator it = m_counters.find(id);
    return it != m_counters.end();
}

void UnkCandleSwitchManager_801660A0::Add(u8 id, int amount) {
    boost::shared_ptr<TrigerCounter> counter(Get(id));
    counter->m_count += amount;
    if (counter->m_count >= counter->m_target) {
        fn_801690D0(fn_800CB69C(), id, 1);
    }
}

/* Not called anywhere: the linker strips it from the game. Retail emits the shared pointer's
 * destructor between Get's instances and Add, where the instances of a function defined here
 * would go (inferred). */
bool UnkCandleSwitchManager_801660A0::IsReached(u8 id) const {
    boost::shared_ptr<TrigerCounter> counter(Get(id));
    return counter->m_count >= counter->m_target;
}

boost::shared_ptr<TrigerCounter> UnkCandleSwitchManager_801660A0::Get(u8 id) const {
    return (*m_counters.find(id)).second;
}

#include <map_impl>
