#include "test_runner.hpp"
#include "../modules/650_annotation_and_margin_notes_layer/annotation_and_margin_notes_layer.hpp"

namespace {

testing::TestOutcome run_annotation_and_margin_notes_layer() {
    annotation_and_margin_notes_layer::init();
    auto s = annotation_and_margin_notes_layer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    annotation_and_margin_notes_layer::shutdown();
    return testing::ok();
}

const int _reg_annotation_and_margin_notes_layer = testing::register_test(
    "annotation_and_margin_notes_layer",
    "650_annotation_and_margin_notes_layer: stub status check",
    &run_annotation_and_margin_notes_layer);

}
