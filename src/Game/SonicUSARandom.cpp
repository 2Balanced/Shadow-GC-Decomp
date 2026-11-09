/* SonicUSA::System::Random (class name from RTTI): a Mersenne Twister (MT19937) random number
 * generator, and the game-wide one other units draw from by the fn_ names of the four functions
 * that use it (Game/Random.h). The library is built without small data (its RTTI record is in
 * .data). Member names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include <cmath>
#include <dolphin/types.h>
#include "size_assert.h"

namespace SonicUSA {
namespace System {

class Random {
  public:
    /* The state size and shift (unsigned, which keeps the state loops' indexing as retail has it). */
    static const u32 N = 624;
    static const u32 M = 397;

    Random(u32 seed);
    virtual ~Random();

    u32 GetInt(u32 count);
    float GetFloat();
    float GetRange(float lo, float hi);
    float GetFloatClosed();

    /* The next 32 random bits (MT19937's genrand_int32). */
    u32 Next() {
        static u32 mag01[2] = {0x0, 0x9908B0DF};
        u32 y;
        if (m_index >= N) {
            int kk;
            for (kk = 0; kk < N - M; kk++) {
                y = (m_state[kk] & 0x80000000) | (m_state[kk + 1] & 0x7FFFFFFF);
                m_state[kk] = m_state[kk + M] ^ (y >> 1) ^ mag01[y & 1];
            }
            for (; kk < N - 1; kk++) {
                y = (m_state[kk] & 0x80000000) | (m_state[kk + 1] & 0x7FFFFFFF);
                m_state[kk] = m_state[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 1];
            }
            y = (m_state[N - 1] & 0x80000000) | (m_state[0] & 0x7FFFFFFF);
            m_state[N - 1] = m_state[M - 1] ^ (y >> 1) ^ mag01[y & 1];
            m_index = 0;
        }
        y = m_state[m_index++];
        y ^= y >> 11;
        y ^= (y << 7) & 0x9D2C5680;
        y ^= (y << 15) & 0xEFC60000;
        y ^= y >> 18;
        return y;
    }

    /* 0x004 */ u32 m_state[N];
    /* 0x9C4 */ u32 m_index;
};
SIZE_ASSERT(Random, 0x9C8);

} // namespace System
} // namespace SonicUSA

using SonicUSA::System::Random;

extern "C" {
u32 fn_8040EA90(u32 count);
float fn_8040EC80(float lo, float hi);
float fn_8040ECF8(void);
float fn_8040EE90(void);
}

Random lbl_805E07D4(0xBADED9A7);

Random::Random(u32 seed) {
    m_state[0] = seed;
    for (m_index = 1; m_index < N; m_index++) {
        m_state[m_index] = 1812433253 * (m_state[m_index - 1] ^ (m_state[m_index - 1] >> 30)) + m_index;
    }
}

Random::~Random() {}

float fn_8040EE90(void) {
    return lbl_805E07D4.GetFloat();
}

float Random::GetFloatClosed() {
    return (1.0 / 4294967295.0) * Next();
}

float fn_8040ECF8(void) {
    return lbl_805E07D4.GetFloatClosed();
}

float Random::GetRange(float lo, float hi) {
    return (hi - lo) * GetFloat() + lo;
}

float fn_8040EC80(float lo, float hi) {
    return lbl_805E07D4.GetRange(lo, hi);
}

float Random::GetFloat() {
    return (1.0 / 4294967296.0) * Next();
}

u32 Random::GetInt(u32 count) {
    return std::floor(GetFloat() * count);
}

u32 fn_8040EA90(u32 count) {
    return lbl_805E07D4.GetInt(count);
}
