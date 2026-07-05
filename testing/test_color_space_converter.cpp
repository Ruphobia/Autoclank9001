#include "test_runner.hpp"
#include "../modules/1864_color_space_converter/color_space_converter.hpp"

namespace {

testing::TestOutcome run_color_space_converter() {
    color_space_converter::init();
    auto s = color_space_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    color_space_converter::shutdown();
    return testing::ok();
}

const int _reg_color_space_converter = testing::register_test(
    "color_space_converter",
    "1864_color_space_converter: stub status check",
    &run_color_space_converter);

}
