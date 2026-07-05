#include "test_runner.hpp"
#include "../modules/2015_pet_training_log/pet_training_log.hpp"

namespace {

testing::TestOutcome run_pet_training_log() {
    pet_training_log::init();
    auto s = pet_training_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pet_training_log::shutdown();
    return testing::ok();
}

const int _reg_pet_training_log = testing::register_test(
    "pet_training_log",
    "2015_pet_training_log: stub status check",
    &run_pet_training_log);

}
