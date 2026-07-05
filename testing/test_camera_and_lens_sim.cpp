#include "test_runner.hpp"
#include "../modules/1696_camera_and_lens_sim/camera_and_lens_sim.hpp"

namespace {

testing::TestOutcome run_camera_and_lens_sim() {
    camera_and_lens_sim::init();
    auto s = camera_and_lens_sim::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    camera_and_lens_sim::shutdown();
    return testing::ok();
}

const int _reg_camera_and_lens_sim = testing::register_test(
    "camera_and_lens_sim",
    "1696_camera_and_lens_sim: stub status check",
    &run_camera_and_lens_sim);

}
