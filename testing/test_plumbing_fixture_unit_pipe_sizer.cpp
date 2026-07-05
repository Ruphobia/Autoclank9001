#include "test_runner.hpp"
#include "../modules/2021_plumbing_fixture_unit_pipe_sizer/plumbing_fixture_unit_pipe_sizer.hpp"

namespace {

testing::TestOutcome run_plumbing_fixture_unit_pipe_sizer() {
    plumbing_fixture_unit_pipe_sizer::init();
    auto s = plumbing_fixture_unit_pipe_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plumbing_fixture_unit_pipe_sizer::shutdown();
    return testing::ok();
}

const int _reg_plumbing_fixture_unit_pipe_sizer = testing::register_test(
    "plumbing_fixture_unit_pipe_sizer",
    "2021_plumbing_fixture_unit_pipe_sizer: stub status check",
    &run_plumbing_fixture_unit_pipe_sizer);

}
