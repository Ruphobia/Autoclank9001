#include "test_runner.hpp"
#include "../modules/1926_calling_script_formatter/calling_script_formatter.hpp"

namespace {

testing::TestOutcome run_calling_script_formatter() {
    calling_script_formatter::init();
    auto s = calling_script_formatter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    calling_script_formatter::shutdown();
    return testing::ok();
}

const int _reg_calling_script_formatter = testing::register_test(
    "calling_script_formatter",
    "1926_calling_script_formatter: stub status check",
    &run_calling_script_formatter);

}
