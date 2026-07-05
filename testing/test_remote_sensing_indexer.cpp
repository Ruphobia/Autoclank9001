#include "test_runner.hpp"
#include "../modules/1572_remote_sensing_indexer/remote_sensing_indexer.hpp"

namespace {

testing::TestOutcome run_remote_sensing_indexer() {
    remote_sensing_indexer::init();
    auto s = remote_sensing_indexer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    remote_sensing_indexer::shutdown();
    return testing::ok();
}

const int _reg_remote_sensing_indexer = testing::register_test(
    "remote_sensing_indexer",
    "1572_remote_sensing_indexer: stub status check",
    &run_remote_sensing_indexer);

}
