#include "test_runner.hpp"
#include "../modules/2006_swarm_and_queen_lineage_tracker/swarm_and_queen_lineage_tracker.hpp"

namespace {

testing::TestOutcome run_swarm_and_queen_lineage_tracker() {
    swarm_and_queen_lineage_tracker::init();
    auto s = swarm_and_queen_lineage_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    swarm_and_queen_lineage_tracker::shutdown();
    return testing::ok();
}

const int _reg_swarm_and_queen_lineage_tracker = testing::register_test(
    "swarm_and_queen_lineage_tracker",
    "2006_swarm_and_queen_lineage_tracker: stub status check",
    &run_swarm_and_queen_lineage_tracker);

}
