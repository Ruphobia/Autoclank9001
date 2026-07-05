#include "test_runner.hpp"
#include "../modules/242_query_history_and_snippets/query_history_and_snippets.hpp"

namespace {

testing::TestOutcome run_query_history_and_snippets() {
    query_history_and_snippets::init();
    auto s = query_history_and_snippets::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    query_history_and_snippets::shutdown();
    return testing::ok();
}

const int _reg_query_history_and_snippets = testing::register_test(
    "query_history_and_snippets",
    "242_query_history_and_snippets: stub status check",
    &run_query_history_and_snippets);

}
