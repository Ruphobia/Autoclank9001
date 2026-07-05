#include "test_runner.hpp"
#include "../modules/798_unicode_inspector/unicode_inspector.hpp"

namespace {

testing::TestOutcome run_unicode_inspector() {
    unicode_inspector::init();
    auto s = unicode_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    unicode_inspector::shutdown();
    return testing::ok();
}

const int _reg_unicode_inspector = testing::register_test(
    "unicode_inspector",
    "798_unicode_inspector: stub status check",
    &run_unicode_inspector);

}
