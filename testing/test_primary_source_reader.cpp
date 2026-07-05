// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/777_primary_source_reader/primary_source_reader.hpp"

namespace {

testing::TestOutcome run_primary_source_reader() {
    primary_source_reader::init();
    auto s = primary_source_reader::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    primary_source_reader::shutdown();
    return testing::ok();
}

const int _reg_primary_source_reader = testing::register_test(
    "primary_source_reader",
    "777_primary_source_reader: stub status check",
    &run_primary_source_reader);

}
