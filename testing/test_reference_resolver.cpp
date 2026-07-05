#include "test_runner.hpp"
#include "../modules/628_reference_resolver/reference_resolver.hpp"

namespace {

testing::TestOutcome run_reference_resolver() {
    reference_resolver::init();
    auto s = reference_resolver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reference_resolver::shutdown();
    return testing::ok();
}

const int _reg_reference_resolver = testing::register_test(
    "reference_resolver",
    "628_reference_resolver: stub status check",
    &run_reference_resolver);

}
