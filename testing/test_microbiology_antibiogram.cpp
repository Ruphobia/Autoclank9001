#include "test_runner.hpp"
#include "../modules/1416_microbiology_antibiogram/microbiology_antibiogram.hpp"

namespace {

testing::TestOutcome run_microbiology_antibiogram() {
    microbiology_antibiogram::init();
    auto s = microbiology_antibiogram::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    microbiology_antibiogram::shutdown();
    return testing::ok();
}

const int _reg_microbiology_antibiogram = testing::register_test(
    "microbiology_antibiogram",
    "1416_microbiology_antibiogram: stub status check",
    &run_microbiology_antibiogram);

}
