#include "test_runner.hpp"
#include "../modules/571_subscription_auditor/subscription_auditor.hpp"

namespace {

testing::TestOutcome run_subscription_auditor() {
    subscription_auditor::init();
    auto s = subscription_auditor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    subscription_auditor::shutdown();
    return testing::ok();
}

const int _reg_subscription_auditor = testing::register_test(
    "subscription_auditor",
    "571_subscription_auditor: stub status check",
    &run_subscription_auditor);

}
