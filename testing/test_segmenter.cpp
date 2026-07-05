// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1628_segmenter/segmenter.hpp"

namespace {

testing::TestOutcome run_segmenter() {
    segmenter::init();
    auto s = segmenter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    segmenter::shutdown();
    return testing::ok();
}

const int _reg_segmenter = testing::register_test(
    "segmenter",
    "1628_segmenter: stub status check",
    &run_segmenter);

}
