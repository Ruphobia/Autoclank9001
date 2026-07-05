// SPDX-License-Identifier: GPL-3.0-or-later
#include "editor_session.hpp"

#include <algorithm>
#include <cmath>

namespace editor_session {

using namespace kicad_model;

// -------------------- Session --------------------

bool Session::run(CommandPtr cmd) {
    if (!cmd) return false;
    if (!cmd->apply()) return false;
    m_undo.push_back(std::move(cmd));
    m_redo.clear();
    bump();
    return true;
}

bool Session::undo() {
    if (m_undo.empty()) return false;
    auto cmd = std::move(m_undo.back());
    m_undo.pop_back();
    bool ok = cmd->unapply();
    if (ok) { m_redo.push_back(std::move(cmd)); bump(); }
    return ok;
}
bool Session::redo() {
    if (m_redo.empty()) return false;
    auto cmd = std::move(m_redo.back());
    m_redo.pop_back();
    bool ok = cmd->apply();
    if (ok) { m_undo.push_back(std::move(cmd)); bump(); }
    return ok;
}

void Session::clipboard_put(std::string mime, std::string data) {
    m_clipboard = { std::move(mime), std::move(data) };
}
std::pair<std::string, std::string> Session::clipboard_get() const { return m_clipboard; }

ItemPtr Session::find_sch(const UUID & u) {
    for (auto & it : m_sch.root.items) if (it->uuid == u) return it;
    return {};
}
ItemPtr Session::find_pcb(const UUID & u) {
    for (auto & it : m_pcb.items) if (it->uuid == u) return it;
    return {};
}

void Session::set_sch(Schematic s) { m_sch = std::move(s); bump(); }
void Session::set_pcb(Board b)     { m_pcb = std::move(b); bump(); }

// -------------------- SessionStore --------------------

std::shared_ptr<Session> SessionStore::get(const SessionId & id) {
    std::lock_guard<std::mutex> lk(m_mtx);
    auto it = m_map.find(id);
    if (it != m_map.end()) return it->second;
    auto s = std::make_shared<Session>(id);
    m_map[id] = s;
    return s;
}
void SessionStore::drop(const SessionId & id) {
    std::lock_guard<std::mutex> lk(m_mtx);
    m_map.erase(id);
}
std::vector<SessionId> SessionStore::list() const {
    std::lock_guard<std::mutex> lk(m_mtx);
    std::vector<SessionId> out;
    out.reserve(m_map.size());
    for (const auto & kv : m_map) out.push_back(kv.first);
    return out;
}

SessionStore & store() {
    static SessionStore g;
    return g;
}

// =========================================================================
// Commands
// =========================================================================

namespace {

// Shared helpers.
template <typename Container>
auto find_by_uuid(Container & c, const UUID & u) -> decltype(c.begin()) {
    return std::find_if(c.begin(), c.end(),
                        [&](const auto & p) { return p->uuid == u; });
}

// Translate the item's primary position by (dx, dy). Understands the
// item types that carry a single (at) point; wires/tracks translate all
// their vertices.
void translate_sch(ItemPtr it, long long dx, long long dy) {
    if (!it) return;
    switch (it->type) {
        case ItemType::SchSymbol:      static_cast<SchSymbol*>(it.get())->at.x    += dx; static_cast<SchSymbol*>(it.get())->at.y    += dy; break;
        case ItemType::SchJunction:    static_cast<SchJunction*>(it.get())->at.x  += dx; static_cast<SchJunction*>(it.get())->at.y  += dy; break;
        case ItemType::SchNoConnect:   static_cast<SchNoConnect*>(it.get())->at.x += dx; static_cast<SchNoConnect*>(it.get())->at.y += dy; break;
        case ItemType::SchLabel:
        case ItemType::SchGlobalLabel:
        case ItemType::SchHierLabel:   static_cast<SchLabel*>(it.get())->at.x     += dx; static_cast<SchLabel*>(it.get())->at.y     += dy; break;
        case ItemType::SchText:        static_cast<SchText*>(it.get())->at.x      += dx; static_cast<SchText*>(it.get())->at.y      += dy; break;
        case ItemType::SchSheet:       static_cast<SchSheet*>(it.get())->at.x     += dx; static_cast<SchSheet*>(it.get())->at.y     += dy; break;
        case ItemType::SchWire: {
            auto * w = static_cast<SchWire*>(it.get());
            for (auto & p : w->pts) { p.x += dx; p.y += dy; }
            break;
        }
        case ItemType::SchBus: {
            auto * w = static_cast<SchBus*>(it.get());
            for (auto & p : w->pts) { p.x += dx; p.y += dy; }
            break;
        }
        default: break;
    }
}

void translate_pcb(ItemPtr it, long long dx, long long dy) {
    if (!it) return;
    switch (it->type) {
        case ItemType::PcbFootprint: static_cast<Footprint*>(it.get())->at.x  += dx; static_cast<Footprint*>(it.get())->at.y  += dy; break;
        case ItemType::PcbVia:       static_cast<PcbVia*>(it.get())->at.x     += dx; static_cast<PcbVia*>(it.get())->at.y     += dy; break;
        case ItemType::PcbTrack: {
            auto * t = static_cast<PcbTrack*>(it.get());
            t->start.x += dx; t->start.y += dy;
            t->end.x   += dx; t->end.y   += dy;
            break;
        }
        case ItemType::PcbArc: {
            auto * a = static_cast<PcbArc*>(it.get());
            a->start.x += dx; a->start.y += dy;
            a->mid.x   += dx; a->mid.y   += dy;
            a->end.x   += dx; a->end.y   += dy;
            break;
        }
        case ItemType::PcbGrLine: {
            auto * g = static_cast<GrLine*>(it.get());
            g->start.x += dx; g->start.y += dy;
            g->end.x   += dx; g->end.y   += dy;
            break;
        }
        default: break;
    }
}

// -------------------- AddSchItem --------------------
class AddSchItem : public Command {
public:
    AddSchItem(std::shared_ptr<Session> s, ItemPtr item)
        : m_s(std::move(s)), m_item(std::move(item)) {
        if (m_item && m_item->uuid.empty()) m_item->uuid = make_uuid();
    }
    bool apply() override {
        if (!m_item) return false;
        m_s->sch().root.items.push_back(m_item);
        return true;
    }
    bool unapply() override {
        auto & v = m_s->sch().root.items;
        auto it = find_by_uuid(v, m_item->uuid);
        if (it == v.end()) return false;
        v.erase(it);
        return true;
    }
    std::string label() const override { return "Add schematic item"; }
private:
    std::shared_ptr<Session> m_s;
    ItemPtr                  m_item;
};

// -------------------- RemoveSchItem --------------------
class RemoveSchItem : public Command {
public:
    RemoveSchItem(std::shared_ptr<Session> s, UUID uuid)
        : m_s(std::move(s)), m_uuid(std::move(uuid)) {}
    bool apply() override {
        auto & v = m_s->sch().root.items;
        auto it = find_by_uuid(v, m_uuid);
        if (it == v.end()) return false;
        m_backup = *it;
        m_index  = static_cast<std::size_t>(std::distance(v.begin(), it));
        v.erase(it);
        return true;
    }
    bool unapply() override {
        auto & v = m_s->sch().root.items;
        if (!m_backup) return false;
        v.insert(v.begin() + std::min(m_index, v.size()), m_backup);
        return true;
    }
    std::string label() const override { return "Remove schematic item"; }
private:
    std::shared_ptr<Session> m_s;
    UUID                     m_uuid;
    ItemPtr                  m_backup;
    std::size_t              m_index = 0;
};

// -------------------- MoveSchItem --------------------
class MoveSchItem : public Command {
public:
    MoveSchItem(std::shared_ptr<Session> s, UUID uuid, long long dx, long long dy)
        : m_s(std::move(s)), m_uuid(std::move(uuid)), m_dx(dx), m_dy(dy) {}
    bool apply()   override { auto it = m_s->find_sch(m_uuid); if (!it) return false; translate_sch(it,  m_dx,  m_dy); return true; }
    bool unapply() override { auto it = m_s->find_sch(m_uuid); if (!it) return false; translate_sch(it, -m_dx, -m_dy); return true; }
    std::string label() const override { return "Move schematic item"; }
private:
    std::shared_ptr<Session> m_s;
    UUID                     m_uuid;
    long long                m_dx = 0, m_dy = 0;
};

// -------------------- RotateSchItem --------------------
class RotateSchItem : public Command {
public:
    RotateSchItem(std::shared_ptr<Session> s, UUID uuid, double deg)
        : m_s(std::move(s)), m_uuid(std::move(uuid)), m_deg(deg) {}
    bool rotate_by(double d) {
        auto it = m_s->find_sch(m_uuid);
        if (!it) return false;
        if (it->type != ItemType::SchSymbol) return false;
        auto * s = static_cast<SchSymbol*>(it.get());
        s->angle = geom::EDA_ANGLE{s->angle.deg() + d}.normalized();
        return true;
    }
    bool apply()   override { return rotate_by( m_deg); }
    bool unapply() override { return rotate_by(-m_deg); }
    std::string label() const override { return "Rotate schematic item"; }
private:
    std::shared_ptr<Session> m_s;
    UUID                     m_uuid;
    double                   m_deg = 0.0;
};

// -------------------- EditSchField --------------------
class EditSchField : public Command {
public:
    EditSchField(std::shared_ptr<Session> s, UUID uuid, std::string field, std::string val)
        : m_s(std::move(s)), m_uuid(std::move(uuid)), m_field(std::move(field)), m_new(std::move(val)) {}
    bool apply() override {
        auto it = m_s->find_sch(m_uuid);
        if (!it || it->type != ItemType::SchSymbol) return false;
        auto * sym = static_cast<SchSymbol*>(it.get());
        for (auto & f : sym->fields) {
            if (f.name == m_field) { m_old = f.value; f.value = m_new; return true; }
        }
        // Not present; add.
        Field f; f.name = m_field; f.value = m_new; f.at = sym->at; f.uuid = make_uuid();
        sym->fields.push_back(f);
        m_added = true;
        return true;
    }
    bool unapply() override {
        auto it = m_s->find_sch(m_uuid);
        if (!it || it->type != ItemType::SchSymbol) return false;
        auto * sym = static_cast<SchSymbol*>(it.get());
        if (m_added) {
            for (auto fit = sym->fields.begin(); fit != sym->fields.end(); ++fit) {
                if (fit->name == m_field) { sym->fields.erase(fit); return true; }
            }
            return false;
        }
        for (auto & f : sym->fields) if (f.name == m_field) { f.value = m_old; return true; }
        return false;
    }
    std::string label() const override { return "Edit field"; }
private:
    std::shared_ptr<Session> m_s;
    UUID                     m_uuid;
    std::string              m_field, m_new, m_old;
    bool                     m_added = false;
};

// -------------------- PCB variants --------------------

class AddPcbItem : public Command {
public:
    AddPcbItem(std::shared_ptr<Session> s, ItemPtr it) : m_s(std::move(s)), m_item(std::move(it)) {
        if (m_item && m_item->uuid.empty()) m_item->uuid = make_uuid();
    }
    bool apply() override { if (!m_item) return false; m_s->pcb().items.push_back(m_item); return true; }
    bool unapply() override {
        auto & v = m_s->pcb().items;
        auto it = find_by_uuid(v, m_item->uuid);
        if (it == v.end()) return false;
        v.erase(it); return true;
    }
    std::string label() const override { return "Add PCB item"; }
private:
    std::shared_ptr<Session> m_s;
    ItemPtr                  m_item;
};
class RemovePcbItem : public Command {
public:
    RemovePcbItem(std::shared_ptr<Session> s, UUID u) : m_s(std::move(s)), m_uuid(std::move(u)) {}
    bool apply() override {
        auto & v = m_s->pcb().items;
        auto it = find_by_uuid(v, m_uuid);
        if (it == v.end()) return false;
        m_backup = *it;
        m_idx    = static_cast<std::size_t>(std::distance(v.begin(), it));
        v.erase(it); return true;
    }
    bool unapply() override {
        auto & v = m_s->pcb().items;
        if (!m_backup) return false;
        v.insert(v.begin() + std::min(m_idx, v.size()), m_backup);
        return true;
    }
    std::string label() const override { return "Remove PCB item"; }
private:
    std::shared_ptr<Session> m_s;
    UUID                     m_uuid;
    ItemPtr                  m_backup;
    std::size_t              m_idx = 0;
};
class MovePcbItem : public Command {
public:
    MovePcbItem(std::shared_ptr<Session> s, UUID u, long long dx, long long dy)
        : m_s(std::move(s)), m_uuid(std::move(u)), m_dx(dx), m_dy(dy) {}
    bool apply()   override { auto it = m_s->find_pcb(m_uuid); if (!it) return false; translate_pcb(it,  m_dx,  m_dy); return true; }
    bool unapply() override { auto it = m_s->find_pcb(m_uuid); if (!it) return false; translate_pcb(it, -m_dx, -m_dy); return true; }
    std::string label() const override { return "Move PCB item"; }
private:
    std::shared_ptr<Session> m_s;
    UUID                     m_uuid;
    long long                m_dx = 0, m_dy = 0;
};
class RotatePcbItem : public Command {
public:
    RotatePcbItem(std::shared_ptr<Session> s, UUID u, double d)
        : m_s(std::move(s)), m_uuid(std::move(u)), m_deg(d) {}
    bool rot(double d) {
        auto it = m_s->find_pcb(m_uuid);
        if (!it || it->type != ItemType::PcbFootprint) return false;
        auto * fp = static_cast<Footprint*>(it.get());
        fp->angle = geom::EDA_ANGLE{fp->angle.deg() + d}.normalized();
        return true;
    }
    bool apply()   override { return rot(m_deg); }
    bool unapply() override { return rot(-m_deg); }
    std::string label() const override { return "Rotate footprint"; }
private:
    std::shared_ptr<Session> m_s;
    UUID                     m_uuid;
    double                   m_deg = 0.0;
};

} // namespace

// -------------------- MirrorSchItem --------------------
namespace {
class MirrorSchItem : public Command {
public:
    MirrorSchItem(std::shared_ptr<Session> s, UUID uuid, char axis)
        : m_s(std::move(s)), m_uuid(std::move(uuid)), m_axis(axis) {}
    bool flip() {
        auto it = m_s->find_sch(m_uuid);
        if (!it || it->type != ItemType::SchSymbol) return false;
        auto * s = static_cast<SchSymbol*>(it.get());
        if (m_axis == 'x' || m_axis == 'X') s->mirror_x = !s->mirror_x;
        if (m_axis == 'y' || m_axis == 'Y') s->mirror_y = !s->mirror_y;
        return true;
    }
    bool apply()   override { return flip(); }
    bool unapply() override { return flip(); }
    std::string label() const override { return std::string("Mirror ") + m_axis; }
private:
    std::shared_ptr<Session> m_s;
    UUID                     m_uuid;
    char                     m_axis;
};

class FlipPcbItem : public Command {
public:
    FlipPcbItem(std::shared_ptr<Session> s, UUID uuid) : m_s(std::move(s)), m_uuid(std::move(uuid)) {}
    bool flip() {
        auto it = m_s->find_pcb(m_uuid);
        if (!it || it->type != ItemType::PcbFootprint) return false;
        auto * fp = static_cast<Footprint*>(it.get());
        fp->placement_layer = (fp->placement_layer == "B.Cu") ? "F.Cu" : "B.Cu";
        // Mirror pad layers between F.* and B.*.
        for (auto & p : fp->pads) {
            for (auto & L : p.layers) {
                if      (L.rfind("F.", 0) == 0) L = "B." + L.substr(2);
                else if (L.rfind("B.", 0) == 0) L = "F." + L.substr(2);
            }
        }
        return true;
    }
    bool apply()   override { return flip(); }
    bool unapply() override { return flip(); }
    std::string label() const override { return "Flip footprint"; }
private:
    std::shared_ptr<Session> m_s;
    UUID                     m_uuid;
};
} // namespace

CommandPtr add_sch_item   (std::shared_ptr<Session> s, ItemPtr it)                       { return std::make_unique<AddSchItem>   (std::move(s), std::move(it)); }
CommandPtr remove_sch_item(std::shared_ptr<Session> s, UUID uuid)                        { return std::make_unique<RemoveSchItem>(std::move(s), std::move(uuid)); }
CommandPtr move_sch_item  (std::shared_ptr<Session> s, UUID uuid, long long dx, long long dy) { return std::make_unique<MoveSchItem>(std::move(s), std::move(uuid), dx, dy); }
CommandPtr rotate_sch_item(std::shared_ptr<Session> s, UUID uuid, double deg)            { return std::make_unique<RotateSchItem>(std::move(s), std::move(uuid), deg); }
CommandPtr edit_sch_field (std::shared_ptr<Session> s, UUID uuid, std::string f, std::string v) { return std::make_unique<EditSchField>(std::move(s), std::move(uuid), std::move(f), std::move(v)); }
CommandPtr mirror_sch_item(std::shared_ptr<Session> s, UUID uuid, char axis)             { return std::make_unique<MirrorSchItem>(std::move(s), std::move(uuid), axis); }

CommandPtr add_pcb_item   (std::shared_ptr<Session> s, ItemPtr it)                       { return std::make_unique<AddPcbItem>   (std::move(s), std::move(it)); }
CommandPtr remove_pcb_item(std::shared_ptr<Session> s, UUID uuid)                        { return std::make_unique<RemovePcbItem>(std::move(s), std::move(uuid)); }
CommandPtr move_pcb_item  (std::shared_ptr<Session> s, UUID uuid, long long dx, long long dy) { return std::make_unique<MovePcbItem>(std::move(s), std::move(uuid), dx, dy); }
CommandPtr rotate_pcb_item(std::shared_ptr<Session> s, UUID uuid, double deg)            { return std::make_unique<RotatePcbItem>(std::move(s), std::move(uuid), deg); }
CommandPtr flip_pcb_item  (std::shared_ptr<Session> s, UUID uuid)                        { return std::make_unique<FlipPcbItem>  (std::move(s), std::move(uuid)); }

} // namespace editor_session
