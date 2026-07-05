#include "test_runner.hpp"
#include "../modules/1719_tuning_and_temperament_lab/tuning_and_temperament_lab.hpp"

namespace {

testing::TestOutcome run_tuning_and_temperament_lab() {
    tuning_and_temperament_lab::init();
    auto s = tuning_and_temperament_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tuning_and_temperament_lab::shutdown();
    return testing::ok();
}

const int _reg_tuning_and_temperament_lab = testing::register_test(
    "tuning_and_temperament_lab",
    "1719_tuning_and_temperament_lab: stub status check",
    &run_tuning_and_temperament_lab);

}
