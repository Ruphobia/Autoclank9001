// Smoke test for 060_footnote_manager.

#include "test_runner.hpp"
#include "../modules/060_footnote_manager/footnote_manager.hpp"

namespace {

testing::TestOutcome run() {
    footnote_manager::init();

    std::string doc =
        "Body text refers to [^a] and then [^b]. Then [^a] again.\n"
        "\n"
        "[^a]: First note text.\n"
        "[^b]: Second note text.\n";

    auto fns = footnote_manager::extract(doc);
    if (fns.size() != 2) return testing::fail("extract count != 2");
    auto find = [&](const std::string & id) -> const footnote_manager::Footnote * {
        for (const auto & f : fns) if (f.id == id) return &f;
        return nullptr;
    };
    auto fa = find("a"), fb = find("b");
    if (!fa || !fb) return testing::fail("extract did not surface both ids");
    if (fa->ref_positions.size() != 2)
        return testing::fail("a should have 2 ref positions");
    if (fa->text.find("First") == std::string::npos)
        return testing::fail("a definition text missing");

    // Renumber: a -> 1 (first referenced), b -> 2.
    auto renum = footnote_manager::renumber(doc);
    if (renum.find("[^1]") == std::string::npos ||
        renum.find("[^2]") == std::string::npos)
        return testing::fail("renumber did not produce sequential ids");
    if (renum.find("[^a]") != std::string::npos)
        return testing::fail("renumber left old id 'a' in body");
    if (renum.find("[^1]: First note text.") == std::string::npos)
        return testing::fail("renumber did not rewrite the [^1] definition");
    if (renum.find("[^2]: Second note text.") == std::string::npos)
        return testing::fail("renumber did not rewrite the [^2] definition");

    // ibid.: two consecutive [^a] references collapse the second.
    std::string ibid_doc = "First cite [^a] and right after [^a] and then [^b] and [^b] and [^a].";
    std::string ib = footnote_manager::apply_ibid(ibid_doc);
    // After: "First cite [^a] and right after ibid. and then [^b] and ibid. and [^a]."
    if (ib.find("ibid.") == std::string::npos)
        return testing::fail("apply_ibid produced no ibid");
    // Count "ibid." occurrences.
    size_t count_ibid = 0;
    for (size_t pos = 0; (pos = ib.find("ibid.", pos)) != std::string::npos; pos += 5)
        ++count_ibid;
    if (count_ibid != 2)
        return testing::fail("expected 2 ibid., got " + std::to_string(count_ibid));
    // The trailing [^a] is NOT after another [^a] (after [^b]), so it stays.
    if (ib.find("and [^a].") == std::string::npos)
        return testing::fail("trailing distinct [^a] was wrongly rewritten");

    footnote_manager::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "footnote_manager",
    "060_footnote_manager: extract + renumber + ibid rewrite",
    &run);

}
