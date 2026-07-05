#include "test_runner.hpp"
#include "../modules/677_rag_builder/rag_builder.hpp"

namespace {

testing::TestOutcome run_rag_builder() {
    rag_builder::init();
    auto s = rag_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rag_builder::shutdown();
    return testing::ok();
}

const int _reg_rag_builder = testing::register_test(
    "rag_builder",
    "677_rag_builder: stub status check",
    &run_rag_builder);

}
