#include "test_runner.hpp"
#include "../modules/226_er_diagram_designer/er_diagram_designer.hpp"

namespace {

testing::TestOutcome run_er_diagram_designer() {
    er_diagram_designer::init();
    auto s = er_diagram_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    er_diagram_designer::shutdown();
    return testing::ok();
}

const int _reg_er_diagram_designer = testing::register_test(
    "er_diagram_designer",
    "226_er_diagram_designer: stub status check",
    &run_er_diagram_designer);

}
