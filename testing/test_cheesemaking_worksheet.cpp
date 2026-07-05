#include "test_runner.hpp"
#include "../modules/1796_cheesemaking_worksheet/cheesemaking_worksheet.hpp"

namespace {

testing::TestOutcome run_cheesemaking_worksheet() {
    cheesemaking_worksheet::init();
    auto s = cheesemaking_worksheet::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cheesemaking_worksheet::shutdown();
    return testing::ok();
}

const int _reg_cheesemaking_worksheet = testing::register_test(
    "cheesemaking_worksheet",
    "1796_cheesemaking_worksheet: stub status check",
    &run_cheesemaking_worksheet);

}
