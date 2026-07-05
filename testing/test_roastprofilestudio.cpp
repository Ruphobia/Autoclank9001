#include "test_runner.hpp"
#include "../modules/1820_roastprofilestudio/roastprofilestudio.hpp"

namespace {

testing::TestOutcome run_roastprofilestudio() {
    roastprofilestudio::init();
    auto s = roastprofilestudio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    roastprofilestudio::shutdown();
    return testing::ok();
}

const int _reg_roastprofilestudio = testing::register_test(
    "roastprofilestudio",
    "1820_roastprofilestudio: stub status check",
    &run_roastprofilestudio);

}
