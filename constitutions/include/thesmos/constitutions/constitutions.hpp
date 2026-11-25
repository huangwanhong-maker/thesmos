// Constitutions as data: a community's order declared in Ostrom's seven kinds of rules (position, boundary, choice,
// aggregation, information, payoff, scope; Understanding Institutional Diversity, 2005, ch. 7) with bodies, scenes, conversions
// between registers, receiving capacity, forms of circulation, a route for claims, the custody of means, preserved properties,
// a rule of amendment for each level, and a policy of records.
//
// A constitution is read from its written form (noetarium.constitution/1, JSON read exactly by symploke) and admitted by form
// alone, under nine conditions (the design record of 2026-11-25, Section 3.2): (1) it is closed: every position, body, scene
// and rule it names exists; (2) every decision has a body, a rule and a declared unit of count, and the body can be filled;
// (3) every level has a rule of amendment, and what is entrenched is named; (4) the route for claims is declared; (5) every
// conversion between registers is listed; (6) the custody of means is declared, and every executable power is recognised by a
// rule or held as a declared reserve with its scope and term; (7) the properties preserved through amendments are declared;
// (8) receiving capacity and the forms of circulation are declared; (9) the policy of records is declared. Nothing here
// judges whether an order is open or closed, equal or directed.

#pragma once

#include <thesmos/kernel/kernel.hpp>

#include <symploke/jsonld/json.hpp>

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace thesmos::constitutions {

using Json = symploke::jsonld::Json;

struct Entry {
  std::string mode;  // "open", "decided", "lot", "sponsored", "after", or the text as written
  std::string decided_by, after;
  std::vector<std::string> among;  // the positions a lot is drawn among, or that sponsor
};
struct Boundary {
  bool has_entry = false;
  Entry entry;
  std::string exit;
};
struct Choice {
  std::string who = "anyone";
  std::vector<std::string> may, must, must_never, in;
  std::string when, within, or_else;  // for a duty: what calls for it, its term, and what follows if it is not done
};
struct Body {
  std::vector<std::string> of;
};
struct Decision {
  std::string body, rule, count_by, tempo;
};
struct Scene {
  std::vector<std::string> acts, read, write;
  bool read_anyone = false, write_anyone = false;
  std::string presence, tempo;
};
struct Conversion {
  std::string from, decided_by;
  std::vector<std::string> to_position;
  long long at_least = 1;
};
struct Claims {
  bool declared = false, has_route = false, outsiders = false;
  std::string route, decided_by, review;
};
struct Reserve {
  std::string holder, scope, term;
  std::vector<std::string> powers;
};
struct Custody {
  bool declared = false;
  std::map<std::string, std::vector<std::string>> powers;  // power -> positions that can exercise it
  std::vector<Reserve> reserve;
  long long key_threshold = 0;
  std::vector<std::string> key_of;
};
struct Amendment {
  std::map<std::string, std::string> by_level;  // level -> decision
  bool entrenched_declared = false;
  std::vector<std::string> entrenched;
};

struct Constitution {
  std::string format, name, purpose;
  std::map<std::string, std::vector<std::string>> positions;  // position -> kinds of party that may hold it
  std::map<std::string, Boundary> boundary;
  std::vector<Choice> choice;
  std::map<std::string, Body> bodies;
  std::map<std::string, Decision> aggregation;
  std::map<std::string, Scene> scenes;
  bool conversions_declared = false;
  std::map<std::string, Conversion> conversions;
  bool receiving_declared = false;
  std::string receiving_unit;
  long long receiving_per_round = 0;
  bool circulation_declared = false;
  std::vector<std::string> circulation;
  Claims claims;
  Custody custody;
  bool preserved_declared = false;
  std::vector<std::string> preserved;
  Amendment amendment;
  bool records_declared = false;
  std::vector<std::string> captured;
  std::vector<std::string> compacts;  // the compacts the community has ratified (its external relations)
  // Turns that must pass between a proposal and the act that decides it, by tempo (declared as "tempi", else these).
  std::map<std::string, long long> tempi = {{"operative", 0}, {"deliberative", 1}, {"constitutional", 3}};
};

inline const std::set<std::string>& kinds_of_party() {
  static const std::set<std::string> s = {"person", "organisation", "operative", "community"};
  return s;
}
inline const std::set<std::string>& units_of_count() {
  static const std::set<std::string> s = {"person", "origin", "operative", "community"};
  return s;
}
inline const std::set<std::string>& forms_of_circulation() {
  static const std::set<std::string> s = {"gift", "common provision", "reciprocal commitment", "mutual credit", "general claim", "outside money"};
  return s;
}
inline const std::vector<std::string>& levels() {
  static const std::vector<std::string> v = {"operational", "collective choice", "constitutional"};
  return v;
}

// ---- Reading ------------------------------------------------------------------------------------------------------------------

namespace detail {

inline std::string text(const Json* j) { return j && j->is_string() ? j->text : std::string(); }

// A string or a list of strings, as the written form allows both.
inline std::vector<std::string> strings(const Json* j) {
  std::vector<std::string> v;
  if (!j) return v;
  if (j->is_string()) v.push_back(j->text);
  else if (j->is_array())
    for (const Json& i : j->items)
      if (i.is_string()) v.push_back(i.text);
  return v;
}

inline bool anyone(const Json* j) { return j && j->is_string() && j->text == "anyone"; }

}  // namespace detail

// Reads a constitution from its written form. Text that is not JSON is refused with its line and column; parts of unexpected
// shape are read as absent, which admission then reports.
inline Constitution read(std::string_view written) {
  using detail::strings;
  using detail::text;
  const Json doc = symploke::jsonld::parse(written);
  if (!doc.is_object()) throw std::invalid_argument("thesmos::constitutions: a constitution is a JSON object");
  Constitution c;
  c.format = text(doc.get("format"));
  c.name = text(doc.get("name"));
  c.purpose = text(doc.get("purpose"));
  if (const Json* p = doc.get("positions"); p && p->is_object())
    for (const auto& [name, v] : p->members) c.positions[name] = strings(v.get("held by"));
  if (const Json* b = doc.get("boundary"); b && b->is_object())
    for (const auto& [name, v] : b->members) {
      Boundary bd;
      if (const Json* e = v.get("entry")) {
        bd.has_entry = true;
        if (e->is_string()) bd.entry.mode = e->text;
        else if (e->is_object()) {
          if (e->has("decided by")) bd.entry.mode = "decided", bd.entry.decided_by = text(e->get("decided by"));
          else if (e->has("by lot among")) bd.entry.mode = "lot", bd.entry.among = strings(e->get("by lot among"));
          else if (e->has("sponsored by")) bd.entry.mode = "sponsored", bd.entry.among = strings(e->get("sponsored by"));
          else if (e->has("after")) bd.entry.mode = "after", bd.entry.after = text(e->get("after"));
        }
      }
      if (const Json* x = v.get("exit")) bd.exit = x->is_string() ? x->text : "with notice";
      c.boundary[name] = bd;
    }
  if (const Json* ch = doc.get("choice"); ch && ch->is_array())
    for (const Json& r : ch->items) {
      Choice rule;
      if (const Json* w = r.get("who"); w && w->is_string()) rule.who = w->text;
      rule.may = strings(r.get("may"));
      rule.must = strings(r.get("must"));
      rule.must_never = strings(r.get("must never"));
      rule.in = strings(r.get("in"));
      rule.when = text(r.get("when"));
      rule.within = text(r.get("within"));
      rule.or_else = text(r.get("or else"));
      c.choice.push_back(rule);
    }
  if (const Json* b = doc.get("bodies"); b && b->is_object())
    for (const auto& [name, v] : b->members) c.bodies[name] = Body{strings(v.get("of"))};
  if (const Json* a = doc.get("aggregation"); a && a->is_object())
    for (const auto& [name, v] : a->members)
      c.aggregation[name] = Decision{text(v.get("body")), text(v.get("rule")), text(v.get("count by")), text(v.get("tempo"))};
  if (const Json* s = doc.get("scenes"); s && s->is_object())
    for (const auto& [name, v] : s->members) {
      Scene sc;
      sc.acts = strings(v.get("acts"));
      sc.read_anyone = detail::anyone(v.get("read"));
      sc.write_anyone = detail::anyone(v.get("write"));
      if (!sc.read_anyone) sc.read = strings(v.get("read"));
      if (!sc.write_anyone) sc.write = strings(v.get("write"));
      sc.presence = text(v.get("presence"));
      sc.tempo = text(v.get("tempo"));
      c.scenes[name] = sc;
    }
  if (const Json* cv = doc.get("conversions"); cv && cv->is_object()) {
    c.conversions_declared = true;
    for (const auto& [name, v] : cv->members) {
      Conversion k;
      k.from = text(v.get("from"));
      k.decided_by = text(v.get("decided by"));
      k.to_position = strings(v.get("to position"));
      if (const Json* n = v.get("at least"); n && n->is_number()) k.at_least = static_cast<long long>(n->number);
      c.conversions[name] = k;
    }
  }
  if (const Json* r = doc.get("receiving"); r && r->is_object() && r->has("unit") && r->has("per round")) {
    c.receiving_declared = true;
    c.receiving_unit = text(r->get("unit"));
    if (const Json* n = r->get("per round"); n->is_number()) c.receiving_per_round = static_cast<long long>(n->number);
  }
  if (const Json* f = doc.get("circulation"); f && f->is_array()) {
    c.circulation_declared = true;
    c.circulation = strings(f);
  }
  if (const Json* k = doc.get("claims"); k && k->is_object()) {
    c.claims.declared = true;
    c.claims.has_route = k->has("route");
    c.claims.route = text(k->get("route"));
    c.claims.decided_by = text(k->get("decided by"));
    c.claims.review = text(k->get("review"));
    if (const Json* o = k->get("outsiders"); o && o->is_bool()) c.claims.outsiders = o->truth;
  }
  if (const Json* k = doc.get("custody"); k && k->is_object() && k->has("powers")) {
    c.custody.declared = true;
    if (const Json* p = k->get("powers"); p->is_object())
      for (const auto& [power, holders] : p->members) c.custody.powers[power] = strings(&holders);
    if (const Json* r = k->get("reserve"); r && r->is_array())
      for (const Json& x : r->items) c.custody.reserve.push_back(Reserve{text(x.get("holder")), text(x.get("scope")), text(x.get("term")), strings(x.get("powers"))});
    if (const Json* keys = k->get("keys"); keys && keys->is_object()) {
      if (const Json* t = keys->get("threshold"); t && t->is_number()) c.custody.key_threshold = static_cast<long long>(t->number);
      c.custody.key_of = strings(keys->get("of"));
    }
  }
  if (const Json* p = doc.get("preserved"); p && p->is_array()) {
    c.preserved_declared = true;
    c.preserved = strings(p);
  }
  if (const Json* a = doc.get("amendment"); a && a->is_object())
    for (const auto& [key, v] : a->members) {
      if (key == "entrenched") {
        c.amendment.entrenched_declared = v.is_array();
        c.amendment.entrenched = strings(&v);
      } else if (v.is_string())
        c.amendment.by_level[key] = v.text;
    }
  if (const Json* r = doc.get("records"); r && r->is_object() && r->has("captured")) {
    c.records_declared = true;
    c.captured = strings(r->get("captured"));
  }
  if (const Json* x = doc.get("external"); x && x->is_object()) c.compacts = strings(x->get("compacts"));
  if (const Json* t = doc.get("tempi"); t && t->is_object())
    for (const auto& [name, v] : t->members)
      if (v.is_number()) c.tempi[name] = static_cast<long long>(v.number);
  return c;
}

// ---- Admission ----------------------------------------------------------------------------------------------------------------

struct Fault {
  int condition = 0;
  std::string reason;
};

namespace detail {
inline bool contains(const std::vector<std::string>& v, const std::string& x) { return std::find(v.begin(), v.end(), x) != v.end(); }
}  // namespace detail

// What each position, and anyone, is recognised to do by the choice rules (may and must).
inline std::map<std::string, std::set<std::string>> recognised(const Constitution& c) {
  std::map<std::string, std::set<std::string>> r;
  for (const Choice& rule : c.choice) {
    for (const auto& k : rule.may) r[rule.who].insert(k);
    for (const auto& k : rule.must) r[rule.who].insert(k);
  }
  return r;
}

inline std::vector<Fault> admission(const Constitution& c) {
  std::vector<Fault> faults;
  auto known = [&](const std::vector<std::string>& names, const std::string& where) {
    for (const auto& n : names)
      if (n != "anyone" && !c.positions.count(n)) faults.push_back({1, where + " names the position " + n + ", which the constitution lacks"});
  };
  // 1. Closed.
  for (const auto& [name, held] : c.positions)
    for (const auto& k : held)
      if (!kinds_of_party().count(k)) faults.push_back({1, "the position " + name + " is held by an unknown kind of party, " + k});
  for (const auto& [name, b] : c.boundary) {
    known({name}, "a boundary rule");
    if (b.entry.mode == "decided" && !c.aggregation.count(b.entry.decided_by))
      faults.push_back({1, "entry to " + name + " is decided by " + b.entry.decided_by + ", which has no rule"});
    if (b.entry.mode == "lot" || b.entry.mode == "sponsored") known(b.entry.among, "entry to " + name);
    if (b.entry.mode == "after" && (!c.conversions_declared || !c.conversions.count(b.entry.after)))
      faults.push_back({5, "entry to " + name + " follows the conversion " + b.entry.after + ", which is not listed"});
  }
  for (const Choice& rule : c.choice) {
    known({rule.who}, "a choice rule");
    for (const auto& s : rule.in)
      if (!c.scenes.count(s)) faults.push_back({1, "a choice rule names the scene " + s + ", which the constitution lacks"});
  }
  for (const auto& [name, body] : c.bodies) known(body.of, "the body " + name);
  for (const auto& [name, scene] : c.scenes) {
    known(scene.read, "the scene " + name);
    known(scene.write, "the scene " + name);
  }
  for (const auto& [name, k] : c.conversions) {
    known(k.to_position, "the conversion " + name);
    if (!c.aggregation.count(k.decided_by)) faults.push_back({1, "the conversion " + name + " is decided by a rule that is lacking"});
  }
  if (c.claims.declared) {
    for (const std::string* body : {&c.claims.route, &c.claims.review})
      if (!body->empty() && !c.bodies.count(*body)) faults.push_back({1, "claims name the body " + *body + ", which the constitution lacks"});
    if (!c.claims.decided_by.empty() && !c.aggregation.count(c.claims.decided_by)) faults.push_back({1, "claims are decided by a rule that is lacking"});
  }
  // 2. Every decision has a body, a rule and a unit of count, and the body can be filled.
  for (const auto& [name, d] : c.aggregation) {
    const auto body = c.bodies.find(d.body);
    if (body == c.bodies.end()) {
      faults.push_back({2, "the decision " + name + " has no body"});
      continue;
    }
    if (d.rule.empty()) faults.push_back({2, "the decision " + name + " has no rule"});
    if (!units_of_count().count(d.count_by)) faults.push_back({2, "the decision " + name + " declares no unit of count"});
    bool fillable = false;
    for (const auto& p : body->second.of) {
      const auto b = c.boundary.find(p);
      fillable = fillable || (b != c.boundary.end() && b->second.has_entry);
    }
    if (!fillable) faults.push_back({2, "the body " + d.body + " cannot be filled: none of its positions has a rule of entry"});
  }
  // 3. Each level has a rule of amendment; what is entrenched is named.
  for (const auto& level : levels()) {
    const auto it = c.amendment.by_level.find(level);
    if (it == c.amendment.by_level.end() || !c.aggregation.count(it->second)) faults.push_back({3, "the " + level + " level has no rule of amendment"});
  }
  if (!c.amendment.entrenched_declared) faults.push_back({3, "what is entrenched is not named"});
  // 4. The route for claims is declared (it may be declared empty).
  if (!c.claims.declared || !c.claims.has_route) faults.push_back({4, "the route for claims is not declared"});
  // 5. Every conversion between registers is listed.
  if (!c.conversions_declared) faults.push_back({5, "the conversions between registers are not listed"});
  // 6. The custody of means; executable powers recognised or held as a declared reserve.
  if (!c.custody.declared) faults.push_back({6, "the custody of means is not declared"});
  else {
    const auto rec = recognised(c);
    auto allowed = [&](const std::string& holder, const std::string& power) {
      for (const std::string& who : {holder, std::string("anyone")}) {
        const auto it = rec.find(who);
        if (it != rec.end() && it->second.count(power)) return true;
      }
      for (const Reserve& r : c.custody.reserve)
        if (r.holder == holder && !r.scope.empty() && !r.term.empty() && detail::contains(r.powers, power)) return true;
      return false;
    };
    for (const auto& [power, holders] : c.custody.powers)
      for (const auto& h : holders) {
        known({h}, "the custody of means");
        if (!allowed(h, power)) faults.push_back({6, h + " can " + power + " and no rule recognises it: overreach"});
      }
  }
  // 7. Preserved properties.
  if (!c.preserved_declared) faults.push_back({7, "the properties preserved through amendments are not declared"});
  // 8. Receiving capacity and the forms of circulation.
  if (!c.receiving_declared) faults.push_back({8, "receiving capacity is not declared"});
  if (!c.circulation_declared || c.circulation.empty()) faults.push_back({8, "the forms of circulation are not declared"});
  else
    for (const auto& f : c.circulation)
      if (!forms_of_circulation().count(f)) faults.push_back({8, f + " is not a form of circulation of the ground"});
  // 9. The policy of records.
  if (!c.records_declared) faults.push_back({9, "the policy of records is not declared"});
  return faults;
}

// ---- What the fold reads ------------------------------------------------------------------------------------------------------

// Whether some choice rule names this kind of act (may, must or must never). A kind no rule names is governed by its own
// conditions alone.
inline bool mentions(const Constitution& c, const std::string& kind) {
  for (const Choice& r : c.choice)
    if (detail::contains(r.may, kind) || detail::contains(r.must, kind) || detail::contains(r.must_never, kind)) return true;
  return false;
}
inline bool permits(const Constitution& c, const std::string& position, const std::string& kind) {
  for (const Choice& r : c.choice)
    if (r.who == position && (detail::contains(r.may, kind) || detail::contains(r.must, kind))) return true;
  return false;
}
inline bool forbids(const Constitution& c, const std::string& position, const std::string& kind) {
  for (const Choice& r : c.choice)
    if (r.who == position && detail::contains(r.must_never, kind)) return true;
  return false;
}
inline bool may_hold(const Constitution& c, const std::string& position, kernel::Kind k) {
  const auto it = c.positions.find(position);
  return it != c.positions.end() && detail::contains(it->second, std::string(kernel::word(k)));
}
inline long long wait(const Constitution& c, const std::string& tempo) {
  const auto it = c.tempi.find(tempo);
  return it == c.tempi.end() ? 0 : it->second;
}

// The report of recognised against executable powers (the Political Authority paper): for each party, what it may and can do,
// what it can do with no rule that empowers it (overreach), and what it is empowered to do without the means (unsupported).
struct Powers {
  std::vector<std::string> usable, overreach, unsupported;
};
inline std::map<std::string, Powers> powers(const std::map<std::string, std::set<std::string>>& recognised_by_position,
                                            const std::map<std::string, std::set<std::string>>& positions_held,
                                            const std::map<std::string, std::set<std::string>>& executable) {
  std::map<std::string, Powers> out;
  std::set<std::string> parties;
  for (const auto& [p, _] : positions_held) parties.insert(p);
  for (const auto& [p, _] : executable) parties.insert(p);
  for (const auto& party : parties) {
    std::set<std::string> may, can;
    if (const auto h = positions_held.find(party); h != positions_held.end())
      for (const auto& pos : h->second)
        if (const auto r = recognised_by_position.find(pos); r != recognised_by_position.end()) may.insert(r->second.begin(), r->second.end());
    if (const auto e = executable.find(party); e != executable.end()) can = e->second;
    Powers& w = out[party];
    std::set_intersection(may.begin(), may.end(), can.begin(), can.end(), std::back_inserter(w.usable));
    std::set_difference(can.begin(), can.end(), may.begin(), may.end(), std::back_inserter(w.overreach));
    std::set_difference(may.begin(), may.end(), can.begin(), can.end(), std::back_inserter(w.unsupported));
  }
  return out;
}

}  // namespace thesmos::constitutions
