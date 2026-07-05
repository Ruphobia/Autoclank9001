#include "test_runner.hpp"
#include "../modules/1461_goes_abi_composer/goes_abi_composer.hpp"

namespace {

testing::TestOutcome run_goes_abi_composer() {
    goes_abi_composer::init();
    auto s = goes_abi_composer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    goes_abi_composer::shutdown();
    return testing::ok();
}

const int _reg_goes_abi_composer = testing::register_test(
    "goes_abi_composer",
    "1461_goes_abi_composer: stub status check",
    &run_goes_abi_composer);

}
