#include "test_runner.hpp"
#include "../modules/2088_fantasy_draft_optimizer/fantasy_draft_optimizer.hpp"

namespace {

testing::TestOutcome run_fantasy_draft_optimizer() {
    fantasy_draft_optimizer::init();
    auto s = fantasy_draft_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fantasy_draft_optimizer::shutdown();
    return testing::ok();
}

const int _reg_fantasy_draft_optimizer = testing::register_test(
    "fantasy_draft_optimizer",
    "2088_fantasy_draft_optimizer: stub status check",
    &run_fantasy_draft_optimizer);

}
