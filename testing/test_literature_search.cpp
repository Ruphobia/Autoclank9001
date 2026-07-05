#include "test_runner.hpp"
#include "../modules/1406_literature_search/literature_search.hpp"

namespace {

testing::TestOutcome run_literature_search() {
    literature_search::init();
    auto s = literature_search::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    literature_search::shutdown();
    return testing::ok();
}

const int _reg_literature_search = testing::register_test(
    "literature_search",
    "1406_literature_search: stub status check",
    &run_literature_search);

}
