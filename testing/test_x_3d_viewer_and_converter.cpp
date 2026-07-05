#include "test_runner.hpp"
#include "../modules/1664_x_3d_viewer_and_converter/x_3d_viewer_and_converter.hpp"

namespace {

testing::TestOutcome run_x_3d_viewer_and_converter() {
    x_3d_viewer_and_converter::init();
    auto s = x_3d_viewer_and_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    x_3d_viewer_and_converter::shutdown();
    return testing::ok();
}

const int _reg_x_3d_viewer_and_converter = testing::register_test(
    "x_3d_viewer_and_converter",
    "1664_x_3d_viewer_and_converter: stub status check",
    &run_x_3d_viewer_and_converter);

}
