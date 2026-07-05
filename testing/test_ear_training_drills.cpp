#include "test_runner.hpp"
#include "../modules/1710_ear_training_drills/ear_training_drills.hpp"

namespace {

testing::TestOutcome run_ear_training_drills() {
    ear_training_drills::init();
    auto s = ear_training_drills::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ear_training_drills::shutdown();
    return testing::ok();
}

const int _reg_ear_training_drills = testing::register_test(
    "ear_training_drills",
    "1710_ear_training_drills: stub status check",
    &run_ear_training_drills);

}
