#include "test_runner.hpp"
#include "../modules/479_tsp_playground/tsp_playground.hpp"

namespace {

testing::TestOutcome run_tsp_playground() {
    tsp_playground::init();
    auto s = tsp_playground::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tsp_playground::shutdown();
    return testing::ok();
}

const int _reg_tsp_playground = testing::register_test(
    "tsp_playground",
    "479_tsp_playground: stub status check",
    &run_tsp_playground);

}
