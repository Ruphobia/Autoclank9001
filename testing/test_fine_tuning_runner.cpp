#include "test_runner.hpp"
#include "../modules/673_fine_tuning_runner/fine_tuning_runner.hpp"

namespace {

testing::TestOutcome run_fine_tuning_runner() {
    fine_tuning_runner::init();
    auto s = fine_tuning_runner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fine_tuning_runner::shutdown();
    return testing::ok();
}

const int _reg_fine_tuning_runner = testing::register_test(
    "fine_tuning_runner",
    "673_fine_tuning_runner: stub status check",
    &run_fine_tuning_runner);

}
