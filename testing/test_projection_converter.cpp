#include "test_runner.hpp"
#include "../modules/1581_projection_converter/projection_converter.hpp"

namespace {

testing::TestOutcome run_projection_converter() {
    projection_converter::init();
    auto s = projection_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    projection_converter::shutdown();
    return testing::ok();
}

const int _reg_projection_converter = testing::register_test(
    "projection_converter",
    "1581_projection_converter: stub status check",
    &run_projection_converter);

}
