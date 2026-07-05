#include "test_runner.hpp"
#include "../modules/2111_engine_and_systems_maintenance_log/engine_and_systems_maintenance_log.hpp"

namespace {

testing::TestOutcome run_engine_and_systems_maintenance_log() {
    engine_and_systems_maintenance_log::init();
    auto s = engine_and_systems_maintenance_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    engine_and_systems_maintenance_log::shutdown();
    return testing::ok();
}

const int _reg_engine_and_systems_maintenance_log = testing::register_test(
    "engine_and_systems_maintenance_log",
    "2111_engine_and_systems_maintenance_log: stub status check",
    &run_engine_and_systems_maintenance_log);

}
