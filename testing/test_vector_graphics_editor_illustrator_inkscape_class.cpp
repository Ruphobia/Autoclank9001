#include "test_runner.hpp"
#include "../modules/1621_vector_graphics_editor_illustrator_inkscape_class/vector_graphics_editor_illustrator_inkscape_class.hpp"

namespace {

testing::TestOutcome run_vector_graphics_editor_illustrator_inkscape_class() {
    vector_graphics_editor_illustrator_inkscape_class::init();
    auto s = vector_graphics_editor_illustrator_inkscape_class::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vector_graphics_editor_illustrator_inkscape_class::shutdown();
    return testing::ok();
}

const int _reg_vector_graphics_editor_illustrator_inkscape_class = testing::register_test(
    "vector_graphics_editor_illustrator_inkscape_class",
    "1621_vector_graphics_editor_illustrator_inkscape_class: stub status check",
    &run_vector_graphics_editor_illustrator_inkscape_class);

}
