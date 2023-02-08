/*  Copyright (C) 2023  Mikołaj Lenczewski <mblenczewski@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef HASH_H
#define HASH_H

#include "common.h"

/* default value as recommended by: http://isthe.com/chongo/tech/comp/fnv/ */
#define HASH_FNV1A_DEFAULT_SEED 0x811C9DC5
#define HASH_FNV1A_PRIME 0x01000193

/* implementation of: https://create.stephan-brumme.com/fnv-hash/
 */
static inline u32
hash_fnv1a(void* buf, u64 len, hashes_fnv1a_val_t seed) {
	assert(buf);

	u32 hash = seed;
	u8* src = (u8*)buf;

	while (len--) hash = (*src++ ^ hash) * HASH_FNV1A_PRIME;

	return hash;
}

#define HASH_XXH64_PRIME1 0x9E3779B185EBCA87ULL
#define HASH_XXH64_PRIME2 0xC2B2AE3D27D4EB4FULL
#define HASH_XXH64_PRIME3 0x165667B19E3779F9ULL
#define HASH_XXH64_PRIME4 0x85EBCA77C2B2AE63ULL
#define HASH_XXH64_PRIME5 0x27D4EB2F165667C5ULL

/* implementation of: https://github.com/Cyan4973/xxHash/blob/dev/doc/xxhash_spec.md#xxh64-algorithm-description
 */
static inline u64
hash_xxh64(void* buf, u64 len, hashes_xxh64_val_t seed) {
	assert(buf);

/* rotate x `bits` places to the left, should compile to a single CPU instruction (ROL) */
#define ROL(x, bits) (((x) << (bits)) | ((x) >> (64 - (bits))))

/* core hash step of a single accumulator  */
#define ROUND(acc, input) (ROL((acc) + ((input) * HASH_XXH64_PRIME2), 31) * HASH_XXH64_PRIME1)

/* core hash step of a 32-byte stripe (processes each of the 4 accumulators */
#define ROUND4(stripe, acc0, acc1, acc2, acc3) \
do { \
	(acc0) = ROUND(acc0, stripe[0]); \
	(acc1) = ROUND(acc1, stripe[1]); \
	(acc2) = ROUND(acc2, stripe[2]); \
	(acc3) = ROUND(acc3, stripe[3]); \
} while (0);

/* merges two accumulator values */
#define MERGE_ACC(acc, input) ((((acc) ^ ROUND(0, (input))) * HASH_XXH64_PRIME1) + HASH_XXH64_PRIME4)

	u64 hash, acc0, acc1, acc2, acc3;
	u8 const* data = (u8*)buf, * stop = data + len;

	/* STEP 1 : Initialisation of accumulators */
	/* if we don't have enough bytes to use the accumulators, then simply
	 * initialise the hash from the seed and skip to the single-lane mixing
	 * phase
	 */
	if (len < 32) {
		hash = seed + HASH_XXH64_PRIME5;
		goto hash_mixing;
	}

	/* initialise the 4 unique lane accumulators
	 */
	acc0 = seed + HASH_XXH64_PRIME1 + HASH_XXH64_PRIME2;
	acc1 = seed + HASH_XXH64_PRIME2;
	acc2 = seed;
	acc3 = seed - HASH_XXH64_PRIME1;

	/* STEP 2 : Process stripes */
	/* process all 32-byte stripes */
	for (; data + 32 <= stop; data += 32) {
		u64 const* stripe = (u64*)data;
		ROUND4(stripe, acc0, acc1, acc2, acc3);
	}

	/* STEP 3 : Merge accumulators into single u64 hash */
	hash = ROL(acc0, 1) + ROL(acc1, 7) + ROL(acc2, 12) + ROL(acc3, 18);
	hash = MERGE_ACC(hash, acc0);
	hash = MERGE_ACC(hash, acc1);
	hash = MERGE_ACC(hash, acc2);
	hash = MERGE_ACC(hash, acc3);

	/* STEP 4 : Mixing in total input length */
hash_mixing:
	hash += len;

	/* STEP 5 : Consume remaining input */
	/* process all single lanes left */
	for (; data + 8 <= stop; data += 8) {
		u64 const lane = *(u64*)data;
		hash = (ROL(hash ^ ROUND(0, lane), 27) * HASH_XXH64_PRIME1) + HASH_XXH64_PRIME4;
	}

	/* process any half-lane found */
	if (data + 4 <= stop) {
		u32 const half_lane = *(u32*)data;
		hash = (ROL(hash ^ (half_lane * HASH_XXH64_PRIME1), 23) * HASH_XXH64_PRIME2) + HASH_XXH64_PRIME3;
		data += 4;
	}

	/* take care of remaining 0..3 bytes, eat 1 byte per step */
	while (data != stop) {
		u8 const byte = *data++;
		hash = ROL(hash ^ (byte * HASH_XXH64_PRIME5), 11) * HASH_XXH64_PRIME1;
	}

	/* STEP 6 : Final mix and output */
	hash ^= hash >> 33;
	hash *= HASH_XXH64_PRIME2;
	hash ^= hash >> 29;
	hash *= HASH_XXH64_PRIME3;
	hash ^= hash >> 32;

	return hash;

#undef ROL
#undef ROUND
#undef ROUND4
#undef MERGE_ACC
}

#endif /* HASH_H */
