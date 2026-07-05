#include "test_runner.hpp"
#include "../modules/575_trademark_searcher/trademark_searcher.hpp"

namespace {

testing::TestOutcome run_trademark_searcher() {
    trademark_searcher::init();
    auto s = trademark_searcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    trademark_searcher::shutdown();
    return testing::ok();
}

const int _reg_trademark_searcher = testing::register_test(
    "trademark_searcher",
    "575_trademark_searcher: stub status check",
    &run_trademark_searcher);

}
