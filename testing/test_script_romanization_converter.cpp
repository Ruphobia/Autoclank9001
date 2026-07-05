#include "test_runner.hpp"
#include "../modules/797_script_romanization_converter/script_romanization_converter.hpp"

namespace {

testing::TestOutcome run_script_romanization_converter() {
    script_romanization_converter::init();
    auto s = script_romanization_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    script_romanization_converter::shutdown();
    return testing::ok();
}

const int _reg_script_romanization_converter = testing::register_test(
    "script_romanization_converter",
    "797_script_romanization_converter: stub status check",
    &run_script_romanization_converter);

}
