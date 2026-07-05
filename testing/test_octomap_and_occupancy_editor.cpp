#include "test_runner.hpp"
#include "../modules/1266_octomap_and_occupancy_editor/octomap_and_occupancy_editor.hpp"

namespace {

testing::TestOutcome run_octomap_and_occupancy_editor() {
    octomap_and_occupancy_editor::init();
    auto s = octomap_and_occupancy_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    octomap_and_occupancy_editor::shutdown();
    return testing::ok();
}

const int _reg_octomap_and_occupancy_editor = testing::register_test(
    "octomap_and_occupancy_editor",
    "1266_octomap_and_occupancy_editor: stub status check",
    &run_octomap_and_occupancy_editor);

}
