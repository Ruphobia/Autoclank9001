#include "test_runner.hpp"
#include "../modules/2152_hypnogram_viewer/hypnogram_viewer.hpp"

namespace {

testing::TestOutcome run_hypnogram_viewer() {
    hypnogram_viewer::init();
    auto s = hypnogram_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hypnogram_viewer::shutdown();
    return testing::ok();
}

const int _reg_hypnogram_viewer = testing::register_test(
    "hypnogram_viewer",
    "2152_hypnogram_viewer: stub status check",
    &run_hypnogram_viewer);

}
