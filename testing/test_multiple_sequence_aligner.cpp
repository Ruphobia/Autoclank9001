#include "test_runner.hpp"
#include "../modules/1349_multiple_sequence_aligner/multiple_sequence_aligner.hpp"

namespace {

testing::TestOutcome run_multiple_sequence_aligner() {
    multiple_sequence_aligner::init();
    auto s = multiple_sequence_aligner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    multiple_sequence_aligner::shutdown();
    return testing::ok();
}

const int _reg_multiple_sequence_aligner = testing::register_test(
    "multiple_sequence_aligner",
    "1349_multiple_sequence_aligner: stub status check",
    &run_multiple_sequence_aligner);

}
