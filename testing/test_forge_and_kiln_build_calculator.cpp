#include "test_runner.hpp"
#include "../modules/1859_forge_and_kiln_build_calculator/forge_and_kiln_build_calculator.hpp"

namespace {

testing::TestOutcome run_forge_and_kiln_build_calculator() {
    forge_and_kiln_build_calculator::init();
    auto s = forge_and_kiln_build_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    forge_and_kiln_build_calculator::shutdown();
    return testing::ok();
}

const int _reg_forge_and_kiln_build_calculator = testing::register_test(
    "forge_and_kiln_build_calculator",
    "1859_forge_and_kiln_build_calculator: stub status check",
    &run_forge_and_kiln_build_calculator);

}
