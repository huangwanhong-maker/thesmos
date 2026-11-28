// Descriptions of occurrences at levels (decision 0010): one occurrence described as what was intended (E), what was done (O), what
// was recorded (M), what a community took it to be (S) and what an order qualifies it as (N), each description at one level and of one
// kind there, an occurrence at as many levels as it has descriptions (multiple membership), the descriptions joined by declared
// bridges, and practices found as patterns of linked descriptions.
//
// A description at S or N names the structure or order that makes it, so that conflicting descriptions by different structures stand
// side by side (Definition 44 of the theory note). A description at M has a grade (A a structured record with digests, B a text with
// its passage, C an account from memory or a self-report). A qualified conduct at N carries the order's label (a transmission, a
// breach). The facets of the taxonomy describe the levels it says, each value one of the facet's; a stage of formation of intent is
// epistemic, and no description at O, S or N is at that stage (Definition 45). A link joins two descriptions by a bridge whose pairs
// admit their levels (Definition 46). An instance of a practice gives each required slot, and each optional slot it can, a
// description at the slot's level, every link the practice asks between filled slots present in the record (Definition 47).
//
// The descriptions are a record of kategoria's ontologies on the taxonomy's ontology (record_of), each description a fact of its
// kind, each link a link of the relation its bridge makes for the levels it joins, so that kategoria holds a record of descriptions
// to the taxonomy (fits).

#pragma once

#include <thesmos/behaviour/ontology.hpp>

#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace thesmos::behaviour {

struct Description {
  std::string id, occurrence;
  std::string level;      // the level's letter
  std::string kind;       // at O a category of conduct, at N a form or a kind of act, else empty (the level itself)
  std::string structure;  // at S and N: the structure or order that makes it
  std::string grade;      // at M: A, B or C
  std::string label;      // at N, a qualified conduct's label
  std::string refers;     // what in the records it refers to: an act's seal, an entry of the behaviour journal, a document's digest
  std::map<std::string, std::string> facets;
  bool operator==(const Description&) const = default;
};
struct Linked {
  std::string bridge, from, to;  // the bridge, and the ids of the descriptions it joins
  bool operator==(const Linked&) const = default;
};
struct Descriptions {
  std::vector<Description> items;
  std::vector<Linked> links;
  const Description* find(const std::string& id) const {
    for (const auto& d : items)
      if (d.id == id) return &d;
    return nullptr;
  }
};

// The kind of the ontology a description is a fact of: the level's kind; at O the category it names; at N the form it names, or the
// kind of act (a constituted act), the first form when it names none.
inline std::string kind_of(const Part& t, const Description& d) {
  const Level* l = t.level(d.level);
  if (!l) return {};
  if (d.level == "O" && !d.kind.empty() && t.category(d.kind)) return ident(d.kind);
  if (d.level == "N") {
    if (d.kind.empty()) return ident(l->name);
    for (const auto& f : l->forms)
      if (f == d.kind) return ident(f);
    if (t.primitive(d.kind) || t.declared_kind(d.kind)) return ident(d.kind);
    return {};
  }
  return d.kind.empty() ? ident(l->name) : std::string();
}

// The levels at which an occurrence is described: its multiple membership.
inline std::set<std::string> levels_of(const Descriptions& ds, const std::string& occurrence) {
  std::set<std::string> out;
  for (const auto& d : ds.items)
    if (d.occurrence == occurrence) out.insert(d.level);
  return out;
}

// Every fault of a record of descriptions under a taxonomy in levels, each named.
inline std::vector<std::string> faults(const Part& t, const Descriptions& ds) {
  std::vector<std::string> out;
  if (t.levels.empty()) {
    out.push_back("the taxonomy has no levels of description");
    return out;
  }
  std::set<std::string> ids;
  for (const auto& d : ds.items) {
    const std::string where = "the description " + d.id;
    if (d.id.empty() || !ids.insert(d.id).second) out.push_back("a description has no id of its own: " + d.id);
    if (d.occurrence.empty()) out.push_back(where + " describes no occurrence");
    const Level* l = t.level(d.level);
    if (!l) {
      out.push_back(where + " is at level " + d.level + ", which the taxonomy lacks");
      continue;
    }
    if (kind_of(t, d).empty()) out.push_back(where + " is of the kind " + d.kind + ", which is no kind at level " + d.level);
    if (!l->indexed.empty() && d.structure.empty()) out.push_back(where + " is at " + l->name + " and names no " + l->indexed + " that makes it");
    if (d.level == "M" && d.grade != "A" && d.grade != "B" && d.grade != "C") out.push_back(where + " is a manifestation and has no grade A, B or C");
    if (d.level != "M" && !d.grade.empty()) out.push_back(where + " has a grade, which only a manifestation has");
    if (d.level == "N" && d.kind == (l->forms.size() > 1 ? l->forms[1] : std::string("?")) && d.label.empty()) out.push_back(where + " qualifies conduct and gives no label");
    if (d.level == "N" && !l->forms.empty() && d.kind == l->forms.front()) out.push_back(where + " is a constituted act and names no kind of act: a constituted act is of a kind");
    for (const auto& [name, value] : d.facets) {
      const Facet* f = t.facet(name);
      if (!f) {
        out.push_back(where + " is described on " + name + ", which is no facet");
        continue;
      }
      if (std::find(f->levels.begin(), f->levels.end(), d.level) == f->levels.end()) out.push_back(where + ": the facet " + name + " does not describe level " + d.level);
      if (std::find(f->values.begin(), f->values.end(), value) == f->values.end()) out.push_back(where + ": " + value + " is no value of the facet " + name);
      if (name == "stage" && !f->values.empty()) {
        const bool formation = value == f->values.front();
        if (d.level == "E" && !formation) out.push_back(where + " is epistemic and at the stage " + value + ": epistemic behaviour is the formation of intent");
        if (d.level != "E" && formation) out.push_back(where + " is at level " + d.level + " and at the formation of intent, which is epistemic");
      }
    }
  }
  for (const auto& k : ds.links) {
    const std::string where = "the link " + k.from + " " + k.bridge + " " + k.to;
    const Bridge* b = t.bridge(k.bridge);
    const Description *f = ds.find(k.from), *g = ds.find(k.to);
    if (!b) out.push_back(where + " names no bridge");
    else if (!f || !g) out.push_back(where + " joins a description the record lacks");
    else if (std::find(b->pairs.begin(), b->pairs.end(), std::pair<std::string, std::string>{f->level, g->level}) == b->pairs.end())
      out.push_back(where + " joins " + f->level + " to " + g->level + ", which " + k.bridge + " does not join");
  }
  return out;
}

// The record of descriptions as a record of kategoria's ontologies: each description a fact of its kind with its facets and grade, each
// link a link of the relation its bridge makes for the two levels.
inline ko::Record record_of(const Part& t, const Descriptions& ds) {
  ko::Record r;
  for (const auto& d : ds.items) {
    ko::Fact f;
    f.id = ident(d.id), f.kind = kind_of(t, d), f.text = d.id;
    for (const auto& [name, value] : d.facets) f.axes[ident(name)] = ident(value);
    if (!d.grade.empty()) f.axes["grade"] = d.grade;
    if (d.level == "N")
      if (const Primitive* p = t.primitive(primitive_of(t, d.kind))) f.axes["force"] = std::string(acts::word(p->force));
    r.facts.push_back(std::move(f));
  }
  for (const auto& k : ds.links) {
    const Bridge* b = t.bridge(k.bridge);
    const Description *f = ds.find(k.from), *g = ds.find(k.to);
    if (!b || !f || !g) continue;
    r.links.push_back({relation_name(*b, {f->level, g->level}), ident(k.from), ident(k.to)});
  }
  return r;
}

// Every instance of a practice in a record of descriptions: each a map from its roles to descriptions, every required slot filled at
// its level, each optional slot filled where a description at its level is linked as the practice asks, and every link the practice
// asks between filled slots present. Slots are filled along the practice's links from the first required slot, so that each instance
// is found once.
inline std::vector<std::map<std::string, std::string>> instances(const Part& t, const Descriptions& ds, const Practice& p) {
  std::vector<std::map<std::string, std::string>> out;
  std::map<std::string, const Slot*> slot;
  for (const auto& s : p.slots) slot[s.role] = &s;
  const Slot* first = nullptr;
  for (const auto& s : p.slots)
    if (!s.optional && !first) first = &s;
  if (!first) return out;
  // The order in which the slots are filled: from the first, the required slots, each linked to a required slot already placed (the
  // practice's required slots are joined among themselves), then the optional ones, each linked to a slot already placed.
  std::vector<const Slot*> order = {first};
  std::set<std::string> placed = {first->role};
  for (const bool optional : {false, true})
    for (bool grew = true; grew;) {
      grew = false;
      for (const auto& k : p.links)
        for (const auto& [a, b] : {std::pair{k.from, k.to}, std::pair{k.to, k.from}})
          if (placed.count(a) && !placed.count(b) && slot.count(b) && slot[b]->optional == optional && (optional || !slot[a]->optional))
            placed.insert(b), order.push_back(slot[b]), grew = true;
    }
  auto linked = [&](const std::string& bridge, const std::string& from, const std::string& to) {
    for (const auto& k : ds.links)
      if (k.bridge == bridge && k.from == from && k.to == to) return true;
    return false;
  };
  std::map<std::string, std::string> at;  // role -> description id ("" for an optional slot left empty)
  std::set<std::string> used;
  std::function<void(std::size_t)> fill = [&](std::size_t i) {
    if (i == order.size()) {
      std::map<std::string, std::string> found;
      for (const auto& [role, id] : at)
        if (!id.empty()) found[role] = id;
      out.push_back(found);
      return;
    }
    const Slot& s = *order[i];
    bool any = false;
    for (const auto& d : ds.items) {
      if (d.level != s.level || used.count(d.id)) continue;
      bool fits = true;
      for (const auto& k : p.links) {
        const auto f = at.find(k.from), g = at.find(k.to);
        if (k.from == s.role && g != at.end() && !g->second.empty()) fits = fits && linked(k.bridge, d.id, g->second);
        if (k.to == s.role && f != at.end() && !f->second.empty()) fits = fits && linked(k.bridge, f->second, d.id);
      }
      bool touches = i == 0;  // a slot after the first is filled by a description linked to one already placed
      for (const auto& k : p.links) {
        const auto f = at.find(k.from), g = at.find(k.to);
        touches = touches || (k.from == s.role && g != at.end() && !g->second.empty()) || (k.to == s.role && f != at.end() && !f->second.empty());
      }
      if (!fits || !touches) continue;
      any = true;
      at[s.role] = d.id, used.insert(d.id);
      fill(i + 1);
      at.erase(s.role), used.erase(d.id);
    }
    if (s.optional && !any) {
      at[s.role] = "";
      fill(i + 1);
      at.erase(s.role);
    }
  };
  fill(0);
  return out;
}

}  // namespace thesmos::behaviour
