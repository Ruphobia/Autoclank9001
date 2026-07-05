#include "test_runner.hpp"
#include "../modules/1324_genome_browser/genome_browser.hpp"

namespace {

testing::TestOutcome run_genome_browser() {
    genome_browser::init();
    auto s = genome_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    genome_browser::shutdown();
    return testing::ok();
}

const int _reg_genome_browser = testing::register_test(
    "genome_browser",
    "1324_genome_browser: stub status check",
    &run_genome_browser);

}
