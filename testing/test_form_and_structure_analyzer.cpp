#include "test_runner.hpp"
#include "../modules/1717_form_and_structure_analyzer/form_and_structure_analyzer.hpp"

namespace {

testing::TestOutcome run_form_and_structure_analyzer() {
    form_and_structure_analyzer::init();
    auto s = form_and_structure_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    form_and_structure_analyzer::shutdown();
    return testing::ok();
}

const int _reg_form_and_structure_analyzer = testing::register_test(
    "form_and_structure_analyzer",
    "1717_form_and_structure_analyzer: stub status check",
    &run_form_and_structure_analyzer);

}
