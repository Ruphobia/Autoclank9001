#include "test_runner.hpp"
#include "../modules/125_layout_inspector/layout_inspector.hpp"

namespace {

testing::TestOutcome run_layout_inspector() {
    layout_inspector::init();
    auto s = layout_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    layout_inspector::shutdown();
    return testing::ok();
}

const int _reg_layout_inspector = testing::register_test(
    "layout_inspector",
    "125_layout_inspector: stub status check",
    &run_layout_inspector);

}
