#include "test_runner.hpp"
#include "../modules/119_accessibility_auditor/accessibility_auditor.hpp"

namespace {

testing::TestOutcome run_accessibility_auditor() {
    accessibility_auditor::init();
    auto s = accessibility_auditor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    accessibility_auditor::shutdown();
    return testing::ok();
}

const int _reg_accessibility_auditor = testing::register_test(
    "accessibility_auditor",
    "119_accessibility_auditor: stub status check",
    &run_accessibility_auditor);

}
