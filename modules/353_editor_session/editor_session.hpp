// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Server-side session state for an interactive KiCad editor tab.
//
// One editor session owns one Schematic + one Board (either or both
// may be empty). Every mutation goes through a Command, which knows
// how to apply itself and how to undo. The command stack supports
// unlimited undo/redo per session.
//
// This is the piece that lets the browser click-place-drag against a
// server-authoritative model; the client renders diffs, the server
// keeps the source of truth.
namespace editor_session {

using SessionId = std::string;   // opaque, chosen by the client

// A single reversible operation. Apply returns true on success; when
// it returns false the command is not pushed to the undo stack.
class Command {
public:
    virtual ~Command() = default;
    virtual bool        apply()   = 0;
    virtual bool        unapply() = 0;
    virtual std::string label() const = 0;
};
using CommandPtr = std::unique_ptr<Command>;

// Selection is a set of item UUIDs across the two documents.
struct Selection {
    std::unordered_set<kicad_model::UUID> sch;
    std::unordered_set<kicad_model::UUID> pcb;
    void clear() { sch.clear(); pcb.clear(); }
    std::size_t size() const { return sch.size() + pcb.size(); }
};

class Session {
public:
    explicit Session(SessionId id) : m_id(std::move(id)) {}

    const SessionId & id() const { return m_id; }

    // Documents.
    kicad_model::Schematic &       sch()       { return m_sch; }
    const kicad_model::Schematic & sch() const { return m_sch; }
    kicad_model::Board           & pcb()       { return m_pcb; }
    const kicad_model::Board     & pcb() const { return m_pcb; }

    // Execute + push to undo stack.
    bool run(CommandPtr cmd);

    bool can_undo() const { return !m_undo.empty(); }
    bool can_redo() const { return !m_redo.empty(); }

    bool undo();
    bool redo();

    // Clipboard (session-local; server-side buffer).
    void clipboard_put(std::string mime, std::string data);
    // Returns pair(mime, data); empty when nothing there.
    std::pair<std::string, std::string> clipboard_get() const;

    Selection & selection()             { return m_sel; }
    const Selection & selection() const { return m_sel; }

    // Coarse "version" bumped on every mutation. Clients that miss a
    // notification poll GET /api/eda/editor/state and use this to know
    // whether they need to re-fetch the document.
    std::uint64_t version() const { return m_version; }

    // Item lookup by UUID (searches both documents).
    kicad_model::ItemPtr find_sch(const kicad_model::UUID & u);
    kicad_model::ItemPtr find_pcb(const kicad_model::UUID & u);

    // Whole-document replace (used when the client saves a fresh file).
    void set_sch(kicad_model::Schematic s);
    void set_pcb(kicad_model::Board b);

private:
    void bump() { ++m_version; }

    SessionId              m_id;
    kicad_model::Schematic m_sch;
    kicad_model::Board     m_pcb;
    std::vector<CommandPtr> m_undo;
    std::vector<CommandPtr> m_redo;
    Selection               m_sel;
    std::pair<std::string, std::string> m_clipboard;
    std::uint64_t           m_version = 1;
};

// Global session registry.
class SessionStore {
public:
    // Get-or-create.
    std::shared_ptr<Session> get(const SessionId & id);
    void drop(const SessionId & id);
    std::vector<SessionId> list() const;

private:
    mutable std::mutex m_mtx;
    std::unordered_map<SessionId, std::shared_ptr<Session>> m_map;
};

SessionStore & store();

// --- Concrete commands (schematic side) ------------------------------

// Add a SchItem at position `at`. Ownership transferred on apply.
CommandPtr add_sch_item(std::shared_ptr<Session> s, kicad_model::ItemPtr item);
// Remove the item with `uuid`; keeps a copy so undo can restore.
CommandPtr remove_sch_item(std::shared_ptr<Session> s, kicad_model::UUID uuid);
// Translate an item by (dx, dy) nanometers.
CommandPtr move_sch_item(std::shared_ptr<Session> s, kicad_model::UUID uuid,
                         long long dx_nm, long long dy_nm);
// Rotate an item by `deg` (should be a multiple of 90 for MVP).
CommandPtr rotate_sch_item(std::shared_ptr<Session> s, kicad_model::UUID uuid, double deg);
// Set a named field's value on a SchSymbol.
CommandPtr edit_sch_field(std::shared_ptr<Session> s, kicad_model::UUID uuid,
                          std::string field_name, std::string new_value);
// Mirror across X axis (flips Y) or Y axis (flips X) on a SchSymbol.
CommandPtr mirror_sch_item(std::shared_ptr<Session> s, kicad_model::UUID uuid, char axis);

// --- Concrete commands (PCB side) ------------------------------------

CommandPtr add_pcb_item(std::shared_ptr<Session> s, kicad_model::ItemPtr item);
CommandPtr remove_pcb_item(std::shared_ptr<Session> s, kicad_model::UUID uuid);
CommandPtr move_pcb_item(std::shared_ptr<Session> s, kicad_model::UUID uuid,
                         long long dx_nm, long long dy_nm);
CommandPtr rotate_pcb_item(std::shared_ptr<Session> s, kicad_model::UUID uuid, double deg);
// Flip a footprint to the opposite copper layer (F.Cu <-> B.Cu).
CommandPtr flip_pcb_item(std::shared_ptr<Session> s, kicad_model::UUID uuid);

} // namespace editor_session
