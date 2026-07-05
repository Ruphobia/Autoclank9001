#include "test_runner.hpp"
#include "../modules/1654_image_diff/image_diff.hpp"

namespace {

testing::TestOutcome run_image_diff() {
    image_diff::init();
    auto s = image_diff::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    image_diff::shutdown();
    return testing::ok();
}

const int _reg_image_diff = testing::register_test(
    "image_diff",
    "1654_image_diff: stub status check",
    &run_image_diff);

}
