#include "test_runner.hpp"
#include "../modules/583_statute_search/statute_search.hpp"

namespace {

testing::TestOutcome run_statute_search() {
    statute_search::init();
    auto s = statute_search::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    statute_search::shutdown();
    return testing::ok();
}

const int _reg_statute_search = testing::register_test(
    "statute_search",
    "583_statute_search: stub status check",
    &run_statute_search);

}
