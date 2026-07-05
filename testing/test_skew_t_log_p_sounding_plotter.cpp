#include "test_runner.hpp"
#include "../modules/1457_skew_t_log_p_sounding_plotter/skew_t_log_p_sounding_plotter.hpp"

namespace {

testing::TestOutcome run_skew_t_log_p_sounding_plotter() {
    skew_t_log_p_sounding_plotter::init();
    auto s = skew_t_log_p_sounding_plotter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    skew_t_log_p_sounding_plotter::shutdown();
    return testing::ok();
}

const int _reg_skew_t_log_p_sounding_plotter = testing::register_test(
    "skew_t_log_p_sounding_plotter",
    "1457_skew_t_log_p_sounding_plotter: stub status check",
    &run_skew_t_log_p_sounding_plotter);

}
