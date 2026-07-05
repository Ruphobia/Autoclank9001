#include "test_runner.hpp"
#include "../modules/579_redline_diff/redline_diff.hpp"

namespace {

testing::TestOutcome run_redline_diff() {
    redline_diff::init();
    auto s = redline_diff::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    redline_diff::shutdown();
    return testing::ok();
}

const int _reg_redline_diff = testing::register_test(
    "redline_diff",
    "579_redline_diff: stub status check",
    &run_redline_diff);

}
