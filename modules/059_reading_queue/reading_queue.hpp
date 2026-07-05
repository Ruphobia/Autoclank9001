#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Reading Queue and Tracker: per-project Kanban-style queue for the
// reference library. Items carry a status (Unread / Reading / Read /
// Dropped), a priority (1 highest .. 5 lowest), an eta_minutes
// estimate, and a session log so retrospective analytics ("how long
// did I actually spend on this paper?") are one query away.
namespace reading_queue {

enum class Status { Unread = 0, Reading = 1, Read = 2, Dropped = 3 };
const char * to_string(Status);
Status        status_from_string(std::string_view);

struct Item {
    int64_t      id                    = 0;
    int64_t      ref_id                = 0;     // optional link to 054_reference_library
    std::string  title;
    int          priority              = 3;     // 1..5
    Status       status                = Status::Unread;
    int          eta_minutes           = 0;
    int          total_session_minutes = 0;
    int64_t      added_unix            = 0;
    int64_t      last_session_unix     = 0;
};

void open(std::string_view project_root);
void close();

int64_t add(int64_t ref_id, std::string_view title,
            int priority = 3, int eta_minutes = 0);

void set_status  (int64_t id, Status);
void set_priority(int64_t id, int priority);
void set_eta     (int64_t id, int eta_minutes);
void log_session (int64_t id, int minutes, int64_t when_unix = 0);
void forget      (int64_t id);

std::optional<Item>  get(int64_t id);
std::vector<Item>    list_by_status(Status, size_t limit = 100);
std::vector<Item>    list_all(size_t limit = 200);

// Next-to-read recommendation: lowest-priority-number unread items,
// breaking ties by smallest eta_minutes (so fast wins land first).
std::vector<Item> recommend(size_t k = 5);

}
