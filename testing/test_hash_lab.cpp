#include "test_runner.hpp"
#include "../modules/375_hash_lab/hash_lab.hpp"

namespace {

testing::TestOutcome run_hash_lab() {
    hash_lab::init();
    auto s = hash_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hash_lab::shutdown();
    return testing::ok();
}

const int _reg_hash_lab = testing::register_test(
    "hash_lab",
    "375_hash_lab: stub status check",
    &run_hash_lab);

}
