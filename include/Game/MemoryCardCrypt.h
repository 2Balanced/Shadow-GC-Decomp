#ifndef GAME_MEMORYCARDCRYPT_H
#define GAME_MEMORYCARDCRYPT_H

#include <dolphin/types.h>
#include "size_assert.h"

/* The checksum and cipher the memory card controller (Game/MemoryCardCtrl.cpp) applies to the save
 * data before it writes it and after it reads it. Both are textbook algorithms; the class and
 * member names are inferred. */

/* An MD5 digest under construction (RFC 1321). */
class CardMd5 {
  public:
    CardMd5();
    ~CardMd5();
    void Transform(u32 *state, u32 *block);
    void Init();
    void Update(const void *input, u32 size);
    void Final(u8 *digest);

    /* 0x00 */ u32 m_count[2]; /* message length in bits, low word first */
    /* 0x08 */ u32 m_state[4];
    /* 0x18 */ u8 m_buffer[64];
};
SIZE_ASSERT(CardMd5, 0x58);

/* A DES key schedule: the 16 round keys, one byte per bit. */
class CardDes {
  public:
    CardDes();
    ~CardDes();
    void SetKey(const char *key);
    /* Encrypts (encrypt != 0) or decrypts one 8-byte block. */
    void Crypt(const char *in, char *out, int encrypt);

    /* 0x000 */ char m_keys[16][48];
};
SIZE_ASSERT(CardDes, 0x300);

#endif
