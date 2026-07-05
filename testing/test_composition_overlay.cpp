#include "test_runner.hpp"
#include "../modules/1680_composition_overlay/composition_overlay.hpp"

namespace {

testing::TestOutcome run_composition_overlay() {
    composition_overlay::init();
    auto s = composition_overlay::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    composition_overlay::shutdown();
    return testing::ok();
}

const int _reg_composition_overlay = testing::register_test(
    "composition_overlay",
    "1680_composition_overlay: stub status check",
    &run_composition_overlay);

}
