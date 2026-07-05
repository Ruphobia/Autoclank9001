#include "test_runner.hpp"
#include "../modules/2002_companion_planting_matrix/companion_planting_matrix.hpp"

namespace {

testing::TestOutcome run_companion_planting_matrix() {
    companion_planting_matrix::init();
    auto s = companion_planting_matrix::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    companion_planting_matrix::shutdown();
    return testing::ok();
}

const int _reg_companion_planting_matrix = testing::register_test(
    "companion_planting_matrix",
    "2002_companion_planting_matrix: stub status check",
    &run_companion_planting_matrix);

}
