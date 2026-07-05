#include "test_runner.hpp"
#include "../modules/1458_hodograph_severe_parameters/hodograph_severe_parameters.hpp"

namespace {

testing::TestOutcome run_hodograph_severe_parameters() {
    hodograph_severe_parameters::init();
    auto s = hodograph_severe_parameters::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hodograph_severe_parameters::shutdown();
    return testing::ok();
}

const int _reg_hodograph_severe_parameters = testing::register_test(
    "hodograph_severe_parameters",
    "1458_hodograph_severe_parameters: stub status check",
    &run_hodograph_severe_parameters);

}
