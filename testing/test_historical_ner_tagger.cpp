#include "test_runner.hpp"
#include "../modules/696_historical_ner_tagger/historical_ner_tagger.hpp"

namespace {

testing::TestOutcome run_historical_ner_tagger() {
    historical_ner_tagger::init();
    auto s = historical_ner_tagger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    historical_ner_tagger::shutdown();
    return testing::ok();
}

const int _reg_historical_ner_tagger = testing::register_test(
    "historical_ner_tagger",
    "696_historical_ner_tagger: stub status check",
    &run_historical_ner_tagger);

}
