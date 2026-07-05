#include "test_runner.hpp"
#include "../modules/614_consent_and_lawful_basis_tracker/consent_and_lawful_basis_tracker.hpp"

namespace {

testing::TestOutcome run_consent_and_lawful_basis_tracker() {
    consent_and_lawful_basis_tracker::init();
    auto s = consent_and_lawful_basis_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    consent_and_lawful_basis_tracker::shutdown();
    return testing::ok();
}

const int _reg_consent_and_lawful_basis_tracker = testing::register_test(
    "consent_and_lawful_basis_tracker",
    "614_consent_and_lawful_basis_tracker: stub status check",
    &run_consent_and_lawful_basis_tracker);

}
