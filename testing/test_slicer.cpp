#include "test_runner.hpp"
#include "../modules/928_slicer/slicer.hpp"

namespace {

testing::TestOutcome run_slicer() {
    slicer::init();
    auto s = slicer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    slicer::shutdown();
    return testing::ok();
}

const int _reg_slicer = testing::register_test(
    "slicer",
    "928_slicer: stub status check",
    &run_slicer);

}
