#include "test_runner.hpp"
#include "../modules/1542_ctd_profile_plotter/ctd_profile_plotter.hpp"

namespace {

testing::TestOutcome run_ctd_profile_plotter() {
    ctd_profile_plotter::init();
    auto s = ctd_profile_plotter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ctd_profile_plotter::shutdown();
    return testing::ok();
}

const int _reg_ctd_profile_plotter = testing::register_test(
    "ctd_profile_plotter",
    "1542_ctd_profile_plotter: stub status check",
    &run_ctd_profile_plotter);

}
