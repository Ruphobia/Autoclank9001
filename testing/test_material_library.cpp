#include "test_runner.hpp"
#include "../modules/940_material_library/material_library.hpp"

namespace {

testing::TestOutcome run_material_library() {
    material_library::init();
    auto s = material_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    material_library::shutdown();
    return testing::ok();
}

const int _reg_material_library = testing::register_test(
    "material_library",
    "940_material_library: stub status check",
    &run_material_library);

}
