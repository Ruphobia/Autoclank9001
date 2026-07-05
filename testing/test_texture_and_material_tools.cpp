#include "test_runner.hpp"
#include "../modules/1665_texture_and_material_tools/texture_and_material_tools.hpp"

namespace {

testing::TestOutcome run_texture_and_material_tools() {
    texture_and_material_tools::init();
    auto s = texture_and_material_tools::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    texture_and_material_tools::shutdown();
    return testing::ok();
}

const int _reg_texture_and_material_tools = testing::register_test(
    "texture_and_material_tools",
    "1665_texture_and_material_tools: stub status check",
    &run_texture_and_material_tools);

}
