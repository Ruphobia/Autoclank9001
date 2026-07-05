// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/812_orthography_mapper/orthography_mapper.hpp"

namespace {

testing::TestOutcome run_orthography_mapper() {
    orthography_mapper::init();
    auto s = orthography_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    orthography_mapper::shutdown();
    return testing::ok();
}

const int _reg_orthography_mapper = testing::register_test(
    "orthography_mapper",
    "812_orthography_mapper: stub status check",
    &run_orthography_mapper);

}
