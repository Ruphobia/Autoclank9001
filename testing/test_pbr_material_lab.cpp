#include "test_runner.hpp"
#include "../modules/1692_pbr_material_lab/pbr_material_lab.hpp"

namespace {

testing::TestOutcome run_pbr_material_lab() {
    pbr_material_lab::init();
    auto s = pbr_material_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pbr_material_lab::shutdown();
    return testing::ok();
}

const int _reg_pbr_material_lab = testing::register_test(
    "pbr_material_lab",
    "1692_pbr_material_lab: stub status check",
    &run_pbr_material_lab);

}
