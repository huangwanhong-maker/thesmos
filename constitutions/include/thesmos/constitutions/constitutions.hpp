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
//
// A constitution may also declare its holdings and the norms of access to them (thesmos's holdings, decision 0006). Their form is
// held within two of the nine conditions: every norm names what the constitution declares (closure, condition 1), and every holding
// is kept by a declared office or a named party (the custody of means, condition 6).
//
// A constitution may declare its part "behaviour" (decision 0008): kinds declared by counts-as, features and categories of conduct,
// counts-as rules, trigger rules and standards of recognition, extending the society's base taxonomy. Their form is held within
// closure (condition 1): each declaration's faults (behaviour::faults), and the scenes and positions they name declared. A
// constitution that declares no part has the ground's kinds and the base's in force, as before.
//
// Since decision 0013 a decision may count by provider or by declared weight, its rule may be a threshold at an exact fraction ("at least
// 3/5", "more than 1/2") or a double majority, written {"double majority": {"members": "1/2", "weight": "2/3"}} and read as its canonical
// text; a count by weight declares its weights ("weights": {"<member>": 3, ...}, whole numbers or fractions written "p/q"), and a double
// majority counts by weight (condition 2). The compacts a community has ratified are records (the part "external", "compacts": names, or
// {"id", "reservations", "since", "terms"}), each with its reservations held within closure (condition 1).
//
// Since decision 0014 a constitution may declare its order, a label from an open vocabulary ("order": "polity", "treaty organisation",
// ...), which the profile shows and registries read and which neither admission nor the fold reads; and in its part "external" the
// registries it is listed in ("listed in") and its envoys written as objects ({"to", "holder", "position"}), data of the same kind. A
// position may declare a term in turns ("term": 20), read as the trigger rule that ends a position held for more than that many turns, an
// effect by law (term_triggers); a body may declare its seats ("seats": 5), and the fold refuses any act that would give the body more
// members than its seats. A term or seats that is no whole number of at least one is a fault (conditions 1 and 2).

#pragma once

#include <thesmos/behaviour/behaviour.hpp>
#include <thesmos/compacts/compacts.hpp>
#include <thesmos/holdings/holdings.hpp>
#include <thesmos/procedures/procedures.hpp>
#include <thesmos/work/work.hpp>
#include <thesmos/kernel/kernel.hpp>

#include <symploke/jsonld/json.hpp>

#include <algorithm>
#include <optional>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace thesmos::constitutions {

using Json = symploke::jsonld::Json;

// A condition on which an application is decided by rule (decision 0011): the holders of the position fewer than a number, or the
// applicant presented a passport issued by a community; since decision 0012 some posted work of a kind open, or some open work whose
// review the applicant's provider may give; an entry's conditions hold together.
struct Condition {
  std::string what;  // "holders fewer than", "passport from", "work open of kind", "work open for the applicant's provider"
  long long n = 0;
  std::string value;
};
struct Entry {
  std::string mode;  // "open", "decided", "lot", "sponsored", "after", "application", "assigned", or the text as written
  std::string decided_by, after;
  std::vector<std::string> among;  // the positions a lot is drawn among, or that sponsor
  std::string office;              // "assigned": the office that assigns (decision 0011)
  std::vector<Condition> conditions;  // "application": decided by rule when they hold together (decision 0011)
  std::vector<std::string> unread;    // conditions that do not read
};
struct Boundary {
  bool has_entry = false;
  Entry entry;
  std::string exit;
  std::string exit_office;  // exit "by the office": the office that ends the position (decision 0011)
  long long term = 0;       // the turns a holder holds it, after which the position ends by law; 0 for none (decision 0014)
  std::string term_unread;  // why a term written could not be read
};
// A route for a type of item (decision 0011): the body that answers it, its term, the body that reviews its answers, and for questions
// the subjects they may be about. Declared empty, it is a route that reaches no body, and its items are refused with the declaration.
struct Route {
  bool declared = false, empty = false;
  std::string body, review, term;
  std::vector<std::string> subjects;
};
// A route for petitions of one kind: met by a deposit checked within a holding that declares review; adopted by a decision (named, or
// the rule of amendment at a level); or carried by an office to another community.
struct PetitionRoute {
  std::string by, within, level, to, carrier;
};
struct Routes {
  bool declared = false;
  Route applications, questions, claims;
  bool petitions_declared = false;
  std::map<std::string, PetitionRoute> petitions;
  std::map<std::string, Route> types;  // the types of item a community declares, beside application, question and petition
  std::vector<std::string> unread;
};
inline const std::set<std::string>& subjects_of_questions() {
  static const std::set<std::string> s = {"norm", "rule", "holding", "work", "term", "route", "position"};
  return s;
}
// Reads an entry's conditions: "holders fewer than <n>" and "passport from <community>", joined by " and ".
inline std::vector<Condition> read_conditions(const std::string& text, std::vector<std::string>& unread) {
  std::vector<Condition> out;
  if (text.empty()) return out;
  std::size_t at = 0;
  while (at <= text.size()) {
    const auto next = text.find(" and ", at);
    const std::string atom = text.substr(at, next == std::string::npos ? std::string::npos : next - at);
    if (atom.rfind("holders fewer than ", 0) == 0) {
      const std::string n = atom.substr(19);
      if (!n.empty() && n.size() <= 9 && n.find_first_not_of("0123456789") == std::string::npos) out.push_back({"holders fewer than", std::stoll(n), {}});
      else unread.push_back("the condition \"" + atom + "\" names no number");
    } else if (atom.rfind("passport from ", 0) == 0 && atom.size() > 14)
      out.push_back({"passport from", 0, atom.substr(14)});
    else if (atom.rfind("work open of kind ", 0) == 0 && atom.size() > 18)  // decision 0012
      out.push_back({"work open of kind", 0, atom.substr(18)});
    else if (atom == "work open for the applicant's provider")
      out.push_back({"work open for the applicant's provider", 0, {}});
    else
      unread.push_back("the condition \"" + atom + "\" is none the fold reads: holders fewer than <n>, passport from <community>, work open of kind <k>, "
                       "work open for the applicant's provider");
    if (next == std::string::npos) break;
    at = next + 5;
  }
  return out;
}
struct Choice {
  std::string who = "anyone";
  std::vector<std::string> may, must, must_never, in;
  std::string when, within, or_else;  // for a duty: what calls for it, its term, and what follows if it is not done
};
struct Body {
  std::vector<std::string> of;
  long long seats = 0;       // the most members it may have, counted as the parties holding one of its positions; 0 for no bound (decision 0014)
  std::string seats_unread;  // why seats written could not be read
};
// An envoy of the community as its part "external" writes it (decision 0014): a position whose holders are its envoys, as older records
// wrote it, or an object naming the community it is sent to, the party that holds the mission and the position. Data for the profile and
// for registries; the fold reads none of it.
struct Envoy {
  std::string to, holder, position;
};
struct Decision {
  std::string body, rule, count_by, tempo;  // the rule as its canonical text (a double majority written as an object read into it)
  procedures::Weights weights;              // for a count by weight: each member's declared weight (decision 0013)
  std::vector<std::string> unread;          // what of it could not be read
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
// The part "passports" (decision 0009): from whom the community accepts passports, what it requires of their holders, and the office
// whose reports of other communities' heads and of foreign mandates' status the fold reads.
struct Passports {
  bool declared = false;
  std::string accepted_from;              // "witnessed" (a notary it recognises at the level of evidence witnessed the issuer's head),
                                          // "recognised" (it recognises the issuer at the level of evidence), or "named"
  std::vector<std::string> named;         // the issuers it accepts by name
  std::vector<std::string> require;       // of the holder: "provider", "model"
  std::string status_by;                  // the position whose reports of heads and statuses the fold reads
};
struct Amendment {
  std::map<std::string, std::string> by_level;  // level -> decision
  bool entrenched_declared = false;
  std::vector<std::string> entrenched;
};

struct Constitution {
  std::string format, name, purpose;
  std::string order;                   // its order, a label the profile shows and registries read, and the fold never reads (decision 0014)
  std::vector<std::string> listed_in;  // the registries it says it is listed in (the part "external", decision 0014)
  std::vector<Envoy> envoys;           // its envoys as the part "external" writes them (decision 0014)
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
  std::vector<compacts::Ratification> compacts;  // the compacts the community has ratified, with their reservations and withdrawals (decision 0013)
  // Who may read each part of the constitution, as its policy of records declares ("records": {"visibility": {part: …}}): public
  // unless declared members' or an office's; the purpose, the boundary rules and the route for claims are always public.
  std::map<std::string, thesmos::holdings::Visibility> visibility;
  std::vector<std::string> unread_visibility;
  thesmos::holdings::Register holdings;  // its holdings at the founding and the norms of access to them (read from "holdings" and "access")
  Passports passports;  // from whom it accepts passports (decision 0009)
  Routes routes;        // a route for every directive (decision 0011)
  thesmos::work::Kinds work;  // the kinds of work it posts, their life declared as data (decision 0012)
  // Its part "behaviour" as written, and the taxonomy in force: the ground's and the base's, with the part's declarations (decision 0008).
  bool behaviour_declared = false;
  thesmos::behaviour::Part behaviour_written;
  thesmos::behaviour::Part behaviour = thesmos::behaviour::in_force({}, false);
  // Turns that must pass between a proposal and the act that decides it, by tempo (declared as "tempi", else these).
  std::map<std::string, long long> tempi = {{"operative", 0}, {"deliberative", 1}, {"constitutional", 3}};
};

inline const std::set<std::string>& kinds_of_party() {
  static const std::set<std::string> s = {"person", "organisation", "operative", "community"};
  return s;
}
inline const std::set<std::string>& units_of_count() {
  static const std::set<std::string> s = {"person", "origin", "operative", "community", "provider", "weight"};
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
// The parts of a constitution, and of the community's state, whose visibility a policy of records may declare: the parts of the
// written form, who holds which position ("holders") and the work posted ("work").
inline const std::set<std::string>& parts() {
  static const std::set<std::string> s = {"purpose", "positions", "holders", "boundary", "choice", "bodies", "aggregation", "scenes", "conversions", "receiving",
                                          "circulation", "claims", "custody", "preserved", "amendment", "records", "external", "holdings", "access", "work", "behaviour",
                                          "passports", "routes", "rulings"};
  return s;
}
// The parts always public: what an outsider needs to judge whether to join, how to bring a claim (G8), from whom the community
// accepts passports (decision 0009), and where each directive goes (decision 0011).
inline const std::set<std::string>& always_public() {
  static const std::set<std::string> s = {"purpose", "boundary", "claims", "passports", "routes"};
  return s;
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

// A whole number of at least one, written as a JSON number (a term in turns, seats; decision 0014); nothing otherwise.
inline std::optional<long long> at_least_one(const Json& j) {
  if (!j.is_number() || !(j.number >= 1) || j.number > 1e15 || j.number != static_cast<double>(static_cast<long long>(j.number))) return std::nullopt;
  return static_cast<long long>(j.number);
}

// A weight: a whole number, or a fraction written "p/q"; never negative (decision 0013).
inline std::optional<procedures::Rational> weight(const Json& j) {
  if (j.is_number() && j.number >= 0 && j.number == static_cast<double>(static_cast<long long>(j.number))) return procedures::Rational(static_cast<long long>(j.number));
  if (!j.is_string() || j.text.empty() || j.text.find_first_not_of("0123456789/") != std::string::npos || j.text.front() == '/' || j.text.back() == '/' ||
      std::count(j.text.begin(), j.text.end(), '/') > 1)
    return std::nullopt;
  try {
    return procedures::Rational::parse(j.text);
  } catch (const std::exception&) {
    return std::nullopt;
  }
}
// A decision's weights read from {"<member>": <weight>, ...}, what fails noted.
inline procedures::Weights weights(const Json* j, std::vector<std::string>& unread, const std::string& where) {
  procedures::Weights out;
  if (!j) return out;
  if (!j->is_object()) {
    unread.push_back(where + " declares its weights as an object of members");
    return out;
  }
  for (const auto& [member, x] : j->members) {
    if (const auto w = weight(x)) out[member] = *w;
    else unread.push_back(where + " gives " + member + " a weight that is no whole number or fraction p/q");
  }
  return out;
}
// A decision's rule: a string, or a double majority written as an object, read into its canonical text.
inline std::string rule(const Json* j, std::vector<std::string>& unread, const std::string& where) {
  if (!j) return {};
  if (j->is_string()) return j->text;
  const Json* d = j->is_object() ? j->get("double majority") : nullptr;
  if (d && d->is_object())
    if (const auto t = procedures::double_majority(text(d->get("members")), text(d->get("weight")))) return *t;
  unread.push_back(where + " writes its rule as an object that is no double majority of members and weight at fractions p/q");
  return {};
}

}  // namespace detail

// The ratification of a compact that stands for the community at a turn, or nothing (decision 0013).
inline const compacts::Ratification* ratification(const Constitution& c, const std::string& id, long long turn) {
  for (auto it = c.compacts.rbegin(); it != c.compacts.rend(); ++it)
    if (it->id == id && it->stands(turn)) return &*it;
  return nullptr;
}
// The compacts whose ratification stands at a turn, in the order of ratification.
inline std::vector<std::string> compact_ids(const Constitution& c, long long turn) {
  std::vector<std::string> out;
  for (const auto& r : c.compacts)
    if (r.stands(turn) && std::find(out.begin(), out.end(), r.id) == out.end()) out.push_back(r.id);
  return out;
}

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
  c.order = text(doc.get("order"));  // decision 0014: a label, read as absent when it is not text
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
          else if (e->has("assigned by")) bd.entry.mode = "assigned", bd.entry.office = text(e->get("assigned by"));  // decision 0011
          else if (const Json* a = e->get("on application")) {
            bd.entry.mode = "application";
            const Json* w = a->is_object() ? a->get("while") : nullptr;
            bd.entry.conditions = read_conditions(w && w->is_string() ? w->text : std::string(), bd.entry.unread);
          }
        }
      }
      if (const Json* x = v.get("exit")) {
        if (x->is_object() && x->has("by the office")) bd.exit = "by the office", bd.exit_office = text(x->get("by the office"));
        else bd.exit = x->is_string() ? x->text : "with notice";
      }
      if (const Json* t = v.get("term")) {  // decision 0014
        if (const auto n = detail::at_least_one(*t)) bd.term = *n;
        else bd.term_unread = "the term of the position " + name + " is no whole number of turns of at least one";
      }
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
    for (const auto& [name, v] : b->members) {
      Body body{strings(v.get("of"))};
      if (const Json* n = v.get("seats")) {  // decision 0014
        if (const auto k = detail::at_least_one(*n)) body.seats = *k;
        else body.seats_unread = "the seats of the body " + name + " are no whole number of at least one";
      }
      c.bodies[name] = body;
    }
  if (const Json* a = doc.get("aggregation"); a && a->is_object())
    for (const auto& [name, v] : a->members) {
      Decision d{text(v.get("body")), {}, text(v.get("count by")), text(v.get("tempo"))};
      d.rule = detail::rule(v.get("rule"), d.unread, "the decision " + name);       // decision 0013
      d.weights = detail::weights(v.get("weights"), d.unread, "the decision " + name);
      c.aggregation[name] = d;
    }
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
  if (const Json* r = doc.get("records"); r && r->is_object())
    if (const Json* v = r->get("visibility"); v && v->is_object())
      for (const auto& [part, x] : v->members) c.visibility[part] = holdings::read_visibility(&x, c.unread_visibility, "the visibility of " + part, holdings::Visibility::everyone());
  if (const Json* x = doc.get("external"); x && x->is_object()) {  // the registries it is listed in and its envoys, data (decision 0014)
    c.listed_in = strings(x->get("listed in"));
    if (const Json* e = x->get("envoys"); e && e->is_array())
      for (const Json& k : e->items) {
        if (k.is_string()) c.envoys.push_back(Envoy{{}, {}, k.text});
        else if (k.is_object()) c.envoys.push_back(Envoy{text(k.get("to")), text(k.get("holder")), text(k.get("position"))});
      }
  }
  if (const Json* x = doc.get("external"); x && x->is_object())  // the compacts ratified at the founding, by name or as records (decision 0013)
    if (const Json* list = x->get("compacts"); list && list->is_array())
      for (const Json& k : list->items) {
        compacts::Ratification r;
        r.party = c.name;
        if (k.is_string()) r.id = k.text;
        else if (k.is_object()) {
          r.id = text(k.get("id"));
          if (const Json* s = k.get("since"); s && s->is_number() && s->number >= 0) r.since = static_cast<long long>(s->number);
          if (const Json* t = k.get("terms"); t && t->is_object()) r.has_terms = true, r.terms = compacts::read(*t), r.digest = compacts::digest(r.terms);
          for (const auto& a : strings(k.get("reservations"))) r.reservations.push_back({a, r.since, -1});
        } else
          continue;
        c.compacts.push_back(std::move(r));
      }
  c.holdings = holdings::read(doc.get("holdings"), doc.get("access"));
  if (const Json* x = doc.get("passports"); x && x->is_object()) {
    c.passports.declared = true;
    if (const Json* a = x->get("accepted from")) {
      if (a->is_string()) c.passports.accepted_from = a->text;
      else if (a->is_array()) c.passports.accepted_from = "named", c.passports.named = strings(a);
    }
    c.passports.require = strings(x->get("require"));
    c.passports.status_by = text(x->get("status by"));
  }
  c.work = work::read(doc.get("work"));  // decision 0012
  if (const Json* r = doc.get("routes")) {  // decision 0011
    c.routes.declared = true;
    auto route = [&](const Json* x, const std::string& label) {
      Route out;
      if (!x) return out;
      out.declared = true;
      if (x->is_null() || (x->is_string() && x->text == "none")) {
        out.empty = true;
        return out;
      }
      if (!x->is_object()) {
        c.routes.unread.push_back("the route for " + label + " is neither an object nor declared empty (\"none\")");
        return out;
      }
      out.body = text(x->get("body")), out.review = text(x->get("review")), out.subjects = strings(x->get("subjects"));
      if (const Json* term = x->get("term")) {
        if (term->is_number() && !term->lexical.empty() && term->lexical.find_first_not_of("0123456789") == std::string::npos) out.term = term->lexical;
        else if (term->is_string()) out.term = term->text;
        else c.routes.unread.push_back("the term of the route for " + label + " is neither a number of turns nor a tempo");
      }
      return out;
    };
    if (!r->is_object()) c.routes.unread.push_back("the routes are not an object");
    else
      for (const auto& [key, x] : r->members) {
        if (key == "applications") c.routes.applications = route(&x, key);
        else if (key == "questions") c.routes.questions = route(&x, key);
        else if (key == "claims") c.routes.claims = route(&x, key);
        else if (key == "petitions") {
          c.routes.petitions_declared = true;
          if (x.is_object())
            for (const auto& [kind, y] : x.members) {
              PetitionRoute q;
              q.by = text(y.get("by")), q.within = text(y.get("within")), q.level = text(y.get("level")), q.to = text(y.get("to")), q.carrier = text(y.get("carrier"));
              c.routes.petitions[kind] = q;
            }
          else if (!x.is_null() && !(x.is_string() && x.text == "none"))
            c.routes.unread.push_back("the routes for petitions are neither an object nor declared empty");
        } else
          c.routes.types[key] = route(&x, key);
      }
  }
  if (const Json* b = doc.get("behaviour")) {
    c.behaviour_declared = true;
    c.behaviour_written = behaviour::read_part(*b);
    c.behaviour = behaviour::in_force(c.behaviour_written, true);
  }
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
    if (!b.term_unread.empty()) faults.push_back({1, b.term_unread});  // decision 0014
    if (b.entry.mode == "decided" && !c.aggregation.count(b.entry.decided_by))
      faults.push_back({1, "entry to " + name + " is decided by " + b.entry.decided_by + ", which has no rule"});
    if (b.entry.mode == "lot" || b.entry.mode == "sponsored") known(b.entry.among, "entry to " + name);
    if (b.entry.mode == "after" && (!c.conversions_declared || !c.conversions.count(b.entry.after)))
      faults.push_back({5, "entry to " + name + " follows the conversion " + b.entry.after + ", which is not listed"});
    // Entry by assignment and by application, and exit by the office (decision 0011).
    if (b.entry.mode == "assigned" && !c.positions.count(b.entry.office))
      faults.push_back({1, "entry to " + name + " is assigned by " + (b.entry.office.empty() ? std::string("no office") : b.entry.office) + ", which the constitution lacks"});
    for (const auto& u : b.entry.unread) faults.push_back({1, "entry to " + name + " on application: " + u});
    if (b.exit == "by the office" && !c.positions.count(b.exit_office))
      faults.push_back({1, "exit from " + name + " is by the office " + (b.exit_office.empty() ? std::string("none named") : b.exit_office) + ", which the constitution lacks"});
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
  // The kinds of work, within closure (decision 0012): who posts, takes, checks, reviews and accepts declared, their holdings held.
  if (c.work.declared) {
    work::Names names;
    for (const auto& [p, kinds] : c.positions) names.positions.insert(p);
    for (const auto& [h, x] : c.holdings.holdings) names.holdings.insert(h);
    for (const auto& [d, x] : c.aggregation) names.decisions.insert(d);
    for (const auto& f : work::faults(c.work, names)) faults.push_back({1, f});
  }
  // The compacts ratified (decision 0013): each named, its terms standing and its own, its reservations admitted by them, and no compact
  // ratified twice over at once.
  {
    std::set<std::string> open;
    for (const auto& r : c.compacts) {
      if (r.id.empty()) faults.push_back({1, "a compact is ratified by no name"});
      if (r.has_terms) {
        for (const auto& f : compacts::faults(r.terms)) faults.push_back({1, "the terms of the compact " + r.id + " do not stand: " + f});
        if (r.terms.id != r.id) faults.push_back({1, "the compact " + r.id + " carries the terms of " + r.terms.id});
      }
      for (const auto& f : compacts::reservation_faults(r)) faults.push_back({1, f});
      if (r.withdrawn < 0 && !open.insert(r.id).second) faults.push_back({1, "the compact " + r.id + " is ratified twice over"});
    }
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
  // 2. Every decision has a body, a rule and a unit of count, and the body can be filled; seats written are a whole number (decision 0014).
  for (const auto& [name, body] : c.bodies)
    if (!body.seats_unread.empty()) faults.push_back({2, body.seats_unread});
  for (const auto& [name, d] : c.aggregation) {
    const auto body = c.bodies.find(d.body);
    if (body == c.bodies.end()) {
      faults.push_back({2, "the decision " + name + " has no body"});
      continue;
    }
    for (const auto& u : d.unread) faults.push_back({2, u});
    const auto rule = procedures::read_rule(d.rule);
    if (d.rule.empty()) faults.push_back({2, "the decision " + name + " has no rule"});
    else if (!rule) faults.push_back({2, "the decision " + name + " has the rule " + d.rule + ", which the kernel does not know"});
    if (!units_of_count().count(d.count_by)) faults.push_back({2, "the decision " + name + " declares no unit of count"});
    // A count by weight declares a positive weight for some member, and a double majority counts by weight (decision 0013).
    bool weighed = false;
    for (const auto& [member, w] : d.weights) weighed = weighed || procedures::Rational(0) < w;
    if (d.count_by == "weight" && !weighed) faults.push_back({2, "the decision " + name + " counts by weight and declares no positive weight"});
    if (rule && rule->form == procedures::Rule::Form::double_majority && d.count_by != "weight")
      faults.push_back({2, "the decision " + name + " is a double majority of members and weight, and counts by " + (d.count_by.empty() ? std::string("nothing") : d.count_by)});
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
  // 4. The route for claims is declared (it may be declared empty); and, where the constitution declares its routes, a route for every
  // directive (G8', decision 0011): applications, questions, petitions and every type of item its declared kinds open, each declared
  // (empty, if so), its bodies, decisions, holdings and offices declared, and the routes' claims agreeing with the part claims.
  if (!c.claims.declared || !c.claims.has_route) faults.push_back({4, "the route for claims is not declared"});
  if (c.routes.declared) {
    const auto& r = c.routes;
    for (const auto& u : r.unread) faults.push_back({4, u});
    auto body = [&](const std::string& b, const std::string& where) {
      if (!b.empty() && !c.bodies.count(b)) faults.push_back({4, where + " names the body " + b + ", which the constitution lacks"});
    };
    auto term = [&](const Route& x, const std::string& where) {
      if (!x.term.empty() && x.term.find_first_not_of("0123456789") != std::string::npos && !c.tempi.count(x.term))
        faults.push_back({4, where + " has the term " + x.term + ", neither a number of turns nor a declared tempo"});
    };
    if (!r.applications.declared) faults.push_back({4, "the routes declare no route for applications (G8')"});
    body(r.applications.review, "the route for applications"), term(r.applications, "the route for applications");
    if (!r.questions.declared) faults.push_back({4, "the routes declare no route for questions (G8')"});
    else if (!r.questions.empty) {
      if (r.questions.body.empty()) faults.push_back({4, "the route for questions names no body that answers them"});
      body(r.questions.body, "the route for questions"), body(r.questions.review, "the route for questions"), term(r.questions, "the route for questions");
      for (const auto& s : r.questions.subjects)
        if (!subjects_of_questions().count(s)) faults.push_back({4, "questions may be about " + s + ", which is no subject the ground knows"});
    }
    if (!r.petitions_declared) faults.push_back({4, "the routes declare no route for petitions (G8')"});
    for (const auto& [kind, q] : r.petitions) {
      const std::string where = "the route for petitions of kind " + kind;
      if (q.by == "deposit") {
        const auto h = c.holdings.holdings.find(q.within);
        if (h == c.holdings.holdings.end() || h->second.review.empty())
          faults.push_back({4, where + " is met by a deposit within " + (q.within.empty() ? std::string("no holding") : q.within) + ", which must be a holding that declares review"});
      } else if (q.by == "amendment") {
        if (!c.amendment.by_level.count(q.level.empty() ? "operational" : q.level)) faults.push_back({4, where + " is decided by amendment at a level that has no rule"});
      } else if (q.by == "carried") {
        if (!c.positions.count(q.carrier) || q.to.empty()) faults.push_back({4, where + " is carried by an office to a community, and names " + (q.carrier.empty() ? std::string("no office") : q.carrier)});
      } else if (!c.aggregation.count(q.by))
        faults.push_back({4, where + " is met by " + (q.by.empty() ? std::string("nothing") : q.by) + ": a deposit, amendment, carried, or a decision the constitution declares"});
    }
    if (r.claims.declared && !r.claims.empty && (r.claims.body != c.claims.route || r.claims.review != c.claims.review))
      faults.push_back({4, "the routes restate the route for claims otherwise than the part claims"});
    for (const auto& [type, x] : r.types) {
      if (!x.empty && x.body.empty()) faults.push_back({4, "the route for items of type " + type + " names no body"});
      body(x.body, "the route for items of type " + type), body(x.review, "the route for items of type " + type), term(x, "the route for items of type " + type);
    }
    // Every type of item a declared kind opens has its route.
    for (const auto& d : c.behaviour.declared) {
      const auto f = d.fixed.find("type");
      if (behaviour::primitive_of(c.behaviour, d.kind) != "request" || f == d.fixed.end()) continue;
      const std::string& type = f->second;
      if (type != "application" && type != "question" && type != "petition" && !r.types.count(type))
        faults.push_back({4, "the declared kind " + d.kind + " opens items of type " + type + ", for which the routes declare no route (G8')"});
    }
  }
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
  // 9. The policy of records, with the visibility of its parts: each part named exists, each office shown to is declared, and what
  // an outsider needs to join and to claim stays public (the route for claims under condition 4, G8).
  if (!c.records_declared) faults.push_back({9, "the policy of records is not declared"});
  for (const auto& u : c.unread_visibility) faults.push_back({9, u});
  for (const auto& [part, v] : c.visibility) {
    if (!parts().count(part)) faults.push_back({1, "the policy of records shows the part " + part + ", which a constitution lacks"});
    for (const auto& o : v.offices)
      if (!c.positions.count(o)) faults.push_back({1, "the policy of records shows " + part + " to the office " + o + ", which the constitution lacks"});
    if (always_public().count(part) && v.level != holdings::Visibility::Level::everyone)
      faults.push_back({part == "claims" ? 4 : 9, part == "passports"
                                                     ? std::string("the part passports of a community is public: an outsider reads it to judge whether it may present a passport")
                                                     : "the " + part + " of a community is public: an outsider reads it to judge whether to join and how to claim"});
  }
  // Holdings and the norms of access, within closure (1) and the custody of means (6).
  for (const auto& f : holdings::faults(c.holdings, c.positions)) faults.push_back({f.condition, f.reason});
  // The part passports, within closure (1): an acceptance the ground knows, requirements it knows, and a declared office of status.
  if (c.passports.declared) {
    const auto& x = c.passports;
    if (x.accepted_from != "witnessed" && x.accepted_from != "recognised" && !(x.accepted_from == "named" && !x.named.empty()))
      faults.push_back({1, "passports are accepted from issuers witnessed, recognised or named, and the constitution says none of these"});
    for (const auto& r : x.require)
      if (r != "provider" && r != "model") faults.push_back({1, "passports require of their holders a provider or a model, not " + r});
    if (x.status_by.empty() || !c.positions.count(x.status_by)) faults.push_back({1, "the reports of heads and statuses are made by an office the constitution lacks: " + (x.status_by.empty() ? std::string("none named") : x.status_by)});
    else {  // the office may report: no choice rule names the kind, or one empowers the office (or anyone), and none forbids the office
      bool named = false, may = false, never = false;
      for (const Choice& r : c.choice) {
        const bool here = detail::contains(r.may, "report") || detail::contains(r.must, "report");
        named = named || here || detail::contains(r.must_never, "report");
        if (r.who == x.status_by || r.who == "anyone") may = may || here;
        if (r.who == x.status_by) never = never || detail::contains(r.must_never, "report");
      }
      if ((named && !may) || never)
        faults.push_back({1, "the office of status, " + x.status_by + ", may not report under the choice rules, so no head or status it found could be read"});
    }
  }
  // The part behaviour, within closure (1): its own faults of form, and the scenes and positions it names declared.
  if (c.behaviour_declared) {
    for (const auto& f : behaviour::faults(c.behaviour)) faults.push_back({1, f});
    for (const auto& d : c.behaviour_written.declared) {
      for (const auto& s : d.in)
        if (!c.scenes.count(s)) faults.push_back({1, "the declared kind " + d.kind + " is made in the scene " + s + ", which the constitution lacks"});
      for (const auto& w : d.valid_when)
        for (const auto& a : w.atoms)
          if (a.op == behaviour::Atom::Op::holds && !c.positions.count(a.values.front()))
            faults.push_back({1, "a condition of " + d.kind + " names the position " + a.values.front() + ", which the constitution lacks"});
    }
    for (const auto& g : c.behaviour_written.triggers)
      if (!g.position.empty() && !c.positions.count(g.position)) faults.push_back({1, "the trigger rule " + g.rule + " names the position " + g.position + ", which the constitution lacks"});
  }
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
// The visibility of a part of the constitution or of the community's state: as declared, else public.
inline holdings::Visibility shown(const Constitution& c, const std::string& part) {
  const auto it = c.visibility.find(part);
  return it == c.visibility.end() || always_public().count(part) ? holdings::Visibility::everyone() : it->second;
}
inline bool may_hold(const Constitution& c, const std::string& position, kernel::Kind k) {
  const auto it = c.positions.find(position);
  return it != c.positions.end() && detail::contains(it->second, std::string(kernel::word(k)));
}
inline long long wait(const Constitution& c, const std::string& tempo) {
  const auto it = c.tempi.find(tempo);
  return it == c.tempi.end() ? 0 : it->second;
}

// The trigger rules a constitution's terms are shorthand for (decision 0014): for each position with a term of n turns, "a position held
// for more than n turns ends", an effect by law of the ground (Definition 34 of the theory note), so that a term adds nothing a declared
// trigger rule could not do.
inline std::vector<behaviour::Trigger> term_triggers(const Constitution& c) {
  std::vector<behaviour::Trigger> out;
  for (const auto& [name, b] : c.boundary)
    if (b.term > 0) {
      behaviour::Trigger g;
      g.rule = "the term of the position " + name, g.then = "end position", g.position = name, g.held_for = b.term;
      out.push_back(std::move(g));
    }
  return out;
}
// The trigger rules in force: those of the taxonomy in force, and those the terms are shorthand for.
inline std::vector<behaviour::Trigger> triggers(const Constitution& c) {
  std::vector<behaviour::Trigger> out = c.behaviour.triggers;
  for (auto& g : term_triggers(c)) out.push_back(std::move(g));
  return out;
}

// The members of a body: the parties holding one of its positions, given who holds which (party, position).
template <class Positions>
inline std::set<std::string> members(const Positions& held, const Body& body) {
  std::set<std::string> out;
  for (const auto& [key, since] : held)
    if (detail::contains(body.of, key.second)) out.insert(key.first);
  return out;
}
// Why the positions entered (party, position) would give a body more members than its seats, or nothing (decision 0014). With
// `amending`, the constitution is the one an amendment would put in force, nothing is entered, and any body beyond its seats is named.
template <class Positions>
inline std::string beyond_seats(const Positions& held, const Constitution& c, const std::vector<std::pair<std::string, std::string>>& entering, bool amending = false) {
  for (const auto& [name, body] : c.bodies) {
    if (body.seats <= 0) continue;
    std::set<std::string> m = members(held, body);
    const std::size_t before = m.size();
    for (const auto& [party, position] : entering)
      if (detail::contains(body.of, position)) m.insert(party);
    if (m.size() > static_cast<std::size_t>(body.seats) && (amending || m.size() > before))
      return "the body " + name + " has " + std::to_string(body.seats) + (body.seats == 1 ? " seat" : " seats") + ", and " +
             (amending ? std::string("the amendment would leave it with ") : std::string("the act would give it ")) + std::to_string(m.size()) + " members";
  }
  return {};
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
