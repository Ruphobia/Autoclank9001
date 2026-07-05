#include "test_runner.hpp"
#include "../modules/817_translation_workbench/translation_workbench.hpp"

namespace {

testing::TestOutcome run_translation_workbench() {
    translation_workbench::init();
    auto s = translation_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    translation_workbench::shutdown();
    return testing::ok();
}

const int _reg_translation_workbench = testing::register_test(
    "translation_workbench",
    "817_translation_workbench: stub status check",
    &run_translation_workbench);

}
