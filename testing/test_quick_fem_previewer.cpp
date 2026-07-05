#include "test_runner.hpp"
#include "../modules/1120_quick_fem_previewer/quick_fem_previewer.hpp"

namespace {

testing::TestOutcome run_quick_fem_previewer() {
    quick_fem_previewer::init();
    auto s = quick_fem_previewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    quick_fem_previewer::shutdown();
    return testing::ok();
}

const int _reg_quick_fem_previewer = testing::register_test(
    "quick_fem_previewer",
    "1120_quick_fem_previewer: stub status check",
    &run_quick_fem_previewer);

}
