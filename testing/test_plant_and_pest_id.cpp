#include "test_runner.hpp"
#include "../modules/2003_plant_and_pest_id/plant_and_pest_id.hpp"

namespace {

testing::TestOutcome run_plant_and_pest_id() {
    plant_and_pest_id::init();
    auto s = plant_and_pest_id::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plant_and_pest_id::shutdown();
    return testing::ok();
}

const int _reg_plant_and_pest_id = testing::register_test(
    "plant_and_pest_id",
    "2003_plant_and_pest_id: stub status check",
    &run_plant_and_pest_id);

}
