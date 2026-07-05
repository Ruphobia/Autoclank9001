#include "test_runner.hpp"
#include "../modules/1019_pipe_network_solver/pipe_network_solver.hpp"

namespace {

testing::TestOutcome run_pipe_network_solver() {
    pipe_network_solver::init();
    auto s = pipe_network_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pipe_network_solver::shutdown();
    return testing::ok();
}

const int _reg_pipe_network_solver = testing::register_test(
    "pipe_network_solver",
    "1019_pipe_network_solver: stub status check",
    &run_pipe_network_solver);

}
