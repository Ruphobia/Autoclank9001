#include "test_runner.hpp"
#include "../modules/634_glossary_and_index_builder/glossary_and_index_builder.hpp"

namespace {

testing::TestOutcome run_glossary_and_index_builder() {
    glossary_and_index_builder::init();
    auto s = glossary_and_index_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    glossary_and_index_builder::shutdown();
    return testing::ok();
}

const int _reg_glossary_and_index_builder = testing::register_test(
    "glossary_and_index_builder",
    "634_glossary_and_index_builder: stub status check",
    &run_glossary_and_index_builder);

}
