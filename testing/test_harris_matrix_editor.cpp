#include "test_runner.hpp"
#include "../modules/1521_harris_matrix_editor/harris_matrix_editor.hpp"

namespace {

testing::TestOutcome run_harris_matrix_editor() {
    harris_matrix_editor::init();
    auto s = harris_matrix_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    harris_matrix_editor::shutdown();
    return testing::ok();
}

const int _reg_harris_matrix_editor = testing::register_test(
    "harris_matrix_editor",
    "1521_harris_matrix_editor: stub status check",
    &run_harris_matrix_editor);

}
