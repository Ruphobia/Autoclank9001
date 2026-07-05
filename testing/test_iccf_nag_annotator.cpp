#include "test_runner.hpp"
#include "../modules/1981_iccf_nag_annotator/iccf_nag_annotator.hpp"

namespace {

testing::TestOutcome run_iccf_nag_annotator() {
    iccf_nag_annotator::init();
    auto s = iccf_nag_annotator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    iccf_nag_annotator::shutdown();
    return testing::ok();
}

const int _reg_iccf_nag_annotator = testing::register_test(
    "iccf_nag_annotator",
    "1981_iccf_nag_annotator: stub status check",
    &run_iccf_nag_annotator);

}
