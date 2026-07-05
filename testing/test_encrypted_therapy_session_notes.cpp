#include "test_runner.hpp"
#include "../modules/2178_encrypted_therapy_session_notes/encrypted_therapy_session_notes.hpp"

namespace {

testing::TestOutcome run_encrypted_therapy_session_notes() {
    encrypted_therapy_session_notes::init();
    auto s = encrypted_therapy_session_notes::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    encrypted_therapy_session_notes::shutdown();
    return testing::ok();
}

const int _reg_encrypted_therapy_session_notes = testing::register_test(
    "encrypted_therapy_session_notes",
    "2178_encrypted_therapy_session_notes: stub status check",
    &run_encrypted_therapy_session_notes);

}
