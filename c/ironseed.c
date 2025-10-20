/*
# MIT License
#
# Copyright (c) 2025 Reed A. Cartwright <racartwright@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#include "ironseed.h"

static constexpr uint64_t PRIME_A = 0xc9f736a1a00d1f5f;
static constexpr uint64_t PRIME_B = 0x88226cde0de826bf;
static constexpr uint64_t PRIME_C = 0x278abb429678dd43;
static constexpr uint64_t PRIME_D = 0x7aa8bb10afef725b;

static inline uint64_t hash_input_coef_start(void) { return PRIME_A; }

static inline uint64_t hash_input_coef_next(uint64_t *m) {
  return *m += PRIME_B;
}

static inline uint64_t hash_output_coef_start(void) { return PRIME_C; }

static inline uint64_t hash_output_coef_next(uint64_t *m) {
  return *m += PRIME_D;
}

// Adapted from mix32 of Java's SplittableRandom algorithm
// This is variant 4 of Stafford's mixing algorithms.
// http://zimbry.blogspot.com/2011/09/better-bit-mixing-improving-on.html
static inline uint32_t finalmix(uint64_t u) {
  u = (u ^ (u >> 33)) * 0x62a9d9ed799705f5;
  u = (u ^ (u >> 28)) * 0xcb24d0a5c88c35b3;
  return u >> 32;
}

struct ironseed_input {
  uint64_t coef;
  size_t size;
  uint64_t digests[];
};

struct ironseed {
  uint64_t coef;
  size_t size;
  uint32_t values[];
};

ironseed_input_t *ironseed_input_create(size_t bits) {
  if (bits == 0) {
    return NULL;
  }
  const size_t length = 2 * (((bits - 1) / 64) + 1);

  ironseed_input_t *p =
      malloc(sizeof(ironseed_input_t) + length * sizeof(uint64_t));

  p->size = length;
  p->coef = hash_input_coef_start();
  for (size_t i = 0; i < length; ++i) {
    p->digests[i] = hash_input_coef_next(&p->coef);
  }

  return p;
}

ironseed_input_t *ironseed_input_clone(const ironseed_input_t *p) {
  if (p == NULL) {
    return NULL;
  }

  const size_t length = p->size;
  ironseed_input_t *q =
      malloc(sizeof(ironseed_input_t) + length * sizeof(uint64_t));

  q->coef = p->coef;
  q->size = p->size;
  for (size_t i = 0; i < length; ++i) {
    q->digests[i] = p->digests[i];
  }

  return q;
}

void ironseed_input_free(ironseed_input_t *p) {
  if (p == NULL) {
    return;
  }
  free(p);
}

size_t ironseed_input_size(const ironseed_input_t *p) { return p->size; }
const uint64_t *ironseed_input_digests(const ironseed_input_t *p) {
  return p->digests;
}

void ironseed_input_update(ironseed_input_t *p, uint32_t value) {
  if (p == NULL) {
    return;
  }
  for (size_t i = 0; i < p->size; ++i) {
    p->digests[i] += hash_input_coef_next(&p->coef) * value;
  }
}

void ironseed_input_update_u32(ironseed_input_t *p, uint32_t value) {
  ironseed_input_update(p, value);
}

void ironseed_input_update_u64(ironseed_input_t *p, uint64_t value) {
  ironseed_input_update(p, (uint32_t)value);
  ironseed_input_update(p, (uint32_t)(value >> 32));
}

void ironseed_input_update_dbl(ironseed_input_t *p, double value) {
  uint64_t u;
  memcpy(&u, &value, sizeof(u));
  ironseed_input_update_u64(p, u);
}

void ironseed_input_update_flt(ironseed_input_t *p, float value) {
  uint32_t u;
  memcpy(&u, &value, sizeof(u));
  ironseed_input_update_u32(p, u);
}

void ironseed_input_update_ptr(ironseed_input_t *p, const void *value) {
  ironseed_input_update_u64(p, (uint64_t)((uintptr_t)value));
}

void ironseed_input_update_fun(ironseed_input_t *p, void (*value)(void)) {
  ironseed_input_update_u64(p, (uint64_t)((uintptr_t)value));
}

void ironseed_input_update_obj(ironseed_input_t *p, const void *obj,
                               size_t len) {

  const uint8_t *buf = (const uint8_t *)obj;
  size_t i = 0;
  for (; i + 4 < len; i += 4) {
    uint32_t u;
    memcpy(&u, buf, sizeof(u));
    ironseed_input_update(p, u);
    buf += 4;
  }
  uint32_t u = 0;
  memcpy(&u, buf, len - i);
  ironseed_input_update(p, u);
}

void ironseed_input_update_buf(ironseed_input_t *p, const uint8_t *str,
                               size_t len) {
  if (p == NULL) {
    return;
  }
  ironseed_input_update(p, (uint32_t)len);
  if (len == 0) {
    return;
  }
  ironseed_input_update_obj(p, str, len);
}

void ironseed_input_update_str(ironseed_input_t *p, const char *str) {
  ironseed_input_update_buf(p, (const uint8_t *)str, strlen(str));
}

ironseed_t *ironseed_create(const uint32_t *p, size_t size) {
  if (size == 0) {
    return NULL;
  }

  const size_t length = size + (size & 1); // round up to even
  ironseed_t *q = malloc(sizeof(ironseed_t) + length * sizeof(uint32_t));

  q->coef = hash_output_coef_start();
  q->size = length;

  for (size_t j = 0; j < length; ++j) {
    if (p != NULL && j < size) {
      q->values[j] = p[j];
    } else {
      q->values[j] = 0;
    }
  }

  return q;
}

ironseed_t *ironseed_create_from_input(const ironseed_input_t *p) {
  if (p == NULL) {
    return NULL;
  }

  const size_t length = p->size;

  ironseed_t *q = malloc(sizeof(ironseed_t) + length * sizeof(uint32_t));

  q->size = length;
  q->coef = hash_output_coef_start();

  uint64_t k = p->coef;
  for (size_t i = 0; i < length; ++i) {
    uint64_t u = p->digests[i] + hash_input_coef_next(&k);
    q->values[i] = finalmix(u);
  }

  return q;
}

ironseed_t *ironseed_clone(const ironseed_t *p) {
  if (p == NULL) {
    return NULL;
  }

  const size_t length = p->size;
  ironseed_t *q = malloc(sizeof(ironseed_t) + length * sizeof(uint32_t));

  q->coef = p->coef;
  q->size = p->size;
  for (size_t i = 0; i < length; ++i) {
    q->values[i] = p->values[i];
  }

  return q;
}

void ironseed_free(ironseed_t *p) {
  if (p == NULL) {
    return;
  }
  free(p);
}

size_t ironseed_size(const ironseed_t *p) { return p->size; }
const uint32_t *ironseed_values(const ironseed_t *p) { return p->values; }

uint32_t ironseed_next_seed(ironseed_t *p) {
  if (p == NULL) {
    return 0;
  }

  uint64_t v = hash_output_coef_next(&p->coef);
  for (int j = 0; j < p->size; ++j) {
    v += hash_output_coef_next(&p->coef) * p->values[j];
  }

  return finalmix(v);
}

void ironseed_fill_seeds(ironseed_t *p, uint32_t *a, size_t length) {
  if (p == NULL) {
    return;
  }

  for (int j = 0; j < length; ++j) {
    a[j] = ironseed_next_seed(p);
  }
}

uint64_t ironseed_restart_seeds(ironseed_t *p) {
  if (p == NULL) {
    return 0;
  }

  uint64_t u = p->coef;
  p->coef = hash_output_coef_start();
  return u;
}
