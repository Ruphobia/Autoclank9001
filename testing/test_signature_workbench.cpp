#include "test_runner.hpp"
#include "../modules/373_signature_workbench/signature_workbench.hpp"

namespace {

testing::TestOutcome run_signature_workbench() {
    signature_workbench::init();
    auto s = signature_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    signature_workbench::shutdown();
    return testing::ok();
}

const int _reg_signature_workbench = testing::register_test(
    "signature_workbench",
    "373_signature_workbench: stub status check",
    &run_signature_workbench);

}
