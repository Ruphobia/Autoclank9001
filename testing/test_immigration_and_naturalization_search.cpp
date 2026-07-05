#include "test_runner.hpp"
#include "../modules/2138_immigration_and_naturalization_search/immigration_and_naturalization_search.hpp"

namespace {

testing::TestOutcome run_immigration_and_naturalization_search() {
    immigration_and_naturalization_search::init();
    auto s = immigration_and_naturalization_search::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    immigration_and_naturalization_search::shutdown();
    return testing::ok();
}

const int _reg_immigration_and_naturalization_search = testing::register_test(
    "immigration_and_naturalization_search",
    "2138_immigration_and_naturalization_search: stub status check",
    &run_immigration_and_naturalization_search);

}
