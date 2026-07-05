#include "test_runner.hpp"
#include "../modules/229_query_builder/query_builder.hpp"

namespace {

testing::TestOutcome run_query_builder() {
    query_builder::init();
    auto s = query_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    query_builder::shutdown();
    return testing::ok();
}

const int _reg_query_builder = testing::register_test(
    "query_builder",
    "229_query_builder: stub status check",
    &run_query_builder);

}
