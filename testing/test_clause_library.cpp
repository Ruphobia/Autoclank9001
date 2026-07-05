#include "test_runner.hpp"
#include "../modules/580_clause_library/clause_library.hpp"

namespace {

testing::TestOutcome run_clause_library() {
    clause_library::init();
    auto s = clause_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    clause_library::shutdown();
    return testing::ok();
}

const int _reg_clause_library = testing::register_test(
    "clause_library",
    "580_clause_library: stub status check",
    &run_clause_library);

}
