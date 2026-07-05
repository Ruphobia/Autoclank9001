#include "test_runner.hpp"
#include "../modules/802_sequence_alignment_lab/sequence_alignment_lab.hpp"

namespace {

testing::TestOutcome run_sequence_alignment_lab() {
    sequence_alignment_lab::init();
    auto s = sequence_alignment_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sequence_alignment_lab::shutdown();
    return testing::ok();
}

const int _reg_sequence_alignment_lab = testing::register_test(
    "sequence_alignment_lab",
    "802_sequence_alignment_lab: stub status check",
    &run_sequence_alignment_lab);

}
