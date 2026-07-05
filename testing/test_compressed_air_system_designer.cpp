#include "test_runner.hpp"
#include "../modules/1027_compressed_air_system_designer/compressed_air_system_designer.hpp"

namespace {

testing::TestOutcome run_compressed_air_system_designer() {
    compressed_air_system_designer::init();
    auto s = compressed_air_system_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    compressed_air_system_designer::shutdown();
    return testing::ok();
}

const int _reg_compressed_air_system_designer = testing::register_test(
    "compressed_air_system_designer",
    "1027_compressed_air_system_designer: stub status check",
    &run_compressed_air_system_designer);

}
