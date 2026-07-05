#include "test_runner.hpp"
#include "../modules/384_blockchain_explorer/blockchain_explorer.hpp"

namespace {

testing::TestOutcome run_blockchain_explorer() {
    blockchain_explorer::init();
    auto s = blockchain_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    blockchain_explorer::shutdown();
    return testing::ok();
}

const int _reg_blockchain_explorer = testing::register_test(
    "blockchain_explorer",
    "384_blockchain_explorer: stub status check",
    &run_blockchain_explorer);

}
