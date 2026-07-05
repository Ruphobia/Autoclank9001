#include "test_runner.hpp"
#include "../modules/149_har_viewer_and_diff/har_viewer_and_diff.hpp"

namespace {

testing::TestOutcome run_har_viewer_and_diff() {
    har_viewer_and_diff::init();
    auto s = har_viewer_and_diff::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    har_viewer_and_diff::shutdown();
    return testing::ok();
}

const int _reg_har_viewer_and_diff = testing::register_test(
    "har_viewer_and_diff",
    "149_har_viewer_and_diff: stub status check",
    &run_har_viewer_and_diff);

}
