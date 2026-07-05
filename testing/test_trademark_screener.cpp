#include "test_runner.hpp"
#include "../modules/591_trademark_screener/trademark_screener.hpp"

namespace {

testing::TestOutcome run_trademark_screener() {
    trademark_screener::init();
    auto s = trademark_screener::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    trademark_screener::shutdown();
    return testing::ok();
}

const int _reg_trademark_screener = testing::register_test(
    "trademark_screener",
    "591_trademark_screener: stub status check",
    &run_trademark_screener);

}
