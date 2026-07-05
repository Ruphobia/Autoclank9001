#include "test_runner.hpp"
#include "../modules/1156_lifting_line_analyzer/lifting_line_analyzer.hpp"

namespace {

testing::TestOutcome run_lifting_line_analyzer() {
    lifting_line_analyzer::init();
    auto s = lifting_line_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lifting_line_analyzer::shutdown();
    return testing::ok();
}

const int _reg_lifting_line_analyzer = testing::register_test(
    "lifting_line_analyzer",
    "1156_lifting_line_analyzer: stub status check",
    &run_lifting_line_analyzer);

}
