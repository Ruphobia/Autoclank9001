#include "test_runner.hpp"
#include "../modules/557_reconciliation_studio/reconciliation_studio.hpp"

namespace {

testing::TestOutcome run_reconciliation_studio() {
    reconciliation_studio::init();
    auto s = reconciliation_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reconciliation_studio::shutdown();
    return testing::ok();
}

const int _reg_reconciliation_studio = testing::register_test(
    "reconciliation_studio",
    "557_reconciliation_studio: stub status check",
    &run_reconciliation_studio);

}
