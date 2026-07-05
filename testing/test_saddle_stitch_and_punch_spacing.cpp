#include "test_runner.hpp"
#include "../modules/1854_saddle_stitch_and_punch_spacing/saddle_stitch_and_punch_spacing.hpp"

namespace {

testing::TestOutcome run_saddle_stitch_and_punch_spacing() {
    saddle_stitch_and_punch_spacing::init();
    auto s = saddle_stitch_and_punch_spacing::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    saddle_stitch_and_punch_spacing::shutdown();
    return testing::ok();
}

const int _reg_saddle_stitch_and_punch_spacing = testing::register_test(
    "saddle_stitch_and_punch_spacing",
    "1854_saddle_stitch_and_punch_spacing: stub status check",
    &run_saddle_stitch_and_punch_spacing);

}
