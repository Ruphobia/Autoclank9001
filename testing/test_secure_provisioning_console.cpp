#include "test_runner.hpp"
#include "../modules/195_secure_provisioning_console/secure_provisioning_console.hpp"

namespace {

testing::TestOutcome run_secure_provisioning_console() {
    secure_provisioning_console::init();
    auto s = secure_provisioning_console::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    secure_provisioning_console::shutdown();
    return testing::ok();
}

const int _reg_secure_provisioning_console = testing::register_test(
    "secure_provisioning_console",
    "195_secure_provisioning_console: stub status check",
    &run_secure_provisioning_console);

}
