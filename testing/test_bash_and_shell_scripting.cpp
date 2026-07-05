#include "test_runner.hpp"
#include "../modules/107_bash_and_shell_scripting/bash_and_shell_scripting.hpp"

namespace {

testing::TestOutcome run_bash_and_shell_scripting() {
    bash_and_shell_scripting::init();
    auto s = bash_and_shell_scripting::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bash_and_shell_scripting::shutdown();
    return testing::ok();
}

const int _reg_bash_and_shell_scripting = testing::register_test(
    "bash_and_shell_scripting",
    "107_bash_and_shell_scripting: stub status check",
    &run_bash_and_shell_scripting);

}
