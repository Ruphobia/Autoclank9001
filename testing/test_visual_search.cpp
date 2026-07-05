#include "test_runner.hpp"
#include "../modules/1631_visual_search/visual_search.hpp"

namespace {

testing::TestOutcome run_visual_search() {
    visual_search::init();
    auto s = visual_search::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    visual_search::shutdown();
    return testing::ok();
}

const int _reg_visual_search = testing::register_test(
    "visual_search",
    "1631_visual_search: stub status check",
    &run_visual_search);

}
