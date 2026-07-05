#include "test_runner.hpp"
#include "../modules/489_pick_path_optimizer/pick_path_optimizer.hpp"

namespace {

testing::TestOutcome run_pick_path_optimizer() {
    pick_path_optimizer::init();
    auto s = pick_path_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pick_path_optimizer::shutdown();
    return testing::ok();
}

const int _reg_pick_path_optimizer = testing::register_test(
    "pick_path_optimizer",
    "489_pick_path_optimizer: stub status check",
    &run_pick_path_optimizer);

}
