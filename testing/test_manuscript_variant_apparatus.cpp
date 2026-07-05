#include "test_runner.hpp"
#include "../modules/741_manuscript_variant_apparatus/manuscript_variant_apparatus.hpp"

namespace {

testing::TestOutcome run_manuscript_variant_apparatus() {
    manuscript_variant_apparatus::init();
    auto s = manuscript_variant_apparatus::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    manuscript_variant_apparatus::shutdown();
    return testing::ok();
}

const int _reg_manuscript_variant_apparatus = testing::register_test(
    "manuscript_variant_apparatus",
    "741_manuscript_variant_apparatus: stub status check",
    &run_manuscript_variant_apparatus);

}
