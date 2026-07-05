#include "test_runner.hpp"
#include "../modules/1607_companion_matrix/companion_matrix.hpp"

namespace {

testing::TestOutcome run_companion_matrix() {
    companion_matrix::init();
    auto s = companion_matrix::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    companion_matrix::shutdown();
    return testing::ok();
}

const int _reg_companion_matrix = testing::register_test(
    "companion_matrix",
    "1607_companion_matrix: stub status check",
    &run_companion_matrix);

}
