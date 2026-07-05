#include "test_runner.hpp"
#include "../modules/1386_alignment_viewer/alignment_viewer.hpp"

namespace {

testing::TestOutcome run_alignment_viewer() {
    alignment_viewer::init();
    auto s = alignment_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    alignment_viewer::shutdown();
    return testing::ok();
}

const int _reg_alignment_viewer = testing::register_test(
    "alignment_viewer",
    "1386_alignment_viewer: stub status check",
    &run_alignment_viewer);

}
