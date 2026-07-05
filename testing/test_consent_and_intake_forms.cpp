#include "test_runner.hpp"
#include "../modules/1906_consent_and_intake_forms/consent_and_intake_forms.hpp"

namespace {

testing::TestOutcome run_consent_and_intake_forms() {
    consent_and_intake_forms::init();
    auto s = consent_and_intake_forms::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    consent_and_intake_forms::shutdown();
    return testing::ok();
}

const int _reg_consent_and_intake_forms = testing::register_test(
    "consent_and_intake_forms",
    "1906_consent_and_intake_forms: stub status check",
    &run_consent_and_intake_forms);

}
