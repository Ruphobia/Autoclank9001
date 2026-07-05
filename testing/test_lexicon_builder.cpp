#include "test_runner.hpp"
#include "../modules/807_lexicon_builder/lexicon_builder.hpp"

namespace {

testing::TestOutcome run_lexicon_builder() {
    lexicon_builder::init();
    auto s = lexicon_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lexicon_builder::shutdown();
    return testing::ok();
}

const int _reg_lexicon_builder = testing::register_test(
    "lexicon_builder",
    "807_lexicon_builder: stub status check",
    &run_lexicon_builder);

}
