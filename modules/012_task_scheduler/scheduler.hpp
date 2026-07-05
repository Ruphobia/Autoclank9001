#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Task scheduler: break a long work process into a list of steps,
// persist it, and execute the steps in dependency order. Built on top
// of 011_scripting so each step is a bash body that the scheduler
// hands to scripting::execute. Plans live under
// <project>/.tool/plans/<name>.json so they survive restarts.
namespace scheduler {

enum class Status {
    Pending  = 0,
    Running  = 1,
    Done     = 2,
    Failed   = 3,
    Skipped  = 4,
};

const char * to_string(Status s);
Status        status_from_string(std::string_view s);

struct Task {
    std::string              id;             // stable within a plan
    std::string              description;
    std::string              body;           // bash, handed to scripting::execute
    std::vector<std::string> depends_on;     // ids that must be Done first
    int                      timeout_seconds = 60;
    Status                   status   = Status::Pending;
    int                      exit_code = 0;
    bool                     timed_out = false;
    std::string              stdout_text;    // truncated when persisted
    std::string              stderr_text;
    int64_t                  started_at_unix  = 0;
    int64_t                  finished_at_unix = 0;
};

struct Plan {
    std::string       name;
    std::string       description;
    std::vector<Task> tasks;
};

// No model load. Ensures the plans directory exists lazily.
void init();
void shutdown();

// Persistence. Plan files live at <project>/.tool/plans/<name>.json.
// Names are sanitized to [A-Za-z0-9_-]. save_plan throws on empty name.
void                    save_plan(const Plan & p, std::string_view project_root);
Plan                    load_plan(std::string_view name, std::string_view project_root);
std::vector<std::string> list_plans(std::string_view project_root);

// Returns the index of the next runnable task (Pending, with every
// dependency Done) or nullopt if nothing can run right now. If a task
// can never run because a dependency Failed, it is auto-marked Skipped
// and the search continues.
std::optional<size_t> next_runnable(Plan & p);

// Execute one ready task: pulls the next runnable, runs its body via
// scripting::execute, updates status/timestamps/exit_code in place, and
// persists the plan. Returns the task index that ran, or nullopt if no
// task could run. `cwd` is passed through to scripting::execute.
std::optional<size_t> run_step(Plan & p, std::string_view project_root,
                               std::string_view cwd = {});

// Iterate run_step until no task can run (everything is Done, Failed, or
// Skipped). Returns the number of tasks that actually ran.
size_t run_all(Plan & p, std::string_view project_root,
               std::string_view cwd = {});

}
