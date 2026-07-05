#include "test_runner.hpp"
#include "../modules/990_hardness_converter/hardness_converter.hpp"

namespace {

testing::TestOutcome run_hardness_converter() {
    hardness_converter::init();
    auto s = hardness_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hardness_converter::shutdown();
    return testing::ok();
}

const int _reg_hardness_converter = testing::register_test(
    "hardness_converter",
    "990_hardness_converter: stub status check",
    &run_hardness_converter);

}
