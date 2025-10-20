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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "ironseed.h"

TEST_CASE("Ironseed: default constructed") {
  ironseed_input_t *hash = ironseed_input_create(256);

  CHECK(ironseed_input_size(hash) == 8);

  CHECK(ironseed_input_digests(hash)[0] == 5915939354302563870);
  CHECK(ironseed_input_digests(hash)[7] == 795621534831891799);

  ironseed_t *fe = ironseed_create_from_input(hash);

  CHECK(ironseed_size(fe) == 8);

  CHECK(ironseed_values(fe)[0] == -2128494816);
  CHECK(ironseed_values(fe)[1] == 1928268316);
  CHECK(ironseed_values(fe)[2] == -1098770175);
  CHECK(ironseed_values(fe)[3] == -309390410);
  CHECK(ironseed_values(fe)[4] == 1233806517);
  CHECK(ironseed_values(fe)[5] == 656251397);
  CHECK(ironseed_values(fe)[6] == -1726969757);
  CHECK(ironseed_values(fe)[7] == 1158962031);

  ironseed_input_free(hash);
  ironseed_free(fe);
}

TEST_CASE("Ironseed: input=1") {
  ironseed_input_t *hash = ironseed_input_create(256);

  ironseed_input_update(hash, 1);

  ironseed_t *fe = ironseed_create_from_input(hash);

  CHECK(ironseed_size(fe) == 8);

  CHECK(ironseed_values(fe)[0] == 1100802175);
  CHECK(ironseed_values(fe)[1] == -412525365);
  CHECK(ironseed_values(fe)[2] == 1477556999);
  CHECK(ironseed_values(fe)[3] == 1670677042);
  CHECK(ironseed_values(fe)[4] == 281748010);
  CHECK(ironseed_values(fe)[5] == 494767993);
  CHECK(ironseed_values(fe)[6] == 808804019);
  CHECK(ironseed_values(fe)[7] == -864784934);

  ironseed_input_free(hash);
  ironseed_free(fe);
}