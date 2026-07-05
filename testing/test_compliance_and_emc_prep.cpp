#include "test_runner.hpp"
#include "../modules/220_compliance_and_emc_prep/compliance_and_emc_prep.hpp"

namespace {

testing::TestOutcome run_compliance_and_emc_prep() {
    compliance_and_emc_prep::init();
    auto s = compliance_and_emc_prep::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    compliance_and_emc_prep::shutdown();
    return testing::ok();
}

const int _reg_compliance_and_emc_prep = testing::register_test(
    "compliance_and_emc_prep",
    "220_compliance_and_emc_prep: stub status check",
    &run_compliance_and_emc_prep);

}
