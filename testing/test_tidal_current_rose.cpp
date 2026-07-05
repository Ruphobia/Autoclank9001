#include "test_runner.hpp"
#include "../modules/1540_tidal_current_rose/tidal_current_rose.hpp"

namespace {

testing::TestOutcome run_tidal_current_rose() {
    tidal_current_rose::init();
    auto s = tidal_current_rose::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tidal_current_rose::shutdown();
    return testing::ok();
}

const int _reg_tidal_current_rose = testing::register_test(
    "tidal_current_rose",
    "1540_tidal_current_rose: stub status check",
    &run_tidal_current_rose);

}
