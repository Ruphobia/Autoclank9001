#include "test_runner.hpp"
#include "../modules/1042_pipe_sizer/pipe_sizer.hpp"

namespace {

testing::TestOutcome run_pipe_sizer() {
    pipe_sizer::init();
    auto s = pipe_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pipe_sizer::shutdown();
    return testing::ok();
}

const int _reg_pipe_sizer = testing::register_test(
    "pipe_sizer",
    "1042_pipe_sizer: stub status check",
    &run_pipe_sizer);

}
