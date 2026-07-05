#include "test_runner.hpp"
#include "../modules/874_zernike_wavefront_analyzer/zernike_wavefront_analyzer.hpp"

namespace {

testing::TestOutcome run_zernike_wavefront_analyzer() {
    zernike_wavefront_analyzer::init();
    auto s = zernike_wavefront_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    zernike_wavefront_analyzer::shutdown();
    return testing::ok();
}

const int _reg_zernike_wavefront_analyzer = testing::register_test(
    "zernike_wavefront_analyzer",
    "874_zernike_wavefront_analyzer: stub status check",
    &run_zernike_wavefront_analyzer);

}
