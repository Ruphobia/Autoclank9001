#include "test_runner.hpp"
#include "../modules/808_morphology_sketcher/morphology_sketcher.hpp"

namespace {

testing::TestOutcome run_morphology_sketcher() {
    morphology_sketcher::init();
    auto s = morphology_sketcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    morphology_sketcher::shutdown();
    return testing::ok();
}

const int _reg_morphology_sketcher = testing::register_test(
    "morphology_sketcher",
    "808_morphology_sketcher: stub status check",
    &run_morphology_sketcher);

}
