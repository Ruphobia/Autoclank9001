#include "test_runner.hpp"
#include "../modules/1871_anatomy_reference_viewer/anatomy_reference_viewer.hpp"

namespace {

testing::TestOutcome run_anatomy_reference_viewer() {
    anatomy_reference_viewer::init();
    auto s = anatomy_reference_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    anatomy_reference_viewer::shutdown();
    return testing::ok();
}

const int _reg_anatomy_reference_viewer = testing::register_test(
    "anatomy_reference_viewer",
    "1871_anatomy_reference_viewer: stub status check",
    &run_anatomy_reference_viewer);

}
