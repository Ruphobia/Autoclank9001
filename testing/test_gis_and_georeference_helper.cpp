#include "test_runner.hpp"
#include "../modules/1126_gis_and_georeference_helper/gis_and_georeference_helper.hpp"

namespace {

testing::TestOutcome run_gis_and_georeference_helper() {
    gis_and_georeference_helper::init();
    auto s = gis_and_georeference_helper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gis_and_georeference_helper::shutdown();
    return testing::ok();
}

const int _reg_gis_and_georeference_helper = testing::register_test(
    "gis_and_georeference_helper",
    "1126_gis_and_georeference_helper: stub status check",
    &run_gis_and_georeference_helper);

}
