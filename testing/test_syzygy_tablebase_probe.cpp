#include "test_runner.hpp"
#include "../modules/1967_syzygy_tablebase_probe/syzygy_tablebase_probe.hpp"

namespace {

testing::TestOutcome run_syzygy_tablebase_probe() {
    syzygy_tablebase_probe::init();
    auto s = syzygy_tablebase_probe::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    syzygy_tablebase_probe::shutdown();
    return testing::ok();
}

const int _reg_syzygy_tablebase_probe = testing::register_test(
    "syzygy_tablebase_probe",
    "1967_syzygy_tablebase_probe: stub status check",
    &run_syzygy_tablebase_probe);

}
