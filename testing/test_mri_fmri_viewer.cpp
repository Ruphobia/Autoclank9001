#include "test_runner.hpp"
#include "../modules/1391_mri_fmri_viewer/mri_fmri_viewer.hpp"

namespace {

testing::TestOutcome run_mri_fmri_viewer() {
    mri_fmri_viewer::init();
    auto s = mri_fmri_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mri_fmri_viewer::shutdown();
    return testing::ok();
}

const int _reg_mri_fmri_viewer = testing::register_test(
    "mri_fmri_viewer",
    "1391_mri_fmri_viewer: stub status check",
    &run_mri_fmri_viewer);

}
