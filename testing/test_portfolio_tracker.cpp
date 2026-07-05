#include "test_runner.hpp"
#include "../modules/559_portfolio_tracker/portfolio_tracker.hpp"

namespace {

testing::TestOutcome run_portfolio_tracker() {
    portfolio_tracker::init();
    auto s = portfolio_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    portfolio_tracker::shutdown();
    return testing::ok();
}

const int _reg_portfolio_tracker = testing::register_test(
    "portfolio_tracker",
    "559_portfolio_tracker: stub status check",
    &run_portfolio_tracker);

}
