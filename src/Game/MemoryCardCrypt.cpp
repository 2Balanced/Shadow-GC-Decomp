/* The checksum and cipher behind the memory card's save data (see Game/MemoryCardCtrl.cpp): an MD5
 * digest (RFC 1321) and a bit-per-byte DES. The controller keeps an MD5 of the save data in its
 * first 16 bytes and encrypts the whole block with a fixed DES key before writing it. File, class
 * and member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first).
 *
 * The game's MD5 pads with zero bytes only (the 0x80 marker of RFC 1321 is never written), so its
 * digests differ from the standard ones. The unit was built with automatic inlining: MD5Final
 * carries an inlined copy of Update, which `#pragma auto_inline on` reproduces.
 *
 * NonMatching: 9 of 10 functions match, and the data. CardDes::Crypt (0x80297AB0) differs in the
 * S-box step only (22 instructions): retail loads the six bits in the order 1, 2, 0, 5, 3, 4 and
 * keeps the row in r20; about 40 source forms of that step were tried under GC/2.7. */
#include "Game/MemoryCardCrypt.h"
#include <cstring>
#include <dolphin/types.h>

#pragma auto_inline on

/* DES: initial and final permutation, expansion, P permutation, S-boxes, the key's permuted choices
 * 1 and 2 and the total key rotation after each round (FIPS 46). */
static const int s_initialPermL[32] = {
    57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7,
};

static const int s_initialPermR[32] = {
    56, 48, 40, 32, 24, 16, 8, 0, 58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4, 62, 54, 46, 38, 30, 22, 14, 6,
};

static const int s_finalPerm[64] = {
    39, 7, 47, 15, 55, 23, 63, 31, 38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29, 36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27, 34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25, 32, 0, 40, 8, 48, 16, 56, 24,
};

static const int s_expansion[48] = {
    31, 0, 1, 2, 3, 4, 3, 4, 5, 6, 7, 8,
    7, 8, 9, 10, 11, 12, 11, 12, 13, 14, 15, 16,
    15, 16, 17, 18, 19, 20, 19, 20, 21, 22, 23, 24,
    23, 24, 25, 26, 27, 28, 27, 28, 29, 30, 31, 0,
};

static const int s_perm[32] = {
    15, 6, 19, 20, 28, 11, 27, 16, 0, 14, 22, 25, 4, 17, 30, 9,
    1, 7, 23, 13, 31, 26, 2, 8, 18, 12, 29, 5, 21, 10, 3, 24,
};

static const int s_sbox[8][4][16] = {
    {
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13},
    },
    {
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9},
    },
    {
        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12},
    },
    {
        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14},
    },
    {
        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3},
    },
    {
        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13},
    },
    {
        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12},
    },
    {
        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11},
    },
};

static const int s_keyChoice1[56] = {
    56, 48, 40, 32, 24, 16, 8, 0, 57, 49, 41, 33, 25, 17,
    9, 1, 58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35,
    62, 54, 46, 38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21,
    13, 5, 60, 52, 44, 36, 28, 20, 12, 4, 27, 19, 11, 3,
};

static const int s_keyChoice2[48] = {
    13, 16, 10, 23, 0, 4, 2, 27, 14, 5, 20, 9,
    22, 18, 11, 3, 25, 7, 15, 6, 26, 19, 12, 1,
    40, 51, 30, 36, 46, 54, 29, 39, 50, 44, 32, 47,
    43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31,
};

static const int s_keyRotation[16] = {
    1, 2, 4, 6, 8, 10, 12, 14, 15, 17, 19, 21, 23, 25, 27, 28,
};

/* Bit n of a byte string, most significant bit first. */
#define GETBIT(data, n) (((data)[(n) / 8] & (1 << (7 - (n) % 8))) != 0)

CardMd5::CardMd5() {}

CardMd5::~CardMd5() {
    memset(m_count, 0, sizeof(m_count));
    memset(m_state, 0, sizeof(m_state));
    memset(m_buffer, 0, sizeof(m_buffer));
}

#define MD5_F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define MD5_G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
#define MD5_I(x, y, z) ((y) ^ ((x) | ~(z)))
#define MD5_ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define MD5_STEP(f, a, b, c, d, x, s, t)            \
    {                                               \
        (a) += f((b), (c), (d)) + (x) + (u32)(t);   \
        (a) = MD5_ROTL((a), (s));                   \
        (a) += (b);                                 \
    }

void CardMd5::Transform(u32 *state, u32 *block) {
    u32 a = state[0];
    u32 b = state[1];
    u32 c = state[2];
    u32 d = state[3];

    MD5_STEP(MD5_F, a, b, c, d, block[0], 7, 0xD76AA478);
    MD5_STEP(MD5_F, d, a, b, c, block[1], 12, 0xE8C7B756);
    MD5_STEP(MD5_F, c, d, a, b, block[2], 17, 0x242070DB);
    MD5_STEP(MD5_F, b, c, d, a, block[3], 22, 0xC1BDCEEE);
    MD5_STEP(MD5_F, a, b, c, d, block[4], 7, 0xF57C0FAF);
    MD5_STEP(MD5_F, d, a, b, c, block[5], 12, 0x4787C62A);
    MD5_STEP(MD5_F, c, d, a, b, block[6], 17, 0xA8304613);
    MD5_STEP(MD5_F, b, c, d, a, block[7], 22, 0xFD469501);
    MD5_STEP(MD5_F, a, b, c, d, block[8], 7, 0x698098D8);
    MD5_STEP(MD5_F, d, a, b, c, block[9], 12, 0x8B44F7AF);
    MD5_STEP(MD5_F, c, d, a, b, block[10], 17, 0xFFFF5BB1);
    MD5_STEP(MD5_F, b, c, d, a, block[11], 22, 0x895CD7BE);
    MD5_STEP(MD5_F, a, b, c, d, block[12], 7, 0x6B901122);
    MD5_STEP(MD5_F, d, a, b, c, block[13], 12, 0xFD987193);
    MD5_STEP(MD5_F, c, d, a, b, block[14], 17, 0xA679438E);
    MD5_STEP(MD5_F, b, c, d, a, block[15], 22, 0x49B40821);

    MD5_STEP(MD5_G, a, b, c, d, block[1], 5, 0xF61E2562);
    MD5_STEP(MD5_G, d, a, b, c, block[6], 9, 0xC040B340);
    MD5_STEP(MD5_G, c, d, a, b, block[11], 14, 0x265E5A51);
    MD5_STEP(MD5_G, b, c, d, a, block[0], 20, 0xE9B6C7AA);
    MD5_STEP(MD5_G, a, b, c, d, block[5], 5, 0xD62F105D);
    MD5_STEP(MD5_G, d, a, b, c, block[10], 9, 0x02441453);
    MD5_STEP(MD5_G, c, d, a, b, block[15], 14, 0xD8A1E681);
    MD5_STEP(MD5_G, b, c, d, a, block[4], 20, 0xE7D3FBC8);
    MD5_STEP(MD5_G, a, b, c, d, block[9], 5, 0x21E1CDE6);
    MD5_STEP(MD5_G, d, a, b, c, block[14], 9, 0xC33707D6);
    MD5_STEP(MD5_G, c, d, a, b, block[3], 14, 0xF4D50D87);
    MD5_STEP(MD5_G, b, c, d, a, block[8], 20, 0x455A14ED);
    MD5_STEP(MD5_G, a, b, c, d, block[13], 5, 0xA9E3E905);
    MD5_STEP(MD5_G, d, a, b, c, block[2], 9, 0xFCEFA3F8);
    MD5_STEP(MD5_G, c, d, a, b, block[7], 14, 0x676F02D9);
    MD5_STEP(MD5_G, b, c, d, a, block[12], 20, 0x8D2A4C8A);

    MD5_STEP(MD5_H, a, b, c, d, block[5], 4, 0xFFFA3942);
    MD5_STEP(MD5_H, d, a, b, c, block[8], 11, 0x8771F681);
    MD5_STEP(MD5_H, c, d, a, b, block[11], 16, 0x6D9D6122);
    MD5_STEP(MD5_H, b, c, d, a, block[14], 23, 0xFDE5380C);
    MD5_STEP(MD5_H, a, b, c, d, block[1], 4, 0xA4BEEA44);
    MD5_STEP(MD5_H, d, a, b, c, block[4], 11, 0x4BDECFA9);
    MD5_STEP(MD5_H, c, d, a, b, block[7], 16, 0xF6BB4B60);
    MD5_STEP(MD5_H, b, c, d, a, block[10], 23, 0xBEBFBC70);
    MD5_STEP(MD5_H, a, b, c, d, block[13], 4, 0x289B7EC6);
    MD5_STEP(MD5_H, d, a, b, c, block[0], 11, 0xEAA127FA);
    MD5_STEP(MD5_H, c, d, a, b, block[3], 16, 0xD4EF3085);
    MD5_STEP(MD5_H, b, c, d, a, block[6], 23, 0x04881D05);
    MD5_STEP(MD5_H, a, b, c, d, block[9], 4, 0xD9D4D039);
    MD5_STEP(MD5_H, d, a, b, c, block[12], 11, 0xE6DB99E5);
    MD5_STEP(MD5_H, c, d, a, b, block[15], 16, 0x1FA27CF8);
    MD5_STEP(MD5_H, b, c, d, a, block[2], 23, 0xC4AC5665);

    MD5_STEP(MD5_I, a, b, c, d, block[0], 6, 0xF4292244);
    MD5_STEP(MD5_I, d, a, b, c, block[7], 10, 0x432AFF97);
    MD5_STEP(MD5_I, c, d, a, b, block[14], 15, 0xAB9423A7);
    MD5_STEP(MD5_I, b, c, d, a, block[5], 21, 0xFC93A039);
    MD5_STEP(MD5_I, a, b, c, d, block[12], 6, 0x655B59C3);
    MD5_STEP(MD5_I, d, a, b, c, block[3], 10, 0x8F0CCC92);
    MD5_STEP(MD5_I, c, d, a, b, block[10], 15, 0xFFEFF47D);
    MD5_STEP(MD5_I, b, c, d, a, block[1], 21, 0x85845DD1);
    MD5_STEP(MD5_I, a, b, c, d, block[8], 6, 0x6FA87E4F);
    MD5_STEP(MD5_I, d, a, b, c, block[15], 10, 0xFE2CE6E0);
    MD5_STEP(MD5_I, c, d, a, b, block[6], 15, 0xA3014314);
    MD5_STEP(MD5_I, b, c, d, a, block[13], 21, 0x4E0811A1);
    MD5_STEP(MD5_I, a, b, c, d, block[4], 6, 0xF7537E82);
    MD5_STEP(MD5_I, d, a, b, c, block[11], 10, 0xBD3AF235);
    MD5_STEP(MD5_I, c, d, a, b, block[2], 15, 0x2AD7D2BB);
    MD5_STEP(MD5_I, b, c, d, a, block[9], 21, 0xEB86D391);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

void CardMd5::Init() {
    m_count[0] = 0;
    m_count[1] = 0;
    m_state[0] = 0x67452301;
    m_state[1] = 0xEFCDAB89;
    m_state[2] = 0x98BADCFE;
    m_state[3] = 0x10325476;
}

void CardMd5::Update(const void *input, u32 size) {
    const u8 *data = (const u8 *)input;
    u32 block[16];
    int index;
    u32 i;
    u32 j;

    index = (m_count[0] >> 3) & 0x3F;
    if (m_count[0] + (size << 3) < m_count[0]) {
        m_count[1]++;
    }
    m_count[0] += size << 3;
    m_count[1] += size >> 29;
    while (size--) {
        m_buffer[index++] = *data++;
        if (index == 0x40) {
            for (i = 0, j = 0; i < 16; i++, j += 4) {
                block[i] = ((u32)m_buffer[j + 3] << 24) | ((u32)m_buffer[j + 2] << 16) |
                           ((u32)m_buffer[j + 1] << 8) | (u32)m_buffer[j];
            }
            Transform(m_state, block);
            index = 0;
        }
    }
}

void CardMd5::Final(u8 *digest) {
    u8 padding[64];
    u32 tail[16];
    int index;
    u32 size;
    u32 i;
    u32 j;

    memset(padding, 0, sizeof(padding));
    tail[14] = m_count[0];
    tail[15] = m_count[1];
    index = (m_count[0] >> 3) & 0x3F;
    size = (index < 56) ? (56 - index) : (120 - index);

    Update(padding, size);

    for (i = 0, j = 0; i < 14; i++, j += 4) {
        tail[i] = ((u32)m_buffer[j + 3] << 24) | ((u32)m_buffer[j + 2] << 16) |
                  ((u32)m_buffer[j + 1] << 8) | (u32)m_buffer[j];
    }
    Transform(m_state, tail);

    for (i = 0, j = 0; i < 4; i++, j += 4) {
        digest[j] = m_state[i];
        digest[j + 1] = m_state[i] >> 8;
        digest[j + 2] = m_state[i] >> 16;
        digest[j + 3] = m_state[i] >> 24;
    }
}

CardDes::CardDes() {
    memset(m_keys, 0, sizeof(m_keys));
}

CardDes::~CardDes() {
    memset(m_keys, 0, sizeof(m_keys));
}

void CardDes::SetKey(const char *key) {
    char bits[56];
    char rotated[56];
    int i;
    int j;

    memset(bits, 0, sizeof(bits));
    for (i = 0; i < 56; i++) {
        bits[i] = GETBIT(key, s_keyChoice1[i]);
    }
    for (i = 0; i < 16; i++) {
        char *round = m_keys[i];
        for (j = 0; j < 28; j++) {
            int k = (j + s_keyRotation[i]) % 28;
            rotated[j] = bits[k];
            rotated[j + 28] = bits[k + 28];
        }
        for (j = 0; j < 48; j++) {
            round[j] = rotated[s_keyChoice2[j]];
        }
    }
}

static inline void SBox(int n, const char *x, char *out) {
    int col = (x[1] << 3) | (x[2] << 2);
    int row = (x[0] << 1) | x[5];
    col |= (x[3] << 1) | x[4];
    char v = s_sbox[n][row][col];
    out[0] = (v >> 3) & 1;
    out[1] = (v >> 2) & 1;
    out[2] = (v >> 1) & 1;
    out[3] = v & 1;
}

void CardDes::Crypt(const char *in, char *out, int encrypt) {
    char mixed[48];
    char lr[64];
    char *left = lr;
    char *right = lr + 32;
    char expanded[48];
    char sboxOut[32];
    char f[32];
    int i;
    int j;

    for (j = 0; j < 32; j++) {
        left[j] = GETBIT(in, s_initialPermL[j]);
        right[j] = GETBIT(in, s_initialPermR[j]);
    }
    for (i = 0; i < 16; i++) {
        const char *key = m_keys[encrypt ? i : 15 - i];
        for (j = 0; j < 48; j++) {
            expanded[j] = right[s_expansion[j]];
        }
        for (j = 0; j < 48; j++) {
            mixed[j] = expanded[j] ^ key[j];
        }
        for (j = 0; j < 8; j++) {
            SBox(j, &mixed[j * 6], &sboxOut[j * 4]);
        }
        for (j = 0; j < 32; j++) {
            f[j] = sboxOut[s_perm[j]];
        }
        for (j = 0; j < 32; j++) {
            char t = left[j];
            left[j] = right[j];
            right[j] = f[j] ^ t;
        }
    }
    memset(out, 0, 8);
    for (j = 0; j < 64; j++) {
        int n = s_finalPerm[j];
        if (n < 32) {
            if (right[n]) {
                out[j / 8] |= 1 << (7 - j % 8);
            }
        } else if (left[n - 32]) {
            out[j / 8] |= 1 << (7 - j % 8);
        }
    }
}
