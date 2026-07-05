#include "test_runner.hpp"
#include "../modules/397_rom_dumper_station/rom_dumper_station.hpp"

namespace {

testing::TestOutcome run_rom_dumper_station() {
    rom_dumper_station::init();
    auto s = rom_dumper_station::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rom_dumper_station::shutdown();
    return testing::ok();
}

const int _reg_rom_dumper_station = testing::register_test(
    "rom_dumper_station",
    "397_rom_dumper_station: stub status check",
    &run_rom_dumper_station);

}
