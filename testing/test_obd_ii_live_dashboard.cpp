#include "test_runner.hpp"
#include "../modules/1226_obd_ii_live_dashboard/obd_ii_live_dashboard.hpp"

namespace {

testing::TestOutcome run_obd_ii_live_dashboard() {
    obd_ii_live_dashboard::init();
    auto s = obd_ii_live_dashboard::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    obd_ii_live_dashboard::shutdown();
    return testing::ok();
}

const int _reg_obd_ii_live_dashboard = testing::register_test(
    "obd_ii_live_dashboard",
    "1226_obd_ii_live_dashboard: stub status check",
    &run_obd_ii_live_dashboard);

}
