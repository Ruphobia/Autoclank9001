#include "test_runner.hpp"
#include "../modules/1571_bioacoustic_classifier/bioacoustic_classifier.hpp"

namespace {

testing::TestOutcome run_bioacoustic_classifier() {
    bioacoustic_classifier::init();
    auto s = bioacoustic_classifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bioacoustic_classifier::shutdown();
    return testing::ok();
}

const int _reg_bioacoustic_classifier = testing::register_test(
    "bioacoustic_classifier",
    "1571_bioacoustic_classifier: stub status check",
    &run_bioacoustic_classifier);

}
