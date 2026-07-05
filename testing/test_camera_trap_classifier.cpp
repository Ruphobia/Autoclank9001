#include "test_runner.hpp"
#include "../modules/1499_camera_trap_classifier/camera_trap_classifier.hpp"

namespace {

testing::TestOutcome run_camera_trap_classifier() {
    camera_trap_classifier::init();
    auto s = camera_trap_classifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    camera_trap_classifier::shutdown();
    return testing::ok();
}

const int _reg_camera_trap_classifier = testing::register_test(
    "camera_trap_classifier",
    "1499_camera_trap_classifier: stub status check",
    &run_camera_trap_classifier);

}
