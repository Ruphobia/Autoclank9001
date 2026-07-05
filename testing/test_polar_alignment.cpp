#include "test_runner.hpp"
#include "../modules/1483_polar_alignment/polar_alignment.hpp"

namespace {

testing::TestOutcome run_polar_alignment() {
    polar_alignment::init();
    auto s = polar_alignment::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    polar_alignment::shutdown();
    return testing::ok();
}

const int _reg_polar_alignment = testing::register_test(
    "polar_alignment",
    "1483_polar_alignment: stub status check",
    &run_polar_alignment);

}
