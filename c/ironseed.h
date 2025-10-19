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

#ifndef FRAGMITES_IRONSEED_H
#define FRAGMITES_IRONSEED_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ironseed_input;
typedef struct ironseed_input ironseed_input_t;

struct ironseed;
typedef struct ironseed ironseed_t;

ironseed_input_t *ironseed_input_create(size_t bits);
ironseed_input_t *ironseed_input_clone(const ironseed_input_t *p);
void ironseed_input_free(ironseed_input_t *p);
size_t ironseed_input_size(const ironseed_input_t *p);

void ironseed_input_update(ironseed_input_t *p, uint32_t value);
void ironseed_input_update_u32(ironseed_input_t *p, uint32_t value);
void ironseed_input_update_u64(ironseed_input_t *p, uint64_t value);
void ironseed_input_update_dbl(ironseed_input_t *p, double value);
void ironseed_input_update_flt(ironseed_input_t *p, float value);
void ironseed_input_update_ptr(ironseed_input_t *p, const void *value);
void ironseed_input_update_obj(ironseed_input_t *p, const void *value,
                               size_t length);
void ironseed_input_update_buf(ironseed_input_t *p, const uint8_t *value,
                               size_t length);
void ironseed_input_update_str(ironseed_input_t *p, const char *value);
void ironseed_input_update_fun(ironseed_input_t *p, void (*value)(void));

ironseed_t *ironseed_create(const uint32_t *p, size_t size);
ironseed_t *ironseed_create_from_input(const ironseed_input_t *p);
ironseed_t *ironseed_clone(const ironseed_t *p);
void ironseed_free(ironseed_t *p);

size_t ironseed_size(const ironseed_t *p);
const uint32_t *ironseed_values(const ironseed_t *p);

uint32_t ironseed_next_seed(ironseed_t *p);
void ironseed_fill_seeds(ironseed_t *p, uint32_t *a, size_t length);
uint64_t ironseed_restart_seeds(ironseed_t *p);

#ifdef __cplusplus
}
#endif

#endif
