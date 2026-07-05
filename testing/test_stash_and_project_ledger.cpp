#include "test_runner.hpp"
#include "../modules/1782_stash_and_project_ledger/stash_and_project_ledger.hpp"

namespace {

testing::TestOutcome run_stash_and_project_ledger() {
    stash_and_project_ledger::init();
    auto s = stash_and_project_ledger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stash_and_project_ledger::shutdown();
    return testing::ok();
}

const int _reg_stash_and_project_ledger = testing::register_test(
    "stash_and_project_ledger",
    "1782_stash_and_project_ledger: stub status check",
    &run_stash_and_project_ledger);

}
