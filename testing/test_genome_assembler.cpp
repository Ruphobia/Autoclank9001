#include "test_runner.hpp"
#include "../modules/1350_genome_assembler/genome_assembler.hpp"

namespace {

testing::TestOutcome run_genome_assembler() {
    genome_assembler::init();
    auto s = genome_assembler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    genome_assembler::shutdown();
    return testing::ok();
}

const int _reg_genome_assembler = testing::register_test(
    "genome_assembler",
    "1350_genome_assembler: stub status check",
    &run_genome_assembler);

}
