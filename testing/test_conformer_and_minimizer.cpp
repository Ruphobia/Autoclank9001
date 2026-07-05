#include "test_runner.hpp"
#include "../modules/1314_conformer_and_minimizer/conformer_and_minimizer.hpp"

namespace {

testing::TestOutcome run_conformer_and_minimizer() {
    conformer_and_minimizer::init();
    auto s = conformer_and_minimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    conformer_and_minimizer::shutdown();
    return testing::ok();
}

const int _reg_conformer_and_minimizer = testing::register_test(
    "conformer_and_minimizer",
    "1314_conformer_and_minimizer: stub status check",
    &run_conformer_and_minimizer);

}
