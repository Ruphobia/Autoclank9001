#include "test_runner.hpp"
#include "../modules/723_lacuna_reconstructor/lacuna_reconstructor.hpp"

namespace {

testing::TestOutcome run_lacuna_reconstructor() {
    lacuna_reconstructor::init();
    auto s = lacuna_reconstructor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lacuna_reconstructor::shutdown();
    return testing::ok();
}

const int _reg_lacuna_reconstructor = testing::register_test(
    "lacuna_reconstructor",
    "723_lacuna_reconstructor: stub status check",
    &run_lacuna_reconstructor);

}
