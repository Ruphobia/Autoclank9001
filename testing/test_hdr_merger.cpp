// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1675_hdr_merger/hdr_merger.hpp"

namespace {

testing::TestOutcome run_hdr_merger() {
    hdr_merger::init();
    auto s = hdr_merger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hdr_merger::shutdown();
    return testing::ok();
}

const int _reg_hdr_merger = testing::register_test(
    "hdr_merger",
    "1675_hdr_merger: stub status check",
    &run_hdr_merger);

}
