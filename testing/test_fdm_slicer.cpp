#include "test_runner.hpp"
#include "../modules/964_fdm_slicer/fdm_slicer.hpp"

namespace {

testing::TestOutcome run_fdm_slicer() {
    fdm_slicer::init();
    auto s = fdm_slicer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fdm_slicer::shutdown();
    return testing::ok();
}

const int _reg_fdm_slicer = testing::register_test(
    "fdm_slicer",
    "964_fdm_slicer: stub status check",
    &run_fdm_slicer);

}
