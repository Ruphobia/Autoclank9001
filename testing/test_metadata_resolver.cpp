#include "test_runner.hpp"
#include "../modules/078_metadata_resolver/metadata_resolver.hpp"

namespace {

testing::TestOutcome run_metadata_resolver() {
    metadata_resolver::init();
    auto s = metadata_resolver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    metadata_resolver::shutdown();
    return testing::ok();
}

const int _reg_metadata_resolver = testing::register_test(
    "metadata_resolver",
    "078_metadata_resolver: stub status check",
    &run_metadata_resolver);

}
