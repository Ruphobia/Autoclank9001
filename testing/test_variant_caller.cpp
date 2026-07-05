#include "test_runner.hpp"
#include "../modules/1354_variant_caller/variant_caller.hpp"

namespace {

testing::TestOutcome run_variant_caller() {
    variant_caller::init();
    auto s = variant_caller::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    variant_caller::shutdown();
    return testing::ok();
}

const int _reg_variant_caller = testing::register_test(
    "variant_caller",
    "1354_variant_caller: stub status check",
    &run_variant_caller);

}
