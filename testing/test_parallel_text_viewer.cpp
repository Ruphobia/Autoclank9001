#include "test_runner.hpp"
#include "../modules/757_parallel_text_viewer/parallel_text_viewer.hpp"

namespace {

testing::TestOutcome run_parallel_text_viewer() {
    parallel_text_viewer::init();
    auto s = parallel_text_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    parallel_text_viewer::shutdown();
    return testing::ok();
}

const int _reg_parallel_text_viewer = testing::register_test(
    "parallel_text_viewer",
    "757_parallel_text_viewer: stub status check",
    &run_parallel_text_viewer);

}
