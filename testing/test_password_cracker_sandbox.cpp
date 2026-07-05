#include "test_runner.hpp"
#include "../modules/394_password_cracker_sandbox/password_cracker_sandbox.hpp"

namespace {

testing::TestOutcome run_password_cracker_sandbox() {
    password_cracker_sandbox::init();
    auto s = password_cracker_sandbox::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    password_cracker_sandbox::shutdown();
    return testing::ok();
}

const int _reg_password_cracker_sandbox = testing::register_test(
    "password_cracker_sandbox",
    "394_password_cracker_sandbox: stub status check",
    &run_password_cracker_sandbox);

}
