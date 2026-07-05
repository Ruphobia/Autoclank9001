#include "test_runner.hpp"
#include "../modules/361_strings_triage/strings_triage.hpp"

namespace {

testing::TestOutcome run_strings_triage() {
    strings_triage::init();
    auto s = strings_triage::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    strings_triage::shutdown();
    return testing::ok();
}

const int _reg_strings_triage = testing::register_test(
    "strings_triage",
    "361_strings_triage: stub status check",
    &run_strings_triage);

}
