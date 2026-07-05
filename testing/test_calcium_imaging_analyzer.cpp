#include "test_runner.hpp"
#include "../modules/1393_calcium_imaging_analyzer/calcium_imaging_analyzer.hpp"

namespace {

testing::TestOutcome run_calcium_imaging_analyzer() {
    calcium_imaging_analyzer::init();
    auto s = calcium_imaging_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    calcium_imaging_analyzer::shutdown();
    return testing::ok();
}

const int _reg_calcium_imaging_analyzer = testing::register_test(
    "calcium_imaging_analyzer",
    "1393_calcium_imaging_analyzer: stub status check",
    &run_calcium_imaging_analyzer);

}
