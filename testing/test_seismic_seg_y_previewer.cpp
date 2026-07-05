#include "test_runner.hpp"
#include "../modules/1125_seismic_seg_y_previewer/seismic_seg_y_previewer.hpp"

namespace {

testing::TestOutcome run_seismic_seg_y_previewer() {
    seismic_seg_y_previewer::init();
    auto s = seismic_seg_y_previewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    seismic_seg_y_previewer::shutdown();
    return testing::ok();
}

const int _reg_seismic_seg_y_previewer = testing::register_test(
    "seismic_seg_y_previewer",
    "1125_seismic_seg_y_previewer: stub status check",
    &run_seismic_seg_y_previewer);

}
