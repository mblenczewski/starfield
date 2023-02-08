/*  Copyright (C) 2023  Mikołaj Lenczewski <mblenczewski@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <setjmp.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <wchar.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

struct str_view {
	char *ptr;
	u64 len;
};

static inline bool
str_view_equal(struct str_view a, struct str_view b) {
	return a.len == b.len && strncmp(a.ptr, b.ptr, a.len) == 0;
}

#define CONST_CSTR_SV(ccstr) \
	((struct str_view){ .ptr = ccstr, .len = sizeof(ccstr) - 1, })

#define CSTR_SV(cstr) \
	((struct str_view){ .ptr = cstr, .len = strlen(cstr), })

#define ARRLEN(arr) (sizeof (arr) / sizeof *(arr))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define VERIFY_NOT_REACHED(...)							\
do {										\
	fprintf(stderr, "%s:%d:%s: VERIFY_NOT_REACHED()", __FILE__, __LINE__, __func__);\
	fprintf(stderr, __VA_ARGS__);						\
	fprintf(stderr, "\n");							\
	fflush(stderr);								\
	abort();								\
} while (0);

struct mem_stream {
	u8 *ptr;
	u64 cur, len;
};

static inline bool
mem_stream_eof(struct mem_stream *self) {
	assert(self);

	return self->len == self->cur;
}

static inline u8 *
mem_stream_headptr(struct mem_stream *self) {
	assert(self);

	return self->ptr + self->cur;
}

static inline bool
mem_stream_skip(struct mem_stream *self, size_t len) {
	assert(self);

	if (self->len - self->cur < len)
		return false;

	self->cur += len;

	return true;
}

static inline bool
mem_stream_peek(struct mem_stream *self, size_t off, void *buf, size_t len) {
	assert(self);
	assert(buf);

	if (self->len - off - self->cur < len)
		return false;

	memcpy(buf, self->ptr + self->cur + off, len);

	return true;
}

static inline bool
mem_stream_consume(struct mem_stream *self, void *buf, size_t len) {
	assert(self);
	assert(buf);

	return mem_stream_peek(self, 0, buf, len) && mem_stream_skip(self, len);
}

#define KiB 1024
#define MiB 1024 * KiB
#define GiB 1024 * MiB

#define MEM_POOL_BLOCK_SIZE 4 * MiB

struct mem_pool {
	u8 *ptr;
	u64 cap, len;
};

static inline bool
mem_pool_resize(struct mem_pool *self, size_t capacity) {
	assert(self);

	u8 *ptr = realloc(self->ptr, capacity);
	if (!ptr) return false;

	self->ptr = ptr;
	self->cap = capacity;

	return true;
}

static inline bool
mem_pool_init(struct mem_pool *self, size_t capacity) {
	assert(self);

	self->ptr = NULL;
	self->cap = self->len = 0;

	return mem_pool_resize(self, capacity);
}

static inline void
mem_pool_free(struct mem_pool *self) {
	assert(self);

	free(self->ptr);
}

static inline void
mem_pool_reset(struct mem_pool *self) {
	assert(self);

	self->len = 0;
}

static inline bool
mem_pool_prealloc(struct mem_pool *self, size_t size) {
	assert(self);

	if (self->cap < self->len + size && !mem_pool_resize(self, self->len + size))
		return false;

	return true;
}

static inline void *
mem_pool_alloc(struct mem_pool *self, size_t size) {
	assert(self);

	if (!mem_pool_prealloc(self, size))
		return NULL;

	void *ptr = self->ptr + self->len;
	self->len += size;

	return ptr;
}

static inline void *
mem_pool_aligned_alloc(struct mem_pool *self, size_t alignment, size_t size) {
	assert(self);
	assert(alignment);
	assert(alignment % 2 == 0);

	size_t alignment_mask = alignment - 1;
	size_t aligned_start_len = (self->len + alignment_mask) & ~alignment_mask;
	size_t padding = aligned_start_len - self->len;

	if (!mem_pool_prealloc(self, padding + size))
		return NULL;

	void *ptr = self->ptr + self->len + padding;
	self->len += padding + size;

	return ptr;
}

#define RELPTR_MASK(relptr_ty) (1 << ((sizeof(relptr_ty) * 8) - 1))
#define RELPTR_NULL (0)

#define RELPTR_ENC(relptr_ty, ptroff) \
	((relptr_ty)((ptroff) ^ RELPTR_MASK(relptr_ty)))

#define RELPTR_DEC(relptr_ty, relptr) \
	((relptr_ty)((relptr) ^ RELPTR_MASK(relptr_ty)))

#define RELPTR_ABS2REL(absptr_ty, relptr_ty, base, absptr) \
	((absptr) \
	 ? RELPTR_ENC(relptr_ty, (u8 *)(absptr) - (u8 *)(base)) \
	 : RELPTR_NULL)

#define RELPTR_REL2ABS(absptr_ty, relptr_ty, base, relptr) \
	((relptr) \
	 ? (absptr_ty)((u8 *)(base) + RELPTR_DEC(relptr_ty, relptr)) \
	 : NULL)

#define OFFPTR_NULL (-1)

#define OFFPTR_ABS2OFF(absptr_ty, offptr_ty, base, absptr) \
	((offptr_ty)((absptr) - (absptr_ty)(base)))

#define OFFPTR_OFF2ABS(absptr_ty, offptr_ty, base, offptr) \
	((offptr) < (offptr_ty)OFFPTR_NULL \
	 ? ((absptr_ty)(base) + (offptr_ty)offptr) \
	 : NULL)

#endif /* COMMON_H */
