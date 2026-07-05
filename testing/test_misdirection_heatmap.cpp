#include "test_runner.hpp"
#include "../modules/1941_misdirection_heatmap/misdirection_heatmap.hpp"

namespace {

testing::TestOutcome run_misdirection_heatmap() {
    misdirection_heatmap::init();
    auto s = misdirection_heatmap::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    misdirection_heatmap::shutdown();
    return testing::ok();
}

const int _reg_misdirection_heatmap = testing::register_test(
    "misdirection_heatmap",
    "1941_misdirection_heatmap: stub status check",
    &run_misdirection_heatmap);

}
