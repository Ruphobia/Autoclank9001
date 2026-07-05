#include "test_runner.hpp"
#include "../modules/994_corrosion_compatibility_matrix/corrosion_compatibility_matrix.hpp"

namespace {

testing::TestOutcome run_corrosion_compatibility_matrix() {
    corrosion_compatibility_matrix::init();
    auto s = corrosion_compatibility_matrix::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    corrosion_compatibility_matrix::shutdown();
    return testing::ok();
}

const int _reg_corrosion_compatibility_matrix = testing::register_test(
    "corrosion_compatibility_matrix",
    "994_corrosion_compatibility_matrix: stub status check",
    &run_corrosion_compatibility_matrix);

}
