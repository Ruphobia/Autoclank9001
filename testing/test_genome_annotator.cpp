#include "test_runner.hpp"
#include "../modules/1352_genome_annotator/genome_annotator.hpp"

namespace {

testing::TestOutcome run_genome_annotator() {
    genome_annotator::init();
    auto s = genome_annotator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    genome_annotator::shutdown();
    return testing::ok();
}

const int _reg_genome_annotator = testing::register_test(
    "genome_annotator",
    "1352_genome_annotator: stub status check",
    &run_genome_annotator);

}
