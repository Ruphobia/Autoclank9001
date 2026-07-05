#include "test_runner.hpp"
#include "../modules/1038_seismic_load_calculator/seismic_load_calculator.hpp"

namespace {

testing::TestOutcome run_seismic_load_calculator() {
    seismic_load_calculator::init();
    auto s = seismic_load_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    seismic_load_calculator::shutdown();
    return testing::ok();
}

const int _reg_seismic_load_calculator = testing::register_test(
    "seismic_load_calculator",
    "1038_seismic_load_calculator: stub status check",
    &run_seismic_load_calculator);

}
