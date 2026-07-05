#include "test_runner.hpp"
#include "../modules/648_dataset_and_code_provenance_tracker/dataset_and_code_provenance_tracker.hpp"

namespace {

testing::TestOutcome run_dataset_and_code_provenance_tracker() {
    dataset_and_code_provenance_tracker::init();
    auto s = dataset_and_code_provenance_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dataset_and_code_provenance_tracker::shutdown();
    return testing::ok();
}

const int _reg_dataset_and_code_provenance_tracker = testing::register_test(
    "dataset_and_code_provenance_tracker",
    "648_dataset_and_code_provenance_tracker: stub status check",
    &run_dataset_and_code_provenance_tracker);

}
