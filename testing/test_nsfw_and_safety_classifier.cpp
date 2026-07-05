#include "test_runner.hpp"
#include "../modules/1661_nsfw_and_safety_classifier/nsfw_and_safety_classifier.hpp"

namespace {

testing::TestOutcome run_nsfw_and_safety_classifier() {
    nsfw_and_safety_classifier::init();
    auto s = nsfw_and_safety_classifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nsfw_and_safety_classifier::shutdown();
    return testing::ok();
}

const int _reg_nsfw_and_safety_classifier = testing::register_test(
    "nsfw_and_safety_classifier",
    "1661_nsfw_and_safety_classifier: stub status check",
    &run_nsfw_and_safety_classifier);

}
