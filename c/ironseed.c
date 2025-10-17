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

static inline uint64_t init_hash4i_coef(void) { return PRIME_A; }

static inline uint64_t hash4i_coef(uint64_t *m) { return *m += PRIME_B; }

static inline uint64_t init_hash4o_coef(void) { return PRIME_C; }

static inline uint64_t hash4o_coef(uint64_t *m) { return *m += PRIME_D; }

struct ironseed_input {
  size_t num_digests;
  uint64_t coef;
  uint64_t digests[];
};

struct ironseed {
  size_t num_seeds;
  uint32_t seeds[];
};

ironseed_input_t *ironseed_input_create(size_t bits) {
  if (bits == 0) {
    return NULL;
  }
  size_t length = 2 * (((bits - 1) / 64) + 1);

  ironseed_input_t *p =
      malloc(sizeof(ironseed_input_t) + length * sizeof(uint64_t));

  p->num_digests = length;
  p->coef = init_hash4i_coef();
  for (size_t i = 0; i < p->num_digests; ++i) {
    p->digests[i] = hash4i_coef(&p->coef);
  }

  return p;
}

void ironseed_input_free(ironseed_input_t *p) {
  if (p == NULL) {
    return;
  }
  free(p);
}

void ironseed_input_update(ironseed_input_t *p, uint32_t value) {
  if (p == NULL) {
    return;
  }
  for (size_t i = 0; i < p->num_digests; ++i) {
    p->digests[i] += hash4i_coef(&p->coef) * value;
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

void ironseed_input_update_obj(ironseed_input_t *p, const void *obj,
                               size_t len) {

  const char *buf = (const char *)obj;
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

void ironseed_input_update_buf(ironseed_input_t *p, const char *str,
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
  ironseed_input_update_buf(p, str, strlen(str));
}
