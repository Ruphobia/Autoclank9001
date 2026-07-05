#include "test_runner.hpp"
#include "../modules/1672_lighting_diagram_designer/lighting_diagram_designer.hpp"

namespace {

testing::TestOutcome run_lighting_diagram_designer() {
    lighting_diagram_designer::init();
    auto s = lighting_diagram_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lighting_diagram_designer::shutdown();
    return testing::ok();
}

const int _reg_lighting_diagram_designer = testing::register_test(
    "lighting_diagram_designer",
    "1672_lighting_diagram_designer: stub status check",
    &run_lighting_diagram_designer);

}
