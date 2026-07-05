#include "test_runner.hpp"
#include "../modules/799_conlang_workbench/conlang_workbench.hpp"

namespace {

testing::TestOutcome run_conlang_workbench() {
    conlang_workbench::init();
    auto s = conlang_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    conlang_workbench::shutdown();
    return testing::ok();
}

const int _reg_conlang_workbench = testing::register_test(
    "conlang_workbench",
    "799_conlang_workbench: stub status check",
    &run_conlang_workbench);

}
