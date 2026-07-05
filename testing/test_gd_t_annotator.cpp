#include "test_runner.hpp"
#include "../modules/935_gd_t_annotator/gd_t_annotator.hpp"

namespace {

testing::TestOutcome run_gd_t_annotator() {
    gd_t_annotator::init();
    auto s = gd_t_annotator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gd_t_annotator::shutdown();
    return testing::ok();
}

const int _reg_gd_t_annotator = testing::register_test(
    "gd_t_annotator",
    "935_gd_t_annotator: stub status check",
    &run_gd_t_annotator);

}
