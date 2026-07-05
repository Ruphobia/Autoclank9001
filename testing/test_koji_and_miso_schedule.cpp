#include "test_runner.hpp"
#include "../modules/1805_koji_and_miso_schedule/koji_and_miso_schedule.hpp"

namespace {

testing::TestOutcome run_koji_and_miso_schedule() {
    koji_and_miso_schedule::init();
    auto s = koji_and_miso_schedule::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    koji_and_miso_schedule::shutdown();
    return testing::ok();
}

const int _reg_koji_and_miso_schedule = testing::register_test(
    "koji_and_miso_schedule",
    "1805_koji_and_miso_schedule: stub status check",
    &run_koji_and_miso_schedule);

}
