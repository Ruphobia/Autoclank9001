#include "test_runner.hpp"
#include "../modules/341_algorithm_scaffolds/algorithm_scaffolds.hpp"

namespace {

testing::TestOutcome run_algorithm_scaffolds() {
    algorithm_scaffolds::init();
    auto s = algorithm_scaffolds::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    algorithm_scaffolds::shutdown();
    return testing::ok();
}

const int _reg_algorithm_scaffolds = testing::register_test(
    "algorithm_scaffolds",
    "341_algorithm_scaffolds: stub status check",
    &run_algorithm_scaffolds);

}
