#include "test_runner.hpp"
#include "../modules/232_data_grid_editor/data_grid_editor.hpp"

namespace {

testing::TestOutcome run_data_grid_editor() {
    data_grid_editor::init();
    auto s = data_grid_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    data_grid_editor::shutdown();
    return testing::ok();
}

const int _reg_data_grid_editor = testing::register_test(
    "data_grid_editor",
    "232_data_grid_editor: stub status check",
    &run_data_grid_editor);

}
