#include "test_runner.hpp"
#include "../modules/1774_beading_pattern_layout/beading_pattern_layout.hpp"

namespace {

testing::TestOutcome run_beading_pattern_layout() {
    beading_pattern_layout::init();
    auto s = beading_pattern_layout::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    beading_pattern_layout::shutdown();
    return testing::ok();
}

const int _reg_beading_pattern_layout = testing::register_test(
    "beading_pattern_layout",
    "1774_beading_pattern_layout: stub status check",
    &run_beading_pattern_layout);

}
