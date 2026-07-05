#include "test_runner.hpp"
#include "../modules/1781_smocking_pleat_calculator/smocking_pleat_calculator.hpp"

namespace {

testing::TestOutcome run_smocking_pleat_calculator() {
    smocking_pleat_calculator::init();
    auto s = smocking_pleat_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    smocking_pleat_calculator::shutdown();
    return testing::ok();
}

const int _reg_smocking_pleat_calculator = testing::register_test(
    "smocking_pleat_calculator",
    "1781_smocking_pleat_calculator: stub status check",
    &run_smocking_pleat_calculator);

}
