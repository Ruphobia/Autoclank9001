#include "test_runner.hpp"
#include "../modules/1560_mixed_layer_depth_detector/mixed_layer_depth_detector.hpp"

namespace {

testing::TestOutcome run_mixed_layer_depth_detector() {
    mixed_layer_depth_detector::init();
    auto s = mixed_layer_depth_detector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mixed_layer_depth_detector::shutdown();
    return testing::ok();
}

const int _reg_mixed_layer_depth_detector = testing::register_test(
    "mixed_layer_depth_detector",
    "1560_mixed_layer_depth_detector: stub status check",
    &run_mixed_layer_depth_detector);

}
