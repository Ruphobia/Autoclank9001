#include "test_runner.hpp"
#include "../modules/1322_multiple_sequence_alignment/multiple_sequence_alignment.hpp"

namespace {

testing::TestOutcome run_multiple_sequence_alignment() {
    multiple_sequence_alignment::init();
    auto s = multiple_sequence_alignment::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    multiple_sequence_alignment::shutdown();
    return testing::ok();
}

const int _reg_multiple_sequence_alignment = testing::register_test(
    "multiple_sequence_alignment",
    "1322_multiple_sequence_alignment: stub status check",
    &run_multiple_sequence_alignment);

}
