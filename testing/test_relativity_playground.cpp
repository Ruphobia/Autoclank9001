#include "test_runner.hpp"
#include "../modules/912_relativity_playground/relativity_playground.hpp"

namespace {

testing::TestOutcome run_relativity_playground() {
    relativity_playground::init();
    auto s = relativity_playground::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    relativity_playground::shutdown();
    return testing::ok();
}

const int _reg_relativity_playground = testing::register_test(
    "relativity_playground",
    "912_relativity_playground: stub status check",
    &run_relativity_playground);

}
