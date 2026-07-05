#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// IP folder maintainer: dated invention disclosures, prior-art search
// results, claim drafts, and a defensive-publication log under
// <project>/.tool/ip/. Each disclosure lives in its own markdown file
// (<id>.md), so the user can hand-edit / commit / hash / attach
// supporting artifacts. Auto-detection of novel-claim-shaped content
// in the chat is wired separately; this module is the persistence /
// reading layer only.
namespace ip_folder {

struct Disclosure {
    std::string              id;
    std::string              title;
    std::string              body;            // raw markdown
    std::vector<std::string> claims;          // currently recorded
    std::vector<std::string> prior_art;       // citations dropped in so far
    std::vector<std::string> supporting;      // file paths or commit shas
    std::string              status;          // "draft", "filed", "defensive-published", ...
};

void init();
void shutdown();

bool exists(std::string_view id, std::string_view project_root);

// Idempotent header. Subsequent calls update the Last touched stamp
// only; user-edited body content is preserved.
void record_disclosure(std::string_view id,
                       std::string_view title,
                       std::string_view abstract,
                       std::string_view project_root);

// Append a dated bullet to one of the canonical sections. Auto-creates
// the disclosure with a stub header when it does not yet exist.
void add_claim       (std::string_view id, std::string_view claim,       std::string_view project_root);
void add_prior_art   (std::string_view id, std::string_view citation,    std::string_view project_root);
void add_supporting  (std::string_view id, std::string_view ref,         std::string_view project_root);
void set_status      (std::string_view id, std::string_view status,      std::string_view project_root);

std::vector<std::string>     list(std::string_view project_root);
std::optional<Disclosure>    read(std::string_view id, std::string_view project_root);

}
