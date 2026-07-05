#include "test_runner.hpp"
#include "../modules/380_ssh_toolkit/ssh_toolkit.hpp"

namespace {

testing::TestOutcome run_ssh_toolkit() {
    ssh_toolkit::init();
    auto s = ssh_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ssh_toolkit::shutdown();
    return testing::ok();
}

const int _reg_ssh_toolkit = testing::register_test(
    "ssh_toolkit",
    "380_ssh_toolkit: stub status check",
    &run_ssh_toolkit);

}
