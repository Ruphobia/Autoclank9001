#include "test_runner.hpp"
#include "../modules/1952_hex_and_grid_mapper/hex_and_grid_mapper.hpp"

namespace {

testing::TestOutcome run_hex_and_grid_mapper() {
    hex_and_grid_mapper::init();
    auto s = hex_and_grid_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hex_and_grid_mapper::shutdown();
    return testing::ok();
}

const int _reg_hex_and_grid_mapper = testing::register_test(
    "hex_and_grid_mapper",
    "1952_hex_and_grid_mapper: stub status check",
    &run_hex_and_grid_mapper);

}
