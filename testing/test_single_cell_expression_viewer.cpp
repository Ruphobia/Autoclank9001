#include "test_runner.hpp"
#include "../modules/1388_single_cell_expression_viewer/single_cell_expression_viewer.hpp"

namespace {

testing::TestOutcome run_single_cell_expression_viewer() {
    single_cell_expression_viewer::init();
    auto s = single_cell_expression_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    single_cell_expression_viewer::shutdown();
    return testing::ok();
}

const int _reg_single_cell_expression_viewer = testing::register_test(
    "single_cell_expression_viewer",
    "1388_single_cell_expression_viewer: stub status check",
    &run_single_cell_expression_viewer);

}
