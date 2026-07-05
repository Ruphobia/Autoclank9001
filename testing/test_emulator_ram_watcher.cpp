#include "test_runner.hpp"
#include "../modules/1976_emulator_ram_watcher/emulator_ram_watcher.hpp"

namespace {

testing::TestOutcome run_emulator_ram_watcher() {
    emulator_ram_watcher::init();
    auto s = emulator_ram_watcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    emulator_ram_watcher::shutdown();
    return testing::ok();
}

const int _reg_emulator_ram_watcher = testing::register_test(
    "emulator_ram_watcher",
    "1976_emulator_ram_watcher: stub status check",
    &run_emulator_ram_watcher);

}
