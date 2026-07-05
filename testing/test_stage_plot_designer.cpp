#include "test_runner.hpp"
#include "../modules/1935_stage_plot_designer/stage_plot_designer.hpp"

namespace {

testing::TestOutcome run_stage_plot_designer() {
    stage_plot_designer::init();
    auto s = stage_plot_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stage_plot_designer::shutdown();
    return testing::ok();
}

const int _reg_stage_plot_designer = testing::register_test(
    "stage_plot_designer",
    "1935_stage_plot_designer: stub status check",
    &run_stage_plot_designer);

}
