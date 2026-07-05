#include "test_runner.hpp"
#include "../modules/1789_mash_water_chemistry_designer/mash_water_chemistry_designer.hpp"

namespace {

testing::TestOutcome run_mash_water_chemistry_designer() {
    mash_water_chemistry_designer::init();
    auto s = mash_water_chemistry_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mash_water_chemistry_designer::shutdown();
    return testing::ok();
}

const int _reg_mash_water_chemistry_designer = testing::register_test(
    "mash_water_chemistry_designer",
    "1789_mash_water_chemistry_designer: stub status check",
    &run_mash_water_chemistry_designer);

}
