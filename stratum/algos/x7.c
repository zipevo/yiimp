#include "x11.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "../sha3/sph_blake.h"
#include "../sha3/sph_bmw.h"
#include "../sha3/sph_groestl.h"
#include "../sha3/sph_keccak.h"
#include "../sha3/sph_skein.h"
#include "../sha3/sph_luffa.h"
#include "../sha3/sph_echo.h"

// Add timestamp to the function signature
void x7_hash(const char* input, char* output, uint32_t len, uint64_t timestamp)
{
    sph_blake512_context     ctx_blake;
    sph_bmw512_context       ctx_bmw;
    sph_groestl512_context   ctx_groestl;
    sph_skein512_context     ctx_skein;
    sph_keccak512_context    ctx_keccak;
    sph_luffa512_context     ctx_luffa1;
    sph_echo512_context      ctx_echo1;

    uint32_t hashA[16], hashB[16];
    unsigned char temp1[64], temp2[64];

    // Incorporate the timestamp into the initial data
    sph_blake512_init(&ctx_blake);
    sph_blake512(&ctx_blake, &timestamp, sizeof(timestamp));
    sph_blake512(&ctx_blake, input, len);
    sph_blake512_close(&ctx_blake, hashA);

    sph_bmw512_init(&ctx_bmw);
    sph_bmw512(&ctx_bmw, hashA, 64);
    sph_bmw512_close(&ctx_bmw, hashB);

    // XOR operation after the first two algorithms
    memcpy(temp1, hashA, 64);
    memcpy(temp2, hashB, 64);
    for (int i = 0; i < 64; ++i) {
        temp2[i] ^= temp1[i];
    }
    memcpy(hashB, temp2, 64);

    sph_groestl512_init(&ctx_groestl);
    sph_groestl512(&ctx_groestl, hashB, 64);
    sph_groestl512_close(&ctx_groestl, hashA);

    sph_skein512_init(&ctx_skein);
    sph_skein512(&ctx_skein, hashA, 64);
    sph_skein512_close(&ctx_skein, hashB);

    // XOR operation after the third and fourth algorithms
    memcpy(temp1, hashA, 64);
    memcpy(temp2, hashB, 64);
    for (int i = 0; i < 64; ++i) {
        temp2[i] ^= temp1[i];
    }
    memcpy(hashB, temp2, 64);

    sph_keccak512_init(&ctx_keccak);
    sph_keccak512(&ctx_keccak, hashA, 64);
    sph_keccak512_close(&ctx_keccak, hashB);

    sph_luffa512_init(&ctx_luffa1);
    sph_luffa512(&ctx_luffa1, hashB, 64);
    sph_luffa512_close(&ctx_luffa1, hashA)

    sph_echo512_init(&ctx_echo1);
    sph_echo512(&ctx_echo1, hashB, 64);
    sph_echo512_close(&ctx_echo1, hashA);

    // Final XOR operation
    memcpy(temp1, hashB, 64);
    memcpy(temp2, hashA, 64);
    for (int i = 0; i < 64; ++i) {
        temp2[i] ^= temp1[i];
    }
    memcpy(hashA, temp2, 64);

    memcpy(output, hashA, 32);
}
