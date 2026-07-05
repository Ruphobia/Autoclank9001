#include "test_runner.hpp"
#include "../modules/1104_cross_section_viewer/cross_section_viewer.hpp"

namespace {

testing::TestOutcome run_cross_section_viewer() {
    cross_section_viewer::init();
    auto s = cross_section_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cross_section_viewer::shutdown();
    return testing::ok();
}

const int _reg_cross_section_viewer = testing::register_test(
    "cross_section_viewer",
    "1104_cross_section_viewer: stub status check",
    &run_cross_section_viewer);

}
