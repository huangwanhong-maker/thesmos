// The taxonomy of behaviour as an ontology of kategoria (decision 0008, on kategoria's decision 0005): its kinds and categories as a
// forest in the registers of the Generative Relational framework, its admission decided on the orders that forest generates, a revision
// admitted when its map of kinds is a functor that keeps every kind of the ground fixed, and a record of acts carried along a revision
// as the left Kan extension.
//
// The first version (registers Manifestational < Relational < Ontological):
//
//   behaviour (Ontological)
//     normative_act (Relational)
//       each class of normative acts
//         each kind of the ground
//           each declared kind beneath the kind it counts as
//     conduct (Ontological)
//       each category of conduct
//     event (Ontological)
//     state (Relational)
//       measured_state (Manifestational)
//     observation (Manifestational)
//
// The second version, in levels (decision 0010):
//
//   behaviour (Ontological)
//     epistemic_behaviour (Ontological)          E
//     ontological_conduct (Ontological)          O, each category of conduct beneath it
//     manifest_conduct (Manifestational)         M
//     social_conduct (Relational)                S
//     normative_conduct (Relational)             N
//       constituted_act                          its first form: each class, each kind of the ground, each declared kind
//       qualified_conduct                        its second form
//     event (Ontological)
//     state (Relational)
//       measured_state (Manifestational)
//
// with each facet an axis of the levels it describes, and each bridge a relation for each pair of levels it joins (named after the
// bridge, with the two letters when it joins more than one pair). The first version's revision into the second sends normative_act to
// constituted_act, conduct to ontological_conduct and observation to manifest_conduct, and fixes every other root and every kind of
// the ground.
//
// The force of a normative act is a nominal axis required for every normative act; an observation's grade is an ordered axis, C < B <
// A; a categorical or ordinal feature with a vocabulary is an axis of conduct (or of epistemic behaviour, for a feature of level E).
// Names are written as identifiers (behaviour::ident).
//
// The fold admits a taxonomy and its revisions by the direct constraints of behaviour.hpp (faults, revision_faults), which read the
// same claims off the declaration; kategoria decides them here on the categories the declaration generates, and the checks hold the
// two to agree, as search and check are kept apart.

#pragma once

#include <thesmos/behaviour/behaviour.hpp>

#include <kategoria/ontologies/ontologies.hpp>

#include <algorithm>
#include <string>
#include <vector>

namespace thesmos::behaviour {

namespace ko = kategoria::ontologies;

// The kinds of a level and of the constituted acts, as identifiers.
inline std::string level_kind(const Part& t, const std::string& letter) {
  const Level* l = t.level(letter);
  return l ? ident(l->name) : std::string();
}
inline std::string constituted_kind(const Part& t) {
  const Level* n = t.level("N");
  return n && !n->forms.empty() ? ident(n->forms.front()) : std::string("normative_act");
}
// The kinds under which the kinds of act, the categories of conduct and the observations stand: the first version's roots, or the
// second's levels.
inline std::string acts_root(const Part& t) { return t.levels.empty() ? std::string("normative_act") : constituted_kind(t); }
inline std::string conduct_root(const Part& t) { return t.levels.empty() ? std::string("conduct") : level_kind(t, "O"); }
inline std::string observation_root(const Part& t) { return t.levels.empty() ? std::string("observation") : level_kind(t, "M"); }

// The name of the relation a bridge makes for one pair of levels.
inline std::string relation_name(const Bridge& b, const std::pair<std::string, std::string>& pair) {
  return b.pairs.size() == 1 ? ident(b.name) : ident(b.name) + "_" + pair.first + pair.second;
}

// The kinds the taxonomy fixes: its roots, levels and forms, the classes and the kinds of the ground. A revision sends each to itself,
// and every declared kind keeps the kind of the ground it reduces to.
inline std::vector<std::string> fixed_kinds(const Part& t) {
  std::vector<std::string> out = {"behaviour", "event", "state", "measured_state"};
  if (t.levels.empty())
    for (const char* k : {"normative_act", "conduct", "observation"}) out.push_back(k);
  for (const auto& l : t.levels) {
    out.push_back(ident(l.name));
    for (const auto& f : l.forms) out.push_back(ident(f));
  }
  for (const auto& c : t.classes) out.push_back(ident(c));
  for (const auto& p : t.primitives) out.push_back(ident(p.kind));
  return out;
}
// The kinds a revision fixes: the old version's, but for the roots the base's revision into levels recasts.
inline std::vector<std::string> fixed_kinds(const Part& from, const Part& to) {
  auto out = fixed_kinds(from);
  if (from.levels.empty() && !to.levels.empty())
    out.erase(std::remove_if(out.begin(), out.end(), [](const std::string& k) { return k == "normative_act" || k == "conduct" || k == "observation"; }), out.end());
  return out;
}

inline ko::Ontology ontology_of(const Part& t) {
  ko::Ontology o;
  o.name = "behaviour", o.version = t.version;
  o.registers = {"Manifestational", "Relational", "Ontological"};
  o.kinds.push_back({"behaviour", "", "Ontological"});
  const std::string acts = acts_root(t), conduct = conduct_root(t), observation = observation_root(t);
  if (t.levels.empty())
    o.kinds.push_back({"normative_act", "behaviour", "Relational"});
  for (const auto& l : t.levels) {
    o.kinds.push_back({ident(l.name), "behaviour", l.register_});
    for (const auto& f : l.forms) o.kinds.push_back({ident(f), ident(l.name), ""});
  }
  for (const auto& c : t.classes) o.kinds.push_back({ident(c), acts, ""});
  for (const auto& p : t.primitives) o.kinds.push_back({ident(p.kind), ident(p.cls), ""});
  for (const auto& d : t.declared) o.kinds.push_back({ident(d.kind), ident(d.counts_as), ""});
  if (t.levels.empty()) o.kinds.push_back({"conduct", "behaviour", "Ontological"});
  for (const auto& c : t.conduct) o.kinds.push_back({ident(c.name), conduct, ""});
  o.kinds.push_back({"event", "behaviour", "Ontological"});
  o.kinds.push_back({"state", "behaviour", "Relational"});
  o.kinds.push_back({"measured_state", "state", "Manifestational"});
  if (t.levels.empty()) o.kinds.push_back({"observation", "behaviour", "Manifestational"});
  o.axes.push_back({"force", false, {"expressive", "assertive", "directive", "commissive", "declarative"}, {acts}, {acts}});
  o.axes.push_back({"grade", true, {"C", "B", "A"}, {observation}, {}});
  for (const auto& f : t.features) {
    if ((f.space != "categorical" && f.space != "ordinal") || f.vocabulary.empty()) continue;
    ko::Axis a;
    a.name = ident(f.name), a.ordered = f.space == "ordinal";
    for (const auto& v : f.vocabulary) a.values.push_back(ident(v));
    a.applies = {f.level == "E" ? level_kind(t, "E") : conduct};
    o.axes.push_back(std::move(a));
  }
  for (const auto& f : t.facets) {
    ko::Axis a;
    a.name = ident(f.name), a.ordered = f.ordered;
    for (const auto& v : f.values) a.values.push_back(ident(v));
    for (const auto& l : f.levels) a.applies.push_back(level_kind(t, l));
    o.axes.push_back(std::move(a));
  }
  for (const auto& b : t.bridges)
    for (const auto& pr : b.pairs) o.relations.push_back({relation_name(b, pr), level_kind(t, pr.first), level_kind(t, pr.second)});
  return o;
}

// The mapping of a revision: the declared kinds and categories it renames, as identifiers.
inline ko::Mapping mapping_of(const Part& from, const Part& to) {
  ko::Mapping m;
  m.name = "behaviour", m.from = from.version, m.to = to.version;
  for (const auto& [o, n] : to.revised_kinds) m.kinds[ident(o)] = ident(n);
  return m;
}

// The taxonomy admitted by kategoria: its constraints, its kinds and registers orders with one root, the register a functor.
inline ko::Verification verify(const Part& t) {
  ko::Requirements r;
  r.one_root = true;
  return ko::verify(ontology_of(t), r);
}

// A revision admitted by kategoria: the map of kinds a functor of their orders, keeping registers, every fixed kind sent to itself and
// every declared kind reducing to the kind of the ground it reduced to (but for the roots a revision into levels recasts).
inline ko::Admission admit_revision(const Part& from, const Part& to) {
  ko::Conditions c;
  c.requirements.one_root = true;
  c.fixed = fixed_kinds(from, to);
  return ko::admit(ontology_of(from), ontology_of(to), mapping_of(from, to), c);
}

// A chronicle's acts as a record of facts: each act a fact of its kind, with its force.
inline ko::Record record_of(const Part& t, const std::vector<acts::Act>& record) {
  ko::Record r;
  for (std::size_t k = 0; k < record.size(); k++) {
    const auto p = primitive_of(t, record[k].kind);
    ko::Fact f;
    f.id = "act" + std::to_string(k + 1), f.kind = ident(record[k].kind), f.text = record[k].kind;
    if (const Primitive* q = t.primitive(p)) f.axes["force"] = std::string(acts::word(q->force));
    r.facts.push_back(std::move(f));
  }
  return r;
}

// A record carried along a revision, with what it renames and loses named.
inline ko::Migrated carry(const ko::Record& r, const Part& from, const Part& to) { return ko::migrate(r, ontology_of(from), ontology_of(to), mapping_of(from, to)); }

// Whether the record carried is, as a set-valued functor, the left Kan extension of the record along the map of kinds (kategoria).
inline ko::Verdict carried_as_extension(const ko::Record& r, const Part& from, const Part& to) {
  return ko::carried_as_extension(r, ontology_of(from), ontology_of(to), mapping_of(from, to));
}

}  // namespace thesmos::behaviour
