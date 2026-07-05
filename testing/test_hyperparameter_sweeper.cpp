#include "test_runner.hpp"
#include "../modules/664_hyperparameter_sweeper/hyperparameter_sweeper.hpp"

namespace {

testing::TestOutcome run_hyperparameter_sweeper() {
    hyperparameter_sweeper::init();
    auto s = hyperparameter_sweeper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hyperparameter_sweeper::shutdown();
    return testing::ok();
}

const int _reg_hyperparameter_sweeper = testing::register_test(
    "hyperparameter_sweeper",
    "664_hyperparameter_sweeper: stub status check",
    &run_hyperparameter_sweeper);

}
