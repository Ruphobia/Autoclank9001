#include "test_runner.hpp"
#include "../modules/1960_language_and_script_lab/language_and_script_lab.hpp"

namespace {

testing::TestOutcome run_language_and_script_lab() {
    language_and_script_lab::init();
    auto s = language_and_script_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    language_and_script_lab::shutdown();
    return testing::ok();
}

const int _reg_language_and_script_lab = testing::register_test(
    "language_and_script_lab",
    "1960_language_and_script_lab: stub status check",
    &run_language_and_script_lab);

}
