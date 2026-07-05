#include "test_runner.hpp"
#include "../modules/1270_slicer_for_additive_manufacturing/slicer_for_additive_manufacturing.hpp"

namespace {

testing::TestOutcome run_slicer_for_additive_manufacturing() {
    slicer_for_additive_manufacturing::init();
    auto s = slicer_for_additive_manufacturing::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    slicer_for_additive_manufacturing::shutdown();
    return testing::ok();
}

const int _reg_slicer_for_additive_manufacturing = testing::register_test(
    "slicer_for_additive_manufacturing",
    "1270_slicer_for_additive_manufacturing: stub status check",
    &run_slicer_for_additive_manufacturing);

}
