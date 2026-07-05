#include "test_runner.hpp"
#include "../modules/323_foveated_render_budget/foveated_render_budget.hpp"

namespace {

testing::TestOutcome run_foveated_render_budget() {
    foveated_render_budget::init();
    auto s = foveated_render_budget::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    foveated_render_budget::shutdown();
    return testing::ok();
}

const int _reg_foveated_render_budget = testing::register_test(
    "foveated_render_budget",
    "323_foveated_render_budget: stub status check",
    &run_foveated_render_budget);

}
