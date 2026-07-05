#include "test_runner.hpp"
#include "../modules/2143_endogamy_and_pedigree_collapse_detector/endogamy_and_pedigree_collapse_detector.hpp"

namespace {

testing::TestOutcome run_endogamy_and_pedigree_collapse_detector() {
    endogamy_and_pedigree_collapse_detector::init();
    auto s = endogamy_and_pedigree_collapse_detector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    endogamy_and_pedigree_collapse_detector::shutdown();
    return testing::ok();
}

const int _reg_endogamy_and_pedigree_collapse_detector = testing::register_test(
    "endogamy_and_pedigree_collapse_detector",
    "2143_endogamy_and_pedigree_collapse_detector: stub status check",
    &run_endogamy_and_pedigree_collapse_detector);

}
