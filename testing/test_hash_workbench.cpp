#include "test_runner.hpp"
#include "../modules/350_hash_workbench/hash_workbench.hpp"

namespace {

testing::TestOutcome run_hash_workbench() {
    hash_workbench::init();
    auto s = hash_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hash_workbench::shutdown();
    return testing::ok();
}

const int _reg_hash_workbench = testing::register_test(
    "hash_workbench",
    "350_hash_workbench: stub status check",
    &run_hash_workbench);

}
