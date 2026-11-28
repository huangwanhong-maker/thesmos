// A taxonomy of behaviour (decision 0008): what a society records or refers to, in five categories, the normative acts kept apart
// from conduct, and the declared forms by which communities extend the society's law without changing its ground.
//
// The five categories are the ground's: a normative act (behaviour of a kind the law defines, with a force, conditions of validity
// and effects; relational, constituted by its chronicle), conduct (what a party actually does; ontological, evidenced by the
// behaviour journal), an event (what happens that is no party's act), a state (what holds at a time) and an observation (an
// assertion about the others; manifestational). Normative acts are classed by the source of their effect, after the civil-law
// doctrine of juristic acts and the common law's acts in law: unilateral, bilateral, collective and constitutive juristic acts;
// quasi-juristic acts, whose effects the law attaches; acts of evidence; expressive acts. Every effect of every kind is composed of
// effect primitives, a few of which may be produced by law, with no author.
//
// A community extends the society's law in five declared forms, and in no other:
//
//   - a declared normative kind counts as a kind already in force, keeps its force, maps its fields onto that kind's, and may add
//     conditions of its own; its effects are those of the primitive it reduces to (Proposition 14 of the theory note);
//   - a category of conduct is a region of the space of conduct, declared by a predicate in a closed language over recorded features;
//     it is decidable when the predicate reads only recorded, typed features, and readable by the fold only when every feature it
//     reads is carried in an act's envelope; a category defined by a model is analytic and never decides;
//   - a counts-as rule makes conduct of a category, read from an act's envelope, count as a status a norm reads;
//   - a trigger rule produces an effect the ground marks as by law, as a function of the record and the turns;
//   - a standard of recognition gives an observation about conduct its standing.
//
// The society's base taxonomy is data (schema/behaviour.json, read by read_part and held equal to base()); a constitution extends it
// in its part "behaviour". Predicates are read by morphe's BNF:
//
//   predicate ::= clause ( " and " clause )*
//   clause    ::= "not " atom | atom
//   atom      ::= feature " is present" | feature " is " value | feature " is one of " value (", " value)*
//               | feature " contains " value | feature (" at least " | " at most " | " above " | " below ") number
//               | "author holds " value
//   value     ::= "'" text "'"            feature ::= a word, with hyphens for spaces and a dot for a field: field.to-community
//
// The fold reads none of the behaviour journal: an institutional effect arises only by an admitted act or a declared trigger rule
// (invariant G9; Proposition 16).
//
// The taxonomy in levels (decision 0010). The base in force is the second version, a revision of the first: behaviour described in five
// levels of one occurrence (E epistemic behaviour, O ontological conduct, M manifest conduct, S social conduct indexed by the structure
// that attributes it, N normative conduct indexed by the order, in two forms, constituted acts and qualified conduct), facets that cross
// them, bridges between them and practices over them. Its map sends the first version's normative acts to the constituted acts, its
// conduct to O and its observations to M. A feature declares its level, E or conduct's; no counts-as rule and no declared condition may
// read epistemic behaviour alone (invariant G10, freedom of thought). Descriptions of occurrences at levels are in levels.hpp.
//
// Since decision 0011 the base has the third version's directives apply, question and petition, kinds counting as request of the types
// application, question and petition; since decision 0012 the fourth version, with items as an effect primitive and their reopening as
// an effect by law; and since decision 0013 the base in force is the fifth version, whose counts-as rule makes a reading by an operative
// count as a transmission to its provider where a constitution declares that operatives transmit to their provider. A counts-as rule
// may name the declaration under which it applies ("where declared"); the base's are always in force, with the categories they read,
// and inert until their declaration is made. The author's kind and provider are features the registry carries beside the envelope.

#pragma once

#include <thesmos/acts/acts.hpp>

#include <archeion/fixity/sha256.hpp>

#include <morphe/formal/bnf.hpp>

#include <symploke/jsonld/json.hpp>

#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace thesmos::behaviour {

using Json = symploke::jsonld::Json;

// ---- The five categories and their registers ----------------------------------------------------------------------------------------

enum class Category { normative_act, conduct, event, state, observation };
enum class Register { manifestational, relational, ontological };

inline std::string_view word(Category c) {
  switch (c) {
    case Category::normative_act: return "normative act";
    case Category::conduct: return "conduct";
    case Category::event: return "event";
    case Category::state: return "state";
    case Category::observation: return "observation";
  }
  return "?";
}
inline std::string_view word(Register r) {
  switch (r) {
    case Register::manifestational: return "manifestational";
    case Register::relational: return "relational";
    case Register::ontological: return "ontological";
  }
  return "?";
}
// The register of each category (the author's Generative Relational framework, applied to the records of a society): the
// institutional fact a normative act makes is relational; conduct and events are ontological, referred to and never established; an
// institutional state is relational (a measured state manifestational); an observation is a trace until it is given standing.
inline Register register_of(Category c) {
  switch (c) {
    case Category::normative_act: return Register::relational;
    case Category::conduct: return Register::ontological;
    case Category::event: return Register::ontological;
    case Category::state: return Register::relational;
    case Category::observation: return Register::manifestational;
  }
  return Register::relational;
}
inline const std::vector<Category>& categories() {
  static const std::vector<Category> v = {Category::normative_act, Category::conduct, Category::event, Category::state, Category::observation};
  return v;
}

// A name as the taxonomy's identifiers write it: letters and digits kept, spaces and hyphens as underscores, commas dropped.
inline std::string ident(std::string_view name) {
  std::string out;
  for (char c : name) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') out += c;
    else if (c == ' ' || c == '-') out += '_';
  }
  return out;
}
// Whether a declared name can be an identifier: it begins with a letter, holds letters, digits, spaces and hyphens, and keeps none
// of them doubled or at its end.
inline bool nameable(std::string_view name) {
  if (name.empty() || !((name[0] >= 'a' && name[0] <= 'z') || (name[0] >= 'A' && name[0] <= 'Z'))) return false;
  for (std::size_t k = 0; k < name.size(); k++) {
    const char c = name[k];
    const bool letter = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
    if (!letter && c != ' ' && c != '-') return false;
    if (!letter && (k + 1 == name.size() || name[k + 1] == ' ' || name[k + 1] == '-')) return false;
  }
  return true;
}

// ---- The closed language of predicates ----------------------------------------------------------------------------------------------

struct Atom {
  enum class Op { present, equal, one_of, contains, at_least, at_most, above, below, holds };
  Op op = Op::present;
  bool negated = false;
  std::string feature;              // as written: hyphens for spaces, a dot before a field's name
  std::vector<std::string> values;  // the quoted values
  long long number = 0;
  bool operator==(const Atom&) const = default;
};

struct Predicate {
  std::string text;
  std::vector<Atom> atoms;  // a conjunction
  std::string error;        // empty when it reads
  bool ok() const { return error.empty(); }
  bool operator==(const Predicate& o) const { return text == o.text; }
};

namespace detail {
inline const morphe::formal::Bnf& predicate_grammar() {
  static const morphe::formal::Bnf bnf = morphe::formal::compile_bnf(R"bnf(
predicate ::= clause ( " and " clause )* ;
clause    ::= negation | atom ;
negation  ::= "not " atom ;
atom      ::= present | equal | oneof | contains | compare | holds ;
present   ::= feature " is present" ;
equal     ::= feature " is " value ;
oneof     ::= feature " is one of " value ( ", " value )* ;
contains  ::= feature " contains " value ;
compare   ::= feature " " op " " number ;
op        ::= "at least" | "at most" | "above" | "below" ;
holds     ::= "author holds " value ;
feature   ::= [a-z] [a-z0-9.\-]* ;
value     ::= "'" _text "'" ;
_text     ::= [^']* ;
number    ::= [0-9]+ ;
)bnf");
  return bnf;
}
inline std::string unquoted(const std::string& v) { return v.size() >= 2 ? v.substr(1, v.size() - 2) : v; }
}  // namespace detail

// Reads a predicate; one that does not read keeps the reader's diagnosis.
inline Predicate read_predicate(std::string_view text) {
  Predicate p;
  p.text = std::string(text);
  const auto parsed = detail::predicate_grammar().parse(text);
  if (!parsed.accepted) {
    p.error = parsed.error;
    return p;
  }
  for (const morphe::formal::ParseNode* clause : parsed.tree.find_all("clause")) {
    Atom a;
    const morphe::formal::ParseNode* node = clause->find("atom");
    a.negated = clause->find("negation") != nullptr;
    const morphe::formal::ParseNode& form = node->children.front();
    if (const auto* f = form.find("feature")) a.feature = f->text;
    for (const auto* v : form.find_all("value")) a.values.push_back(detail::unquoted(v->text));
    if (form.name == "present") a.op = Atom::Op::present;
    else if (form.name == "equal") a.op = Atom::Op::equal;
    else if (form.name == "oneof") a.op = Atom::Op::one_of;
    else if (form.name == "contains") a.op = Atom::Op::contains;
    else if (form.name == "holds") a.op = Atom::Op::holds;
    else {
      const std::string op = form.find("op")->text;
      a.op = op == "at least" ? Atom::Op::at_least : op == "at most" ? Atom::Op::at_most : op == "above" ? Atom::Op::above : Atom::Op::below;
      const std::string digits = form.find("number")->text;
      if (digits.size() > 18) {
        p.error = "the number " + digits + " is too large";
        return p;
      }
      a.number = std::stoll(digits);
    }
    p.atoms.push_back(std::move(a));
  }
  return p;
}

// A predicate's features named as their records name them: the hyphens of a word read as spaces ("holdings-touched" is "holdings
// touched", "field.to-community" the field "to community").
inline std::string feature_name(const std::string& written) {
  std::string out = written;
  std::replace(out.begin(), out.end(), '-', ' ');
  return out;
}

// What a predicate is evaluated on: scalar features as text, numeric ones as whole numbers, set-valued ones as sets, and whether the
// author holds a position (for conditions the fold evaluates on a state).
struct Features {
  std::map<std::string, std::string> scalars;
  std::map<std::string, long long> numbers;
  std::map<std::string, std::set<std::string>> sets;
  std::function<bool(const std::string&)> holds;
};

inline bool holds(const Atom& a, const Features& f) {
  const std::string name = feature_name(a.feature);
  bool r = false;
  const auto s = f.scalars.find(name);
  const auto n = f.numbers.find(name);
  const auto set = f.sets.find(name);
  switch (a.op) {
    case Atom::Op::present: r = s != f.scalars.end() || n != f.numbers.end() || set != f.sets.end(); break;
    case Atom::Op::equal: r = (s != f.scalars.end() && s->second == a.values.front()) || (n != f.numbers.end() && std::to_string(n->second) == a.values.front()); break;
    case Atom::Op::one_of:
      for (const auto& v : a.values) r = r || (s != f.scalars.end() && s->second == v) || (n != f.numbers.end() && std::to_string(n->second) == v);
      break;
    case Atom::Op::contains: r = set != f.sets.end() && set->second.count(a.values.front()) > 0; break;
    case Atom::Op::holds: r = f.holds && f.holds(a.values.front()); break;
    default: {
      if (n == f.numbers.end()) break;
      const long long x = n->second;
      r = a.op == Atom::Op::at_least ? x >= a.number : a.op == Atom::Op::at_most ? x <= a.number : a.op == Atom::Op::above ? x > a.number : x < a.number;
    }
  }
  return a.negated ? !r : r;
}
inline bool holds(const Predicate& p, const Features& f) {
  for (const Atom& a : p.atoms)
    if (!holds(a, f)) return false;
  return true;
}

// Whether one atom implies another on every point of the space (a sufficient condition, decided syntactically).
inline bool implies(const Atom& x, const Atom& y) {
  if (x.feature != y.feature && y.op != Atom::Op::holds) return false;
  if (x.negated || y.negated) return x == y;
  using O = Atom::Op;
  switch (y.op) {
    case O::present: return x.op != O::holds;
    case O::equal: return x.op == O::equal && x.values == y.values;
    case O::one_of:
      if (x.op == O::equal) return std::find(y.values.begin(), y.values.end(), x.values.front()) != y.values.end();
      if (x.op == O::one_of) return std::all_of(x.values.begin(), x.values.end(), [&](const std::string& v) { return std::find(y.values.begin(), y.values.end(), v) != y.values.end(); });
      return false;
    case O::contains: return x.op == O::contains && x.values == y.values;
    case O::holds: return x.op == O::holds && x.values == y.values;
    case O::at_least: return (x.op == O::at_least && x.number >= y.number) || (x.op == O::above && x.number + 1 >= y.number);
    case O::above: return (x.op == O::above && x.number >= y.number) || (x.op == O::at_least && x.number > y.number);
    case O::at_most: return (x.op == O::at_most && x.number <= y.number) || (x.op == O::below && x.number <= y.number + 1);
    case O::below: return (x.op == O::below && x.number <= y.number) || (x.op == O::at_most && x.number < y.number);
  }
  return false;
}
// Whether every point satisfying `inner` satisfies `outer`: each atom of the outer implied by an atom of the inner (sufficient).
inline bool contained(const Predicate& inner, const Predicate& outer) {
  for (const Atom& y : outer.atoms) {
    bool found = false;
    for (const Atom& x : inner.atoms) found = found || implies(x, y);
    if (!found) return false;
  }
  return true;
}

// The features an act carries in its envelope, and so the only ones the fold may read: its author, kind, turn, instant, mandate and
// fields (named "field.<name>"); and since decision 0013 the author's kind and declared provider, which the registry of parties carries
// beside the envelope (declared facts, as the envelope's are).
inline bool carried(const std::string& written) {
  return written == "author" || written == "kind" || written == "turn" || written == "when" || written == "mandate" || written.rfind("field.", 0) == 0 ||
         written == "author-kind" || written == "author-provider";
}
inline bool fold_readable(const Predicate& p) {
  for (const Atom& a : p.atoms)
    if (a.op != Atom::Op::holds && !carried(a.feature)) return false;
  return true;
}
// An act's envelope as features.
inline Features features_of(const acts::Act& a) {
  Features f;
  f.scalars["author"] = a.author, f.scalars["kind"] = a.kind;
  f.numbers["turn"] = a.turn;
  if (!a.when.empty()) f.scalars["when"] = a.when;
  if (!a.mandate.empty()) f.scalars["mandate"] = a.mandate;
  for (const auto& [k, v] : a.fields) {
    f.scalars["field." + k] = v;
    bool digits = !v.empty() && v.size() <= 18;
    for (char c : v) digits = digits && c >= '0' && c <= '9';
    if (digits) f.numbers["field." + k] = std::stoll(v);
  }
  return f;
}
// An act's envelope with what the registry carries of its author (decision 0013): its kind, and an operative's declared provider.
inline Features features_of(const acts::Act& a, const kernel::Parties& ps) {
  Features f = features_of(a);
  if (ps.has(a.author)) {
    const kernel::Party& p = ps.at(a.author);
    f.scalars["author kind"] = std::string(kernel::word(p.kind));
    if (p.kind == kernel::Kind::operative && !p.provider.empty()) f.scalars["author provider"] = p.provider;
  }
  return f;
}

// ---- Episodes of conduct ------------------------------------------------------------------------------------------------------------

// An episode of conduct, as the behaviour journal records it: who acted and who answers, on which provider and model, where, when,
// what operation, what it spent in each resource's unit, the digests of what it read and wrote, the holdings and items it touched,
// the act it constituted (its kind), and declared features. The act's seal, or the reason it was refused, is recorded in the next entry.
struct Episode {
  std::string party, bearer, provider, model, community, node, operation, kind;
  long long turn = 0;
  std::string begun, ended;
  std::map<std::string, long long> resources;
  std::set<std::string> inputs, outputs, holdings, items;
  std::map<std::string, std::string> features;
  bool operator==(const Episode&) const = default;
};

// The canonical text of an episode: each part prefixed by its length, in a fixed order, so that two episodes share it exactly when
// equal.
inline std::string canonical(const Episode& e) {
  using acts::detail::put;
  std::string s;
  for (const auto& [k, v] : std::vector<std::pair<const char*, const std::string*>>{{"party", &e.party}, {"bearer", &e.bearer}, {"provider", &e.provider}, {"model", &e.model},
                                                                                    {"community", &e.community}, {"node", &e.node}, {"operation", &e.operation}, {"kind", &e.kind},
                                                                                    {"begun", &e.begun}, {"ended", &e.ended}})
    put(s, k, *v);
  put(s, "turn", std::to_string(e.turn));
  for (const auto& [r, n] : e.resources) put(s, "resource." + r, std::to_string(n));
  for (const auto& [name, set] : std::vector<std::pair<const char*, const std::set<std::string>*>>{{"input", &e.inputs}, {"output", &e.outputs}, {"holding", &e.holdings}, {"item", &e.items}})
    for (const auto& x : *set) put(s, name, x);
  for (const auto& [k, v] : e.features) put(s, "feature." + k, v);
  return s;
}
inline std::string digest(const Episode& e) { return archeion::fixity::sha256_hex(canonical(e)); }

// An episode's features, for categories of conduct.
inline Features features_of(const Episode& e) {
  Features f;
  for (const auto& [k, v] : std::vector<std::pair<const char*, const std::string*>>{{"party", &e.party}, {"bearer", &e.bearer}, {"provider", &e.provider}, {"model", &e.model},
                                                                                    {"community", &e.community}, {"node", &e.node}, {"operation", &e.operation}, {"kind", &e.kind}})
    if (!v->empty()) f.scalars[k] = *v;
  f.numbers["turn"] = e.turn;
  for (const auto& [r, n] : e.resources) f.numbers[r] = n;
  f.sets["inputs"] = e.inputs, f.sets["outputs"] = e.outputs, f.sets["holdings touched"] = e.holdings, f.sets["items touched"] = e.items;
  for (const auto& [k, v] : e.features) {
    f.scalars[k] = v;
    bool digits = !v.empty() && v.size() <= 18;
    for (char c : v) digits = digits && c >= '0' && c <= '9';
    if (digits) f.numbers[k] = std::stoll(v);
  }
  return f;
}

// ---- The parts of a taxonomy --------------------------------------------------------------------------------------------------------

struct Effect {  // an effect primitive: what it changes, and the forms in which the law produces it with no author
  std::string name, what;
  std::vector<std::string> by_law;
  bool operator==(const Effect&) const = default;
};
struct Primitive {  // a kind of act of the ground, placed by the source of its effect
  std::string kind, cls;
  acts::Force force = acts::Force::declarative;
  std::vector<std::string> effects, reads;
  bool operator==(const Primitive&) const = default;
};
struct Declared {  // a normative kind declared by counts-as (Definition 31)
  std::string kind, counts_as;
  acts::Force force = acts::Force::declarative;
  std::map<std::string, std::string> renamed;  // a field of the kind counted as <- a field of this kind
  std::map<std::string, std::string> fixed;    // a field of the kind counted as <- a fixed value
  std::vector<Predicate> valid_when;
  std::vector<std::string> in;  // the scenes in which it may be made
  bool operator==(const Declared&) const = default;
};
struct Feature {  // a feature of the space of conduct (Definition 26)
  std::string name, space, unit;
  std::vector<std::string> vocabulary;  // of a categorical feature (empty: open), in order for an ordinal one
  long long scale = 1;                  // a real feature's distance is |a - b| / scale, capped at 1
  long long weight = 1;
  std::string recorded_by, grade;
  std::string level;                    // the level of description it describes: empty for conduct (O), "E" for epistemic behaviour (decision 0010)
  bool operator==(const Feature&) const = default;
};
struct Conduct {  // a category of conduct (Definition 27)
  std::string name;
  Predicate where;
  bool analytic = false;  // defined by a model: it yields observations of grade C, never a decision
  bool operator==(const Conduct&) const = default;
};
struct CountsAs {  // conduct of a category, read from an act's envelope, counts as a status a norm reads (Definition 33)
  std::string rule, conduct, status, value;  // status "transmission"; value the feature of the act that names the service
  std::string where_declared;                // the declaration of a constitution under which alone it applies, or empty (decision 0013)
  bool operator==(const CountsAs&) const = default;
};
// The declarations a counts-as rule may apply under (decision 0013).
inline bool known_declaration(const std::string& d) { return d == "operatives transmit to their provider"; }
struct Trigger {  // when a state of affairs holds, an effect follows by law (Definition 34)
  std::string rule, then;
  std::string position;      // "end position": the position, ended when held for more than held_for turns
  long long held_for = 0;
  std::string matter;        // "lapse", "overdue": the commitments of this matter (empty: every one)
  long long past_due_by = 0; // "lapse": the turns past its due after which an active commitment lapses
  bool by_ground = false;    // applied by the ground itself (the base's), not by the fold's settlement
  bool operator==(const Trigger&) const = default;
};
struct Standard {  // a standard of recognition (Definition 35)
  std::string fact, grade = "C";
  bool another_origin = false;
  bool operator==(const Standard&) const = default;
};

// The taxonomy in levels (decision 0010): levels of description of one occurrence, facets that cross them, bridges between them, and
// practices as templates of linked descriptions. The first version has none of these; the second, the base in force, has them all.
struct Level {  // a level of description (Definition 43)
  std::string letter, name, register_;
  std::string indexed;             // "structure" (S) or "order" (N): a description at it names the structure that makes it; else empty
  std::vector<std::string> forms;  // its forms, the first of a level of normative conduct holding the kinds of act (constituted acts)
  bool operator==(const Level&) const = default;
};
struct Facet {  // a facet that crosses the levels (Definition 45)
  std::string name;
  bool ordered = false;
  std::vector<std::string> values;  // lowest first when ordered
  std::vector<std::string> levels;  // the letters of the levels it describes
  bool operator==(const Facet&) const = default;
};
struct Bridge {  // a declared kind of link from a description at one level to one at another (Definition 46)
  std::string name;
  std::vector<std::pair<std::string, std::string>> pairs;  // (from, to), by the levels' letters
  bool operator==(const Bridge&) const = default;
};
struct Slot {  // a place in a practice, filled by a description at its level
  std::string role, level;
  bool optional = false;
  bool operator==(const Slot&) const = default;
};
struct Step {  // a link a practice asks between two of its slots
  std::string bridge, from, to;
  bool operator==(const Step&) const = default;
};
struct Practice {  // a declared template of linked descriptions over levels (Definition 47)
  std::string name;
  std::vector<Slot> slots;
  std::vector<Step> links;
  bool operator==(const Practice&) const = default;
};

// A taxonomy, or a constitution's part "behaviour" (noetarium.behaviour/1).
struct Part {
  std::string format = "noetarium.behaviour/1", name;
  long version = 1;
  std::string extends;  // "base 1" or empty
  std::vector<std::string> classes;
  std::vector<Effect> effects;
  std::vector<Primitive> primitives;
  std::vector<Declared> declared;
  std::vector<Feature> features;
  std::string nearness = "sum";  // how the distances of features combine: "sum" (weighted) or "max"
  std::vector<Conduct> conduct;
  std::vector<CountsAs> counts_as;
  std::vector<Trigger> triggers;
  std::vector<Standard> recognition;
  std::vector<Level> levels;                         // the levels of description (decision 0010): the base's, never a community's
  std::vector<Facet> facets;                         // the facets that cross them: the base's
  std::vector<Bridge> bridges;                       // the bridges between them: the base's
  std::vector<Practice> practices;                   // practices: the base's, and a community's own
  long revises = 0;                                  // the version this part revises, if any
  std::map<std::string, std::string> revised_kinds;  // old -> new, for declared kinds and categories renamed
  std::vector<std::string> unread;                   // parts of unexpected shape
  bool operator==(const Part&) const = default;

  const Declared* declared_kind(const std::string& k) const {
    for (const auto& d : declared)
      if (d.kind == k) return &d;
    return nullptr;
  }
  const Primitive* primitive(const std::string& k) const {
    for (const auto& p : primitives)
      if (p.kind == k) return &p;
    return nullptr;
  }
  const Conduct* category(const std::string& c) const {
    for (const auto& x : conduct)
      if (x.name == c) return &x;
    return nullptr;
  }
  const Feature* feature(const std::string& f) const {
    for (const auto& x : features)
      if (x.name == f) return &x;
    return nullptr;
  }
  const Effect* effect(const std::string& e) const {
    for (const auto& x : effects)
      if (x.name == e) return &x;
    return nullptr;
  }
  const Level* level(const std::string& letter) const {
    for (const auto& x : levels)
      if (x.letter == letter) return &x;
    return nullptr;
  }
  const Facet* facet(const std::string& f) const {
    for (const auto& x : facets)
      if (x.name == f) return &x;
    return nullptr;
  }
  const Bridge* bridge(const std::string& b) const {
    for (const auto& x : bridges)
      if (x.name == b) return &x;
    return nullptr;
  }
  const Practice* practice(const std::string& p) const {
    for (const auto& x : practices)
      if (x.name == p) return &x;
    return nullptr;
  }
  // The level a feature describes: its declared level, else conduct's.
  std::string level_of_feature(const std::string& name) const {
    const Feature* f = feature(name);
    return f && !f->level.empty() ? f->level : std::string("O");
  }
};

// ---- Reading and writing ------------------------------------------------------------------------------------------------------------

inline std::optional<acts::Force> force_named(std::string_view w) {
  for (auto f : {acts::Force::expressive, acts::Force::assertive, acts::Force::directive, acts::Force::commissive, acts::Force::declarative})
    if (acts::word(f) == w) return f;
  return std::nullopt;
}

namespace detail {
inline std::string text(const Json* j) { return j && j->is_string() ? j->text : std::string(); }
inline std::vector<std::string> strings(const Json* j) {
  std::vector<std::string> v;
  if (!j) return v;
  if (j->is_string()) v.push_back(j->text);
  else if (j->is_array())
    for (const Json& i : j->items)
      if (i.is_string()) v.push_back(i.text);
  return v;
}
inline long long whole(const Json* j, long long otherwise = 0) {
  if (!j || !j->is_number()) return otherwise;
  if (!j->lexical.empty() && j->lexical.find_first_not_of("0123456789-") == std::string::npos && j->lexical.size() <= 18) return std::stoll(j->lexical);
  return static_cast<long long>(j->number);
}
inline std::map<std::string, std::string> texts(const Json* j) {
  std::map<std::string, std::string> m;
  if (j && j->is_object())
    for (const auto& [k, v] : j->members)
      if (v.is_string()) m[k] = v.text;
  return m;
}
}  // namespace detail

// Reads a part "behaviour" (or the base taxonomy) from its written form. Parts of unexpected shape are noted in `unread`, which
// admission reports; a predicate that does not read keeps its diagnosis.
inline Part read_part(const Json& j) {
  using detail::strings;
  using detail::text;
  Part p;
  if (!j.is_object()) {
    p.unread.push_back("the part behaviour is not an object");
    return p;
  }
  p.format = text(j.get("format")), p.name = text(j.get("name")), p.extends = text(j.get("extends"));
  p.version = static_cast<long>(detail::whole(j.get("version"), 1));
  p.classes = strings(j.get("classes"));
  if (const Json* e = j.get("effects"); e && e->is_array())
    for (const Json& x : e->items) p.effects.push_back({text(x.get("effect")), text(x.get("changes")), strings(x.get("by law"))});
  if (const Json* k = j.get("primitives"); k && k->is_array())
    for (const Json& x : k->items) {
      Primitive q;
      q.kind = text(x.get("kind")), q.cls = text(x.get("class"));
      if (const auto f = force_named(text(x.get("force")))) q.force = *f;
      else p.unread.push_back("the primitive " + q.kind + " names no force");
      q.effects = strings(x.get("effects")), q.reads = strings(x.get("reads"));
      p.primitives.push_back(std::move(q));
    }
  if (const Json* k = j.get("kinds"); k && k->is_array())
    for (const Json& x : k->items) {
      Declared d;
      d.kind = text(x.get("kind")), d.counts_as = text(x.get("counts as"));
      if (const auto f = force_named(text(x.get("force")))) d.force = *f;
      else p.unread.push_back("the declared kind " + d.kind + " names no force");
      d.renamed = detail::texts(x.get("fields")), d.fixed = detail::texts(x.get("fixed"));
      for (const auto& c : strings(x.get("valid when"))) d.valid_when.push_back(read_predicate(c));
      d.in = strings(x.get("in"));
      p.declared.push_back(std::move(d));
    }
  else if (j.has("kinds"))
    p.unread.push_back("the declared kinds are not a list");
  if (const Json* f = j.get("features"); f && f->is_array())
    for (const Json& x : f->items) {
      Feature q;
      q.name = text(x.get("name")), q.space = text(x.get("space")), q.unit = text(x.get("unit"));
      q.vocabulary = strings(x.get("vocabulary"));
      q.scale = detail::whole(x.get("scale"), 1), q.weight = detail::whole(x.get("weight"), 1);
      q.recorded_by = text(x.get("recorded by")), q.grade = text(x.get("grade"));
      q.level = text(x.get("level"));
      p.features.push_back(std::move(q));
    }
  if (const Json* n = j.get("nearness"); n && n->is_string()) p.nearness = n->text;
  if (const Json* c = j.get("conduct"); c && c->is_array())
    for (const Json& x : c->items) {
      Conduct q;
      q.name = text(x.get("category")), q.where = read_predicate(text(x.get("where")));
      if (const Json* a = x.get("analytic"); a && a->is_bool()) q.analytic = a->truth;
      p.conduct.push_back(std::move(q));
    }
  if (const Json* c = j.get("counts as"); c && c->is_array())
    for (const Json& x : c->items) {
      CountsAs q;
      q.rule = text(x.get("rule")), q.conduct = text(x.get("conduct"));
      if (const Json* as = x.get("as"); as && as->is_object() && as->members.size() == 1 && as->members.front().second.is_string())
        q.status = as->members.front().first, q.value = as->members.front().second.text;
      else
        p.unread.push_back("the counts-as rule " + q.rule + " names no status");
      q.where_declared = text(x.get("where declared"));  // decision 0013
      p.counts_as.push_back(std::move(q));
    }
  if (const Json* t = j.get("triggers"); t && t->is_array())
    for (const Json& x : t->items) {
      Trigger q;
      q.rule = text(x.get("rule")), q.then = text(x.get("then"));
      if (const Json* b = x.get("by the ground"); b && b->is_bool()) q.by_ground = b->truth;
      if (const Json* w = x.get("when"); w && w->is_object()) {
        if (const Json* pos = w->get("position"); pos && pos->is_string()) q.position = pos->text, q.held_for = detail::whole(w->get("held for more than"));
        if (const Json* c = w->get("commitment"); c && c->is_object()) q.matter = text(c->get("matter")), q.past_due_by = detail::whole(c->get("past due by"));
      }
      p.triggers.push_back(std::move(q));
    }
  if (const Json* r = j.get("recognition"); r && r->is_array())
    for (const Json& x : r->items) {
      Standard q;
      q.fact = text(x.get("fact")), q.grade = text(x.get("grade at least"));
      q.another_origin = text(x.get("attested by")) == "another origin";
      p.recognition.push_back(std::move(q));
    }
  if (const Json* r = j.get("revises"); r && r->is_object()) {
    p.revises = static_cast<long>(detail::whole(r->get("from")));
    p.revised_kinds = detail::texts(r->get("kinds"));
  }
  // The taxonomy in levels (decision 0010).
  auto truth = [](const Json* b) { return b && b->is_bool() && b->truth; };
  if (const Json* l = j.get("levels"); l && l->is_array())
    for (const Json& x : l->items) p.levels.push_back({text(x.get("level")), text(x.get("name")), text(x.get("register")), text(x.get("indexed by")), strings(x.get("forms"))});
  if (const Json* f = j.get("facets"); f && f->is_array())
    for (const Json& x : f->items) p.facets.push_back({text(x.get("facet")), truth(x.get("ordered")), strings(x.get("values")), strings(x.get("levels"))});
  if (const Json* b = j.get("bridges"); b && b->is_array())
    for (const Json& x : b->items) {
      Bridge q;
      q.name = text(x.get("bridge"));
      if (const Json* ps = x.get("pairs"); ps && ps->is_array())
        for (const Json& pr : ps->items) {
          const auto two = strings(&pr);
          if (pr.is_array() && two.size() == 2 && pr.items.size() == 2) q.pairs.push_back({two[0], two[1]});
          else p.unread.push_back("the bridge " + q.name + " has a pair that is not two levels");
        }
      p.bridges.push_back(std::move(q));
    }
  if (const Json* pr = j.get("practices"); pr && pr->is_array())
    for (const Json& x : pr->items) {
      Practice q;
      q.name = text(x.get("practice"));
      if (const Json* sl = x.get("slots"); sl && sl->is_array())
        for (const Json& y : sl->items) q.slots.push_back({text(y.get("role")), text(y.get("level")), truth(y.get("optional"))});
      if (const Json* ln = x.get("links"); ln && ln->is_array())
        for (const Json& y : ln->items) q.links.push_back({text(y.get("bridge")), text(y.get("from")), text(y.get("to"))});
      p.practices.push_back(std::move(q));
    }
  else if (j.has("practices"))
    p.unread.push_back("the practices are not a list");
  return p;
}
inline Part read_part(std::string_view written) { return read_part(symploke::jsonld::parse(written)); }

// The part written back, compact and canonical (RFC 8785): two parts are written alike exactly when they are equal.
inline std::string canonical(const Part& p) {
  Json j = Json::object();
  auto list = [](const std::vector<std::string>& v) {
    Json a = Json::array();
    for (const auto& s : v) a.items.push_back(Json::string(s));
    return a;
  };
  auto map = [](const std::map<std::string, std::string>& m) {
    Json o = Json::object();
    for (const auto& [k, v] : m) o.set(k, Json::string(v));
    return o;
  };
  auto num = [](long long n) { return Json::num(static_cast<double>(n), std::to_string(n)); };
  j.set("format", Json::string(p.format)), j.set("name", Json::string(p.name)), j.set("version", num(p.version)), j.set("extends", Json::string(p.extends));
  j.set("classes", list(p.classes));
  Json effects = Json::array(), prims = Json::array(), decl = Json::array(), feats = Json::array(), cond = Json::array(), counts = Json::array(), trig = Json::array(), rec = Json::array();
  for (const auto& e : p.effects) {
    Json x = Json::object();
    x.set("effect", Json::string(e.name)), x.set("changes", Json::string(e.what)), x.set("by law", list(e.by_law));
    effects.items.push_back(x);
  }
  for (const auto& q : p.primitives) {
    Json x = Json::object();
    x.set("kind", Json::string(q.kind)), x.set("class", Json::string(q.cls)), x.set("force", Json::string(std::string(acts::word(q.force))));
    x.set("effects", list(q.effects)), x.set("reads", list(q.reads));
    prims.items.push_back(x);
  }
  for (const auto& d : p.declared) {
    Json x = Json::object(), v = Json::array();
    x.set("kind", Json::string(d.kind)), x.set("force", Json::string(std::string(acts::word(d.force)))), x.set("counts as", Json::string(d.counts_as));
    x.set("fields", map(d.renamed)), x.set("fixed", map(d.fixed));
    for (const auto& w : d.valid_when) v.items.push_back(Json::string(w.text));
    x.set("valid when", v), x.set("in", list(d.in));
    decl.items.push_back(x);
  }
  for (const auto& f : p.features) {
    Json x = Json::object();
    x.set("name", Json::string(f.name)), x.set("space", Json::string(f.space)), x.set("unit", Json::string(f.unit)), x.set("vocabulary", list(f.vocabulary));
    x.set("scale", num(f.scale)), x.set("weight", num(f.weight)), x.set("recorded by", Json::string(f.recorded_by)), x.set("grade", Json::string(f.grade));
    if (!f.level.empty()) x.set("level", Json::string(f.level));  // written only when set, so that a first version is written as before
    feats.items.push_back(x);
  }
  for (const auto& c : p.conduct) {
    Json x = Json::object();
    x.set("category", Json::string(c.name)), x.set("where", Json::string(c.where.text)), x.set("analytic", Json::boolean(c.analytic));
    cond.items.push_back(x);
  }
  for (const auto& c : p.counts_as) {
    Json x = Json::object(), as = Json::object();
    as.set(c.status, Json::string(c.value));
    x.set("rule", Json::string(c.rule)), x.set("conduct", Json::string(c.conduct)), x.set("as", as);
    if (!c.where_declared.empty()) x.set("where declared", Json::string(c.where_declared));  // written only when set (decision 0013)
    counts.items.push_back(x);
  }
  for (const auto& t : p.triggers) {
    Json x = Json::object(), w = Json::object();
    if (!t.position.empty()) w.set("position", Json::string(t.position)), w.set("held for more than", num(t.held_for));
    if (t.then == "lapse" || t.then == "overdue") {
      Json c = Json::object();
      c.set("matter", Json::string(t.matter)), c.set("past due by", num(t.past_due_by));
      w.set("commitment", c);
    }
    x.set("rule", Json::string(t.rule)), x.set("when", w), x.set("then", Json::string(t.then)), x.set("by the ground", Json::boolean(t.by_ground));
    trig.items.push_back(x);
  }
  for (const auto& r : p.recognition) {
    Json x = Json::object();
    x.set("fact", Json::string(r.fact)), x.set("grade at least", Json::string(r.grade)), x.set("attested by", Json::string(r.another_origin ? "another origin" : "anyone"));
    rec.items.push_back(x);
  }
  j.set("effects", effects), j.set("primitives", prims), j.set("kinds", decl), j.set("features", feats), j.set("nearness", Json::string(p.nearness));
  j.set("conduct", cond), j.set("counts as", counts), j.set("triggers", trig), j.set("recognition", rec);
  if (p.revises) {
    Json r = Json::object();
    r.set("from", num(p.revises)), r.set("kinds", map(p.revised_kinds));
    j.set("revises", r);
  }
  // The taxonomy in levels, each part written only when declared (decision 0010).
  if (!p.levels.empty()) {
    Json a = Json::array();
    for (const auto& l : p.levels) {
      Json x = Json::object();
      x.set("level", Json::string(l.letter)), x.set("name", Json::string(l.name)), x.set("register", Json::string(l.register_)), x.set("indexed by", Json::string(l.indexed));
      x.set("forms", list(l.forms));
      a.items.push_back(x);
    }
    j.set("levels", a);
  }
  if (!p.facets.empty()) {
    Json a = Json::array();
    for (const auto& f : p.facets) {
      Json x = Json::object();
      x.set("facet", Json::string(f.name)), x.set("ordered", Json::boolean(f.ordered)), x.set("values", list(f.values)), x.set("levels", list(f.levels));
      a.items.push_back(x);
    }
    j.set("facets", a);
  }
  if (!p.bridges.empty()) {
    Json a = Json::array();
    for (const auto& b : p.bridges) {
      Json x = Json::object(), ps = Json::array();
      for (const auto& [from, to] : b.pairs) ps.items.push_back(list({from, to}));
      x.set("bridge", Json::string(b.name)), x.set("pairs", ps);
      a.items.push_back(x);
    }
    j.set("bridges", a);
  }
  if (!p.practices.empty()) {
    Json a = Json::array();
    for (const auto& q : p.practices) {
      Json x = Json::object(), sl = Json::array(), ln = Json::array();
      for (const auto& s : q.slots) {
        Json y = Json::object();
        y.set("role", Json::string(s.role)), y.set("level", Json::string(s.level)), y.set("optional", Json::boolean(s.optional));
        sl.items.push_back(y);
      }
      for (const auto& s : q.links) {
        Json y = Json::object();
        y.set("bridge", Json::string(s.bridge)), y.set("from", Json::string(s.from)), y.set("to", Json::string(s.to));
        ln.items.push_back(y);
      }
      x.set("practice", Json::string(q.name)), x.set("slots", sl), x.set("links", ln);
      a.items.push_back(x);
    }
    j.set("practices", a);
  }
  return symploke::jsonld::canonical(j);
}

// ---- The base taxonomy ----------------------------------------------------------------------------------------------------------

// The base taxonomy's first version (schema/behaviour.json, decisions 0008 and 0009): the classes of normative acts, the effect
// primitives with what the law produces by itself, every kind of the ground then known placed by class with its force, effects and the
// fields the fold reads, the features the node records, a few categories of conduct, one example of a declared kind, the ground's own
// effects by law, and a standard of recognition. It is kept as it was adopted; the base in force is its revision, base().
inline const Part& first_base() {
  static const Part b = [] {
    Part p;
    p.name = "base";
    p.classes = {"juristic, unilateral", "juristic, bilateral", "juristic, collective", "juristic, constitutive", "quasi-juristic", "evidence", "expressive"};
    p.effects = {{"position", "positions: enter, leave", {"end position"}},
                 {"mandate", "mandates: issue, revoke", {}},
                 {"commitment", "commitments: create, move", {"lapse", "overdue"}},
                 {"grant", "grants, and allowances charged along chains", {}},
                 {"holding", "holdings and their accesses; the check of a deposit", {"loan returns"}},
                 {"claim", "claims: open, answer", {}},
                 {"assessment", "assessments: record", {}},
                 {"proposal", "proposals and the rules in force: ballots, adoption, a change applied", {}},
                 {"evidence", "nothing beyond the seal", {}},
                 {"party", "known parties with their control and provision: declare", {}},
                 {"recognition", "recognitions of other communities: set", {}}};
    const std::vector<std::string> access = {"holding", "amount", "purpose", "transmit", "kind", "within", "digest", "source", "licence", "unit", "to", "until"};
    const std::vector<std::string> owing = {"id", "to", "matter", "unit", "amount", "due"};
    struct Row {
      const char* kind;
      const char* cls;
      std::vector<std::string> effects, reads;
    };
    const std::vector<Row> rows = {
        {"say", "expressive", {"evidence"}, {}},
        {"acknowledge", "expressive", {"evidence"}, {}},
        {"report", "evidence", {"evidence", "mandate"}, {"matter", "head of", "size", "root", "mandate status", "status"}},
        {"deliver", "quasi-juristic", {"commitment"}, {"id", "artefact"}},
        {"assess", "evidence", {"assessment"}, {"object", "criterion", "value"}},
        {"attest", "evidence", {"holding", "evidence"}, {"holding", "crossing", "digest", "from"}},
        {"hand over", "quasi-juristic", {"commitment"}, {"id", "artefact"}},
        {"request", "quasi-juristic", {"evidence"}, {}},
        {"claim", "quasi-juristic", {"claim"}, {"id", "against"}},
        {"propose", "juristic, collective", {"proposal"}, {"id", "decision", "level", "change"}},
        {"offer", "juristic, unilateral", {"commitment"}, owing},
        {"take", "juristic, bilateral", {"commitment"}, {"id"}},
        {"commit", "juristic, unilateral", {"commitment"}, owing},
        {"grant", "juristic, unilateral", {"grant"}, {"id", "to", "resource", "ceiling"}},
        {"found", "juristic, constitutive", {"position", "holding", "party"}, {"community", "parties"}},
        {"join", "juristic, unilateral", {"position"}, {"position", "sponsor"}},
        {"admit", "juristic, unilateral", {"position", "proposal"}, {"position", "whom", "proposal"}},
        {"leave", "juristic, unilateral", {"position"}, {"position"}},
        {"mandate", "juristic, unilateral", {"mandate", "grant"}, {}},
        {"revoke", "juristic, unilateral", {"mandate"}, {"which"}},
        {"accept", "juristic, unilateral", {"commitment"}, {"id"}},
        {"release", "juristic, unilateral", {"commitment"}, {"id"}},
        {"cancel", "juristic, unilateral", {"commitment"}, {"id"}},
        {"assign", "juristic, unilateral", {"commitment"}, {"id", "to"}},
        {"use", "juristic, unilateral", {"grant", "holding"}, {"id", "amount", "holding", "purpose", "transmit"}},
        {"answer", "juristic, unilateral", {"claim"}, {"id", "finding"}},
        {"convert", "juristic, unilateral", {"position"}, {"rule", "whom"}},
        {"vote", "juristic, collective", {"proposal"}, {"id", "value"}},
        {"amend", "juristic, collective", {"proposal"}, {"id"}},
        {"consult", "juristic, unilateral", {"holding"}, access},
        {"deposit", "juristic, unilateral", {"holding"}, access},
        {"modify", "juristic, unilateral", {"holding"}, access},
        {"lend", "juristic, unilateral", {"holding"}, access},
        {"withdraw", "juristic, unilateral", {"holding"}, access},
        {"present", "quasi-juristic", {"party", "mandate"}, {"passport"}},
        {"recognise", "juristic, unilateral", {"recognition"}, {"of", "level", "matter"}},
    };
    for (const Row& r : rows) p.primitives.push_back({r.kind, r.cls, *acts::force_of(r.kind), r.effects, r.reads});
    {
      Declared notice;
      notice.kind = "notice", notice.counts_as = "report", notice.force = acts::Force::assertive, notice.fixed = {{"type", "notice"}};
      p.declared.push_back(notice);
    }
    p.features = {
        {"operation", "categorical", "", {"run tool", "call model", "read", "write", "send", "compute", "wait", "message", "other"}, 1, 1, "the node", "A"},
        {"tokens", "real", "tokens", {}, 100000, 1, "the principal's ledger", "A"},
        {"runs", "real", "runs", {}, 10, 1, "the node", "A"},
        {"seconds", "real", "seconds", {}, 3600, 1, "the node", "A"},
        {"bytes", "real", "bytes", {}, 1000000, 1, "the node", "A"},
        {"inputs", "digests", "SHA-256", {}, 1, 1, "the node", "A"},
        {"outputs", "digests", "SHA-256", {}, 1, 1, "the node", "A"},
        {"holdings touched", "set", "", {}, 1, 1, "the gateway", "A"},
        {"items touched", "set", "", {}, 1, 1, "the gateway", "A"},
        {"provider", "categorical", "", {}, 1, 1, "the passport", "A"},
        {"model", "categorical", "", {}, 1, 1, "the passport", "A"},
        {"community", "categorical", "", {}, 1, 1, "the gateway", "A"},
        {"node", "categorical", "", {}, 1, 1, "the gateway", "A"},
        {"words", "real", "words", {}, 1000, 1, "analysis", "C"},
    };
    p.conduct = {{"bench run", read_predicate("operation is 'run tool' and holdings-touched contains 'bench'"), false},
                 {"heavy model call", read_predicate("operation is 'call model' and tokens above 100000"), false},
                 {"a reading", read_predicate("operation is 'read'"), false}};
    p.triggers = {{"a loan returns at its term", "loan returns", "", 0, "", 0, true}, {"a commitment past its due is overdue", "overdue", "", 0, "", 0, true}};
    p.recognition = {{"authorship", "B", true}};
    return p;
  }();
  return b;
}

// The base taxonomy's second version (schema/behaviour-2.json, decision 0010), a revision of the first: behaviour described
// in five levels of one occurrence, E epistemic behaviour, O ontological conduct, M manifest conduct, S social conduct indexed by the
// structure that attributes it, and N normative conduct indexed by the order, in two forms (constituted acts, qualified conduct); the
// facets that cross them; the nine bridges between them; three practices. The first version's normative acts are the constituted acts,
// its conduct level O and its observations level M; events and states are kept. It adds the kinds of the ground that ratify and
// denounce a disclosure agreement, the effect primitive agreement with its end by law, and the features of disclosed epistemic behaviour.
inline const Part& second_base() {
  static const Part b = [] {
    Part p = first_base();
    p.version = 2, p.revises = 1;
    p.revised_kinds = {{"normative act", "constituted act"}, {"conduct", "ontological conduct"}, {"observation", "manifest conduct"}};
    p.effects.push_back({"agreement", "disclosure agreements: ratify, denounce; their records kept, returned or erased", {"agreement ends"}});
    p.primitives.push_back({"agree", "juristic, bilateral", *acts::force_of("agree"), {"agreement", "holding"}, {"agreement", "for", "terms"}});
    p.primitives.push_back({"denounce", "juristic, unilateral", *acts::force_of("denounce"), {"agreement"}, {"agreement", "for"}});
    p.features.push_back({"epistemic kind", "categorical", "", {"reasoning", "plan", "deliberation", "model call"}, 1, 1, "the agent's disclosure", "C", "E"});
    p.features.push_back({"disclosed under", "categorical", "", {}, 1, 1, "the disclosure agreement", "A", "E"});
    p.triggers.push_back({"a disclosure agreement ends at its term or its notice", "agreement ends", "", 0, "", 0, true});
    p.levels = {{"E", "epistemic behaviour", "Ontological", "", {}},
                {"O", "ontological conduct", "Ontological", "", {}},
                {"M", "manifest conduct", "Manifestational", "", {}},
                {"S", "social conduct", "Relational", "structure", {}},
                {"N", "normative conduct", "Relational", "order", {"constituted act", "qualified conduct"}}};
    p.facets = {{"stage", true, {"formation of intent", "preparation", "commencement", "execution", "completion"}, {"E", "O", "S", "N"}},
                {"polarity", false, {"doing", "omitting"}, {"E", "O", "S", "N"}},
                {"agency", false, {"individual", "joint", "collective"}, {"E", "O", "S", "N"}},
                {"modality", false, {"communicative", "physical", "computational"}, {"O", "M", "S", "N"}},
                {"intention", true, {"accidental", "negligent", "foreseen", "intended"}, {"E", "S", "N"}},
                {"scope", false, {"self-regarding", "other-regarding", "directed at a structure"}, {"E", "O", "S", "N"}}};
    p.bridges = {{"enactment", {{"E", "O"}}},
                 {"disclosure", {{"E", "M"}}},
                 {"manifestation", {{"O", "M"}}},
                 {"recognition by evidence", {{"M", "S"}, {"M", "N"}}},
                 {"social attribution", {{"M", "S"}}},
                 {"codified counts-as", {{"O", "N"}, {"S", "N"}}},
                 {"custom formation", {{"S", "N"}}},
                 {"authorisation and obligation", {{"N", "E"}, {"N", "O"}}},
                 {"feedback", {{"N", "S"}, {"N", "E"}}}};
    p.practices = {{"protest",
                    {{"instructions", "E", true}, {"note", "O", false}, {"text", "M", false}, {"reading", "S", true}, {"formal protest", "N", false}},
                    {{"enactment", "instructions", "note"}, {"manifestation", "note", "text"}, {"social attribution", "text", "reading"}, {"codified counts-as", "note", "formal protest"}}},
                   {"act through an agent",
                    {{"reasoning", "E", true}, {"operation", "O", false}, {"trace", "M", false}, {"act", "N", false}},
                    {{"enactment", "reasoning", "operation"}, {"manifestation", "operation", "trace"}, {"codified counts-as", "operation", "act"}}},
                   {"review",
                    {{"deliberation", "E", true}, {"reading", "O", false}, {"assessment", "M", false}, {"standing", "S", true}, {"finding", "N", false}},
                    {{"enactment", "deliberation", "reading"}, {"manifestation", "reading", "assessment"}, {"social attribution", "assessment", "standing"},
                     {"recognition by evidence", "assessment", "finding"}}}};
    return p;
  }();
  return b;
}

// The base taxonomy's third version (schema/behaviour-3.json, decision 0011), a revision of the second that renames nothing:
// the directives apply, question and petition, declared kinds that count as request of the types application, question and petition,
// so that each opens an item on its route and adds no effect beyond the request's (Proposition 14).
inline const Part& third_base() {
  static const Part b = [] {
    Part p = second_base();
    p.version = 3, p.revises = 2, p.revised_kinds = {};
    const std::vector<std::tuple<const char*, const char*, const char*>> directives = {
        {"apply", "application", "field.id is present and field.position is present"},
        {"question", "question", "field.id is present and field.about is present and field.subject is present"},
        {"petition", "petition", "field.id is present and field.kind is present"}};
    for (const auto& [kind, type, valid] : directives) {
      Declared d;
      d.kind = kind, d.counts_as = "request", d.force = acts::Force::directive, d.fixed = {{"type", type}};
      d.valid_when = {read_predicate(valid)};
      p.declared.push_back(d);
    }
    return p;
  }();
  return b;
}

// The base taxonomy's fourth version (schema/behaviour-4.json, decision 0012), a revision of the third that renames nothing: items,
// which typed requests open and posted work runs through, are an effect primitive, and the reopening of an item when a taking's term or
// a returned delivery's redelivery runs out is an effect by law, the ground's own; the kinds that act on items say so, and read the
// fields they now read.
inline const Part& fourth_base() {
  static const Part b = [] {
    Part p = third_base();
    p.version = 4, p.revises = 3, p.revised_kinds = {};
    p.effects.push_back({"item", "items: open, take, deliver, check, review, accept, withdraw, answer; rulings registered", {"item reopens"}});
    const std::map<std::string, std::vector<std::string>> reads = {
        {"request", {"id", "type", "work", "spec", "matter", "of", "due", "eligible", "position", "about", "subject", "kind", "reviews", "sponsor"}},
        {"commit", {"item"}}, {"offer", {"item"}}, {"attest", {"item", "result", "verdict"}}, {"answer", {"outcome", "reading", "overrules"}},
        {"admit", {"application", "seed"}}, {"cancel", {"item"}}, {"propose", {"petition"}}, {"deposit", {"petition"}}};
    for (auto& q : p.primitives) {
      static const std::set<std::string> on_items = {"request", "answer", "commit", "offer", "take", "deliver", "assess", "accept", "attest", "cancel", "admit", "propose",
                                                     "release", "hand over", "deposit"};
      if (on_items.count(q.kind) && std::find(q.effects.begin(), q.effects.end(), "item") == q.effects.end()) q.effects.push_back("item");
      if (const auto r = reads.find(q.kind); r != reads.end())
        for (const auto& f : r->second)
          if (std::find(q.reads.begin(), q.reads.end(), f) == q.reads.end()) q.reads.push_back(f);
    }
    p.triggers.push_back({"a taking past its term, or a returned delivery past its redelivery, reopens its item", "item reopens", "", 0, "", 0, true});
    return p;
  }();
  return b;
}

// The base taxonomy in force, its fifth version (schema/behaviour-5.json, decision 0013), a revision of the fourth that renames nothing:
// the author's kind and declared provider, features the registry carries; the category "reading by an operative" (a consult, use, loan
// or withdrawal by an operative whose provider is declared and not "local"); and the counts-as rule by which such a reading counts as a
// transmission to its provider, applied where a constitution declares that operatives transmit to their provider (the design record of
// 2026-11-27, Definition 45): an agent that reads a holding sends it to the model that reads it.
inline const Part& base() {
  static const Part b = [] {
    Part p = fourth_base();
    p.version = 5, p.revises = 4, p.revised_kinds = {};
    p.features.push_back({"author kind", "categorical", "", {"person", "organisation", "operative", "community"}, 1, 1, "the registry", "A"});
    p.features.push_back({"author provider", "categorical", "", {}, 1, 1, "the registry", "A"});
    p.conduct.push_back({"reading by an operative",
                         read_predicate("kind is one of 'consult', 'use', 'lend', 'withdraw' and author-kind is 'operative' and author-provider is present and not author-provider is 'local'"),
                         false});
    p.counts_as.push_back({"a reading by an operative reaches its provider", "reading by an operative", "transmission", "author-provider", "operatives transmit to their provider"});
    return p;
  }();
  return b;
}

// ---- A taxonomy in force --------------------------------------------------------------------------------------------------------

// The taxonomy in force under a constitution's part: the ground's primitives, classes and effect primitives always; the base's
// declared kinds, features, categories and standards when the part extends the base; and the part's own declarations.
//
// Since decision 0010 the base has levels, and since decision 0011 the base in force is the third version: its levels, facets and bridges are the ground's and always in force,
// its practices with the base. A part that extends "base 1" is read under the second version, carried along the base's revision,
// which renames nothing a part may name (kinds of the ground, declared kinds, features, categories); one that extends "base 2" names
// the second version itself. A community declares practices of its own, and never levels, facets or bridges.
inline Part in_force(const Part& part, bool declared) {
  const Part& b = base();
  Part t;
  t.name = declared ? part.name : "ground";
  t.version = declared ? part.version : 1;
  t.classes = b.classes, t.effects = b.effects, t.primitives = b.primitives;
  t.levels = b.levels, t.facets = b.facets, t.bridges = b.bridges;
  const bool extends = !declared || part.extends == "base 1" || part.extends == "base 2" || part.extends == "base 3" || part.extends == "base 4" || part.extends == "base 5";
  if (extends) t.declared = b.declared, t.features = b.features, t.conduct = b.conduct, t.recognition = b.recognition, t.triggers = b.triggers, t.practices = b.practices;
  // The base's counts-as rules apply under declarations a constitution makes (decision 0013): always in force, with the categories and
  // features they read, and inert until the declaration is made.
  for (const auto& r : b.counts_as) {
    t.counts_as.push_back(r);
    if (const Conduct* c = b.category(r.conduct); c && !t.category(c->name)) t.conduct.push_back(*c);
    for (const char* f : {"author kind", "author provider"})
      if (!t.feature(f))
        if (const Feature* g = b.feature(f)) t.features.push_back(*g);
  }
  if (!declared) return t;
  t.format = part.format, t.extends = part.extends, t.nearness = part.nearness, t.unread = part.unread;
  t.revises = part.revises, t.revised_kinds = part.revised_kinds;
  if (!part.levels.empty() || !part.facets.empty() || !part.bridges.empty())
    t.unread.push_back("a community's part declares levels, facets or bridges, which are the base's: it may declare practices of its own");
  for (const auto& x : part.practices) t.practices.push_back(x);
  for (const auto& x : part.declared) t.declared.push_back(x);
  for (const auto& x : part.features) t.features.push_back(x);
  for (const auto& x : part.conduct) t.conduct.push_back(x);
  for (const auto& x : part.counts_as) t.counts_as.push_back(x);
  for (const auto& x : part.triggers) t.triggers.push_back(x);
  for (const auto& x : part.recognition) t.recognition.push_back(x);
  return t;
}

// The kinds a declared kind counts as, from it to its primitive; empty when it reaches none (an unknown kind or a cycle).
inline std::vector<std::string> path(const Part& t, const std::string& kind) {
  std::vector<std::string> out;
  std::string at = kind;
  for (std::size_t steps = 0; steps <= t.declared.size() + 1; steps++) {
    out.push_back(at);
    if (t.primitive(at)) return out;
    const Declared* d = t.declared_kind(at);
    if (!d) return {};
    at = d->counts_as;
  }
  return {};
}
inline std::string primitive_of(const Part& t, const std::string& kind) {
  const auto p = path(t, kind);
  return p.empty() ? std::string() : p.back();
}

// ---- Admission by form ------------------------------------------------------------------------------------------------------------

namespace detail {
inline std::set<std::string> episode_features() {
  return {"party", "bearer", "provider", "model", "community", "node", "operation", "kind", "turn", "inputs", "outputs", "holdings touched", "items touched"};
}
// Whether a predicate reads only features the taxonomy knows: those an act carries, an episode's own, and the declared features.
inline std::string unknown_feature(const Predicate& p, const Part& t) {
  const auto own = episode_features();
  for (const Atom& a : p.atoms) {
    if (a.op == Atom::Op::holds || carried(a.feature)) continue;
    const std::string n = feature_name(a.feature);
    if (!own.count(n) && !t.feature(n)) return a.feature;
  }
  return {};
}
}  // namespace detail

// The level of description an atom reads (decision 0010): the level of the feature it names (a field of an act, "field.x", naming the
// feature x when the taxonomy declares one), conduct's when the feature declares none, and N for the positions an author holds.
inline std::string level_read(const Atom& a, const Part& t) {
  if (a.op == Atom::Op::holds) return "N";
  std::string n = feature_name(a.feature);
  if (a.feature.rfind("field.", 0) == 0) n = n.substr(6);
  return t.level_of_feature(n);
}
// Whether a condition reads epistemic behaviour alone: every atom reads a feature of level E (Definition 50). No norm attaches an
// effect to such a condition (invariant G10, freedom of thought).
inline bool epistemic_only(const Predicate& p, const Part& t) {
  if (!p.ok() || p.atoms.empty()) return false;
  for (const Atom& a : p.atoms)
    if (level_read(a, t) != "E") return false;
  return true;
}

namespace detail {
// Every fault of form of the taxonomy in levels: the five levels of the ground, each once, with a known register; facets, bridges and
// practices that name known levels; a practice's links admitted by their bridges between its slots' levels, and its slots joined.
inline void level_faults(const Part& t, std::set<std::string>& idents, std::vector<std::string>& out) {
  static const std::set<std::string> registers = {"Manifestational", "Relational", "Ontological"};
  if (t.levels.empty()) {
    if (!t.facets.empty() || !t.bridges.empty()) out.push_back("facets and bridges cross levels, and the taxonomy declares none");
    for (const auto& f : t.features)
      if (!f.level.empty()) out.push_back("the feature " + f.name + " describes level " + f.level + ", and the taxonomy declares no levels");
    if (!t.practices.empty()) out.push_back("practices are templates over levels, and the taxonomy declares none");
    return;
  }
  std::set<std::string> letters;
  for (const auto& l : t.levels) {
    if (l.letter.size() != 1 || l.letter[0] < 'A' || l.letter[0] > 'Z' || !letters.insert(l.letter).second) out.push_back("the level " + l.name + " has no letter of its own");
    if (!nameable(l.name)) out.push_back("the level \"" + l.name + "\" is not a name of letters, digits, spaces and hyphens");
    if (!registers.count(l.register_)) out.push_back("the level " + l.name + " is in no register: Manifestational, Relational or Ontological");
    if (!l.indexed.empty() && l.indexed != "structure" && l.indexed != "order") out.push_back("the level " + l.name + " is indexed by a structure, an order, or nothing");
    for (const auto& f : l.forms)
      if (!nameable(f)) out.push_back("the form \"" + f + "\" of " + l.name + " is not a name of letters, digits, spaces and hyphens");
  }
  for (const char* g : {"E", "O", "M", "S", "N"})
    if (!letters.count(g)) out.push_back(std::string("a taxonomy in levels holds the five levels of the ground, E, O, M, S and N; it lacks ") + g);
  if (const Level* n = t.level("N"); n && n->forms.empty()) out.push_back("the level of normative conduct names its forms, the first holding the kinds of act");
  if (const Level* s = t.level("S"); s && s->indexed.empty()) out.push_back("a description of social conduct names the structure that makes it: the level is indexed");
  if (const Level* n = t.level("N"); n && n->indexed.empty()) out.push_back("a description of normative conduct names the order that makes it: the level is indexed");
  std::set<std::string> fnames;
  for (const auto& f : t.features) {
    fnames.insert(f.name);
    if (!f.level.empty() && f.level != "O" && f.level != "E") out.push_back("the feature " + f.name + " describes level " + f.level + ": features describe conduct (O) or epistemic behaviour (E)");
  }
  std::set<std::string> seen;
  for (const auto& f : t.facets) {
    if (!nameable(f.name) || !seen.insert(f.name).second || fnames.count(f.name)) out.push_back("the facet " + f.name + " is not a name of its own");
    std::set<std::string> vals;
    for (const auto& v : f.values)
      if (!nameable(v) || !vals.insert(ident(v)).second) out.push_back("the facet " + f.name + " has a value that is not a name of its own: " + v);
    if (f.values.size() < (f.ordered ? 2u : 1u)) out.push_back("the facet " + f.name + " has too few values");
    if (f.levels.empty()) out.push_back("the facet " + f.name + " describes no level");
    for (const auto& l : f.levels)
      if (!t.level(l)) out.push_back("the facet " + f.name + " describes level " + l + ", which the taxonomy lacks");
  }
  seen.clear();
  for (const auto& b : t.bridges) {
    if (!nameable(b.name) || !seen.insert(b.name).second || !idents.insert(ident(b.name)).second) out.push_back("the bridge " + b.name + " is not a name of its own");
    if (b.pairs.empty()) out.push_back("the bridge " + b.name + " joins no levels");
    for (const auto& [from, to] : b.pairs)
      if (!t.level(from) || !t.level(to) || from == to) out.push_back("the bridge " + b.name + " joins " + from + " to " + to + ": two levels of the taxonomy, apart");
  }
  seen.clear();
  for (const auto& p : t.practices) {
    if (!nameable(p.name) || !seen.insert(p.name).second) out.push_back("the practice " + p.name + " is not a name of its own");
    std::map<std::string, std::string> level_of;
    std::size_t required = 0;
    for (const auto& s : p.slots) {
      if (s.role.empty() || !level_of.emplace(s.role, s.level).second) out.push_back("the practice " + p.name + " names a role twice, or none");
      if (!t.level(s.level)) out.push_back("the practice " + p.name + " places " + s.role + " at level " + s.level + ", which the taxonomy lacks");
      required += s.optional ? 0 : 1;
    }
    if (p.slots.size() < 2 || required == 0) out.push_back("the practice " + p.name + " is a pattern of at least two descriptions, one of them required");
    for (const auto& k : p.links) {
      const Bridge* b = t.bridge(k.bridge);
      const auto f = level_of.find(k.from), g = level_of.find(k.to);
      if (!b) out.push_back("the practice " + p.name + " links by " + k.bridge + ", which is no bridge");
      else if (f == level_of.end() || g == level_of.end()) out.push_back("the practice " + p.name + " links a role it lacks");
      else if (std::find(b->pairs.begin(), b->pairs.end(), std::pair<std::string, std::string>{f->second, g->second}) == b->pairs.end())
        out.push_back("the practice " + p.name + " links " + k.from + " (" + f->second + ") to " + k.to + " (" + g->second + ") by " + k.bridge + ", which does not join those levels");
    }
    // The slots joined by the links, so that every instance is found from any one of them.
    std::set<std::string> reached;
    if (!p.slots.empty()) reached.insert(p.slots.front().role);
    for (bool grew = true; grew;) {
      grew = false;
      for (const auto& k : p.links)
        if (reached.count(k.from) != reached.count(k.to)) reached.insert(k.from), reached.insert(k.to), grew = true;
    }
    if (reached.size() != level_of.size()) out.push_back("the practice " + p.name + " has slots its links do not join");
    // Its required slots joined among themselves, so that an instance is found whatever optional slots it leaves empty.
    std::set<std::string> needed, joined;
    for (const auto& s : p.slots)
      if (!s.optional) needed.insert(s.role);
    if (!needed.empty()) joined.insert(*needed.begin());
    for (bool grew = true; grew;) {
      grew = false;
      for (const auto& k : p.links)
        if (needed.count(k.from) && needed.count(k.to) && joined.count(k.from) != joined.count(k.to)) joined.insert(k.from), joined.insert(k.to), grew = true;
    }
    if (joined.size() != needed.size()) out.push_back("the practice " + p.name + " has required slots that its links among required slots do not join");
  }
}
}  // namespace detail

// Every fault of form of a taxonomy in force (the base, or the base and a part): each declared kind counts as a kind in force and
// reaches a primitive with no cycle, keeps its force, is a name of its own, maps no field both ways, and states conditions that read
// and read only what an act carries; each feature has a known space; each category reads and reads known features; each counts-as rule
// reads, through a decidable category the fold can read, a status the norms know; each trigger produces only an effect the ground
// marks as by law, in its form; each standard names a grade.
inline std::vector<std::string> faults(const Part& t) {
  std::vector<std::string> out;
  for (const auto& u : t.unread) out.push_back(u);
  if (t.format != "noetarium.behaviour/1") out.push_back("the taxonomy's format is not noetarium.behaviour/1");
  if (!t.extends.empty() && t.extends != "base 1" && t.extends != "base 2" && t.extends != "base 3" && t.extends != "base 4" && t.extends != "base 5")
    out.push_back("a taxonomy extends the base, version 1, 2, 3, 4 or 5, or nothing");
  std::set<std::string> names, idents = {"behaviour", "normative_act", "conduct", "event", "state", "measured_state", "observation"};
  for (const auto& l : t.levels) {
    idents.insert(ident(l.name));
    for (const auto& f : l.forms) idents.insert(ident(f));
  }
  for (const auto& c : t.classes) idents.insert(ident(c));
  for (const auto& p : t.primitives) names.insert(p.kind), idents.insert(ident(p.kind));
  for (const auto& d : t.declared) {
    if (!nameable(d.kind)) {
      out.push_back("the declared kind \"" + d.kind + "\" is not a name of letters, digits, spaces and hyphens");
      continue;
    }
    if (t.primitive(d.kind)) out.push_back("the declared kind " + d.kind + " is a kind of the ground");
    else if (!names.insert(d.kind).second || !idents.insert(ident(d.kind)).second) out.push_back("the declared kind " + d.kind + " is declared twice, or shares its identifier");
    if (!t.declared_kind(d.counts_as) && !t.primitive(d.counts_as)) {
      out.push_back("the declared kind " + d.kind + " counts as " + d.counts_as + ", which is no kind in force");
      continue;
    }
    const auto p = path(t, d.kind);
    if (p.empty()) {
      out.push_back("the declared kind " + d.kind + " counts as kinds that never reach a primitive: a cycle");
      continue;
    }
    if (t.primitive(p.back())->force != d.force)
      out.push_back("the declared kind " + d.kind + " is " + std::string(acts::word(d.force)) + " and counts as " + p.back() + ", which is " + std::string(acts::word(t.primitive(p.back())->force)) +
                    ": a declared kind keeps the force of what it counts as");
    for (const auto& [field, from] : d.renamed)
      if (d.fixed.count(field)) out.push_back("the declared kind " + d.kind + " both renames and fixes the field " + field);
    for (const auto& w : d.valid_when) {
      if (!w.ok()) out.push_back("a condition of " + d.kind + " does not read: \"" + w.text + "\": " + w.error);
      else if (!fold_readable(w)) out.push_back("a condition of " + d.kind + " reads what no act carries: \"" + w.text + "\"");
      else if (epistemic_only(w, t))
        out.push_back("a condition of " + d.kind + " reads epistemic behaviour alone: \"" + w.text + "\": no norm attaches an effect to epistemic behaviour alone");
    }
  }
  detail::level_faults(t, idents, out);
  static const std::set<std::string> spaces = {"categorical", "ordinal", "real", "digests", "set", "embedding"};
  std::set<std::string> fnames;
  for (const auto& f : t.features) {
    if (!fnames.insert(f.name).second) out.push_back("the feature " + f.name + " is declared twice");
    if (!spaces.count(f.space)) out.push_back("the feature " + f.name + " has no known space: categorical, ordinal, real, digests, set or embedding");
    if (f.space == "ordinal" && f.vocabulary.size() < 2) out.push_back("the ordinal feature " + f.name + " orders no vocabulary");
    if (f.space == "real" && (f.unit.empty() || f.scale <= 0)) out.push_back("the real feature " + f.name + " names no unit, or no scale above nought");
  }
  std::set<std::string> cnames;
  for (const auto& c : t.conduct) {
    if (!nameable(c.name)) out.push_back("the category of conduct \"" + c.name + "\" is not a name of letters, digits, spaces and hyphens");
    else if (!cnames.insert(c.name).second || !idents.insert(ident(c.name)).second) out.push_back("the category of conduct " + c.name + " is declared twice, or shares an identifier with a kind");
    if (!c.where.ok()) out.push_back("the category " + c.name + " does not read: \"" + c.where.text + "\": " + c.where.error);
    else if (const auto u = detail::unknown_feature(c.where, t); !u.empty()) out.push_back("the category " + c.name + " reads the feature " + u + ", which is not recorded");
  }
  for (const auto& r : t.counts_as) {
    const Conduct* c = t.category(r.conduct);
    if (!c) out.push_back("the counts-as rule " + r.rule + " reads the category " + r.conduct + ", which is not declared");
    else if (c->analytic) out.push_back("the counts-as rule " + r.rule + " reads the analytic category " + r.conduct + ": a category a model defines never decides");
    else if (!fold_readable(c->where)) out.push_back("the counts-as rule " + r.rule + " reads the category " + r.conduct + ", which reads features no act carries: the fold cannot read it");
    else if (epistemic_only(c->where, t))
      out.push_back("the counts-as rule " + r.rule + " reads the category " + r.conduct + ", which reads epistemic behaviour alone: no norm attaches an effect to epistemic behaviour alone");
    if (r.status != "transmission") out.push_back("the counts-as rule " + r.rule + " makes the status " + r.status + ", which no norm reads");
    if (!r.where_declared.empty() && !known_declaration(r.where_declared))
      out.push_back("the counts-as rule " + r.rule + " applies where \"" + r.where_declared + "\" is declared, which no constitution declares: operatives transmit to their provider");
    if (!carried(r.value)) out.push_back("the counts-as rule " + r.rule + " takes its value from " + r.value + ", which no act carries");
  }
  for (const auto& g : t.triggers) {
    const Effect* by = nullptr;
    for (const auto& e : t.effects)
      if (std::find(e.by_law.begin(), e.by_law.end(), g.then) != e.by_law.end()) by = &e;
    if (!by) {
      out.push_back("the trigger rule " + g.rule + " produces " + g.then + ", which the ground does not mark as an effect by law");
      continue;
    }
    if (g.then == "end position" && (g.position.empty() || g.held_for <= 0)) out.push_back("the trigger rule " + g.rule + " ends a position held for more than a number of turns, and names neither");
    if (g.then == "lapse" && g.past_due_by < 0) out.push_back("the trigger rule " + g.rule + " lapses a commitment a number of turns past its due, never fewer than nought");
    if (g.then == "loan returns" && !g.by_ground) out.push_back("the trigger rule " + g.rule + " returns loans, which the ground does at their term itself");
    if (g.then == "agreement ends" && !g.by_ground) out.push_back("the trigger rule " + g.rule + " ends agreements, which the ground does at their term or notice itself");
    if (g.then == "item reopens" && !g.by_ground) out.push_back("the trigger rule " + g.rule + " reopens items, which the ground does when a taking's term or a redelivery runs out");
  }
  for (const auto& r : t.recognition)
    if (r.grade != "A" && r.grade != "B" && r.grade != "C") out.push_back("the standard of recognition of " + r.fact + " names no grade A, B or C");
  return out;
}

// ---- Declared kinds reduced to their primitives ---------------------------------------------------------------------------------

// An act of a declared kind read as an act of the primitive it reduces to: kind replaced along the chain, each declared kind's fields
// renamed and fixed in turn, the rest of its envelope kept. The chain of the act as it stands at each level is kept, so that each
// declared kind's conditions are read on the act as that kind sees it.
struct Reduced {
  bool ok = false;
  std::string reason;
  acts::Act act;                              // the institutional reading
  std::vector<std::pair<const Declared*, acts::Act>> levels;  // each declared kind with the act as it reads it
};

inline Reduced reduce(const Part& t, const acts::Act& a) {
  Reduced r;
  if (t.primitive(a.kind)) {
    r.ok = true, r.act = a;
    return r;
  }
  const auto p = path(t, a.kind);
  if (p.empty()) {
    r.reason = "the kind of act is unknown";
    return r;
  }
  acts::Act at = a;
  for (std::size_t k = 0; k + 1 < p.size(); k++) {
    const Declared* d = t.declared_kind(p[k]);
    r.levels.push_back({d, at});
    acts::Act next = at;
    next.kind = d->counts_as;
    for (const auto& [field, from] : d->renamed) {
      const auto v = at.fields.find(from);
      if (v == at.fields.end()) next.fields.erase(field);
      else next.fields[field] = v->second;
    }
    for (const auto& [field, value] : d->fixed) next.fields[field] = value;
    at = std::move(next);
  }
  r.ok = true, r.act = std::move(at);
  return r;
}

// ---- Revisions admitted directly ------------------------------------------------------------------------------------------------

// The image of a kind or a category under a revision: as renamed, else itself.
inline std::string image(const Part& revised, const std::string& name) {
  const auto it = revised.revised_kinds.find(name);
  return it == revised.revised_kinds.end() ? name : it->second;
}

// Every fault of a revision of a taxonomy in force (Definition 37): the revision is admitted by form and states the version it revises;
// each primitive keeps its name; each declared kind of the old has an image in the new that reduces to the same primitive (and so keeps
// its force), and the map of kinds keeps the order "counts as" (a functor of the two orders, decided on their covers); each category of
// conduct has an image whose region contains the old's (shown atom by atom); each feature kept keeps its space and its unit, and an
// ordinal feature the order of what it keeps.
inline std::vector<std::string> revision_faults(const Part& old_t, const Part& new_t) {
  std::vector<std::string> out = faults(new_t);
  if (new_t.revises != old_t.version || new_t.version <= old_t.version)
    out.push_back("the revision states that it revises version " + std::to_string(new_t.revises) + " into version " + std::to_string(new_t.version) + "; the taxonomy in force is version " +
                  std::to_string(old_t.version));
  for (const auto& [from, to] : new_t.revised_kinds)
    if (old_t.primitive(from)) out.push_back("the revision renames the kind of the ground " + from);
  // The taxonomy's roots and levels (decision 0010). The base's revision into levels recasts the first version's normative acts as the
  // constituted acts of level N, its conduct as level O and its observations as level M, and renames no other root; every other
  // revision renames no root, level or form, and keeps every level, facet value and bridge pair.
  std::set<std::string> roots = {"behaviour", "normative act", "conduct", "event", "state", "measured state", "observation"};
  for (const auto& l : old_t.levels) {
    roots.insert(l.name);
    for (const auto& f : l.forms) roots.insert(f);
  }
  const bool into_levels = old_t.levels.empty() && !new_t.levels.empty();
  if (into_levels) {
    const Level *n = new_t.level("N"), *o = new_t.level("O"), *m = new_t.level("M");
    const std::map<std::string, std::string> asked = {{"normative act", n && !n->forms.empty() ? n->forms.front() : std::string("?")}, {"conduct", o ? o->name : std::string("?")},
                                                      {"observation", m ? m->name : std::string("?")}};
    for (const auto& [from, to] : asked)
      if (image(new_t, from) != to) out.push_back("a revision into levels recasts the " + from + " of the first version as " + to + ", and this one sends it to " + image(new_t, from));
    for (const auto& [from, to] : new_t.revised_kinds)
      if (roots.count(from) && !asked.count(from)) out.push_back("a revision into levels renames no root but normative act, conduct and observation; it renames " + from);
  } else {
    for (const auto& [from, to] : new_t.revised_kinds)
      if (roots.count(from)) out.push_back("the revision renames the root or level " + from + ", which only the base's revision into levels recasts");
  }
  if (!old_t.levels.empty() && new_t.levels.empty()) out.push_back("the revision drops the levels of description");
  for (const auto& l : old_t.levels) {
    const Level* k = new_t.level(l.letter);
    if (!k || k->name != l.name || k->register_ != l.register_ || k->indexed != l.indexed) {
      out.push_back("the revision does not keep the level " + l.letter + ", " + l.name);
      continue;
    }
    for (const auto& f : l.forms)
      if (std::find(k->forms.begin(), k->forms.end(), f) == k->forms.end()) out.push_back("the revision drops the form " + f + " of " + l.name);
  }
  for (const auto& f : old_t.facets) {
    const Facet* g = new_t.facet(f.name);
    if (!g) {
      out.push_back("the revision drops the facet " + f.name);
      continue;
    }
    long last = -1;
    for (const auto& v : f.values) {
      const auto at = std::find(g->values.begin(), g->values.end(), v);
      if (at == g->values.end()) {
        out.push_back("the revision drops the value " + v + " of the facet " + f.name);
        continue;
      }
      const long rank = static_cast<long>(at - g->values.begin());
      if (f.ordered && (!g->ordered || rank < last)) out.push_back("the ordered facet " + f.name + " does not keep its order");
      last = rank;
    }
  }
  for (const auto& b : old_t.bridges) {
    const Bridge* c = new_t.bridge(b.name);
    for (const auto& p : b.pairs)
      if (!c || std::find(c->pairs.begin(), c->pairs.end(), p) == c->pairs.end()) out.push_back("the revision does not keep the bridge " + b.name + " from " + p.first + " to " + p.second);
  }
  for (const auto& d : old_t.declared) {
    const std::string img = image(new_t, d.kind);
    if (!new_t.declared_kind(img) && !new_t.primitive(img)) {
      out.push_back("the declared kind " + d.kind + " has no image (" + img + " is no kind of the revision)");
      continue;
    }
    if (primitive_of(old_t, d.kind) != primitive_of(new_t, img))
      out.push_back("the declared kind " + d.kind + " reduces to " + primitive_of(old_t, d.kind) + ", and its image " + img + " to " + primitive_of(new_t, img));
    const std::string parent_img = image(new_t, d.counts_as);
    const auto up = path(new_t, img);
    if (std::find(up.begin(), up.end(), parent_img) == up.end())
      out.push_back("the map of kinds is no functor of the order counts as: " + d.kind + " counts as " + d.counts_as + ", and " + img + " does not count as " + parent_img);
  }
  for (const auto& c : old_t.conduct) {
    const std::string img = image(new_t, c.name);
    const Conduct* n = new_t.category(img);
    if (!n) out.push_back("the category " + c.name + " has no image (" + img + " is no category of the revision)");
    else if (!contained(c.where, n->where)) out.push_back("the category " + c.name + " is not shown contained in its image " + img + ": \"" + c.where.text + "\" within \"" + n->where.text + "\"");
  }
  for (const auto& f : old_t.features) {
    const Feature* n = new_t.feature(f.name);
    if (!n) continue;
    if (n->space != f.space || n->unit != f.unit) out.push_back("the feature " + f.name + " changes its space or its unit");
    if (f.space == "ordinal") {
      long last = -1;
      for (const auto& v : f.vocabulary) {
        const auto at = std::find(n->vocabulary.begin(), n->vocabulary.end(), v);
        if (at == n->vocabulary.end()) continue;
        const long rank = static_cast<long>(at - n->vocabulary.begin());
        if (rank < last) out.push_back("the ordinal feature " + f.name + " does not keep its order");
        last = rank;
      }
    }
  }
  return out;
}

// ---- Nearness of conduct ----------------------------------------------------------------------------------------------------------

// The distance of two episodes in the space of conduct (Definition 26): each declared feature's distance in [0, 1] (categorical 0 or
// 1, ordinal by rank, real |a - b| / scale capped at 1, digests and sets by Jaccard's distance), combined by a weighted sum or the
// greatest. Embeddings are left to analysis.
inline double distance(const Part& t, const Episode& x, const Episode& y) {
  const Features a = features_of(x), b = features_of(y);
  double total = 0;
  for (const auto& f : t.features) {
    double d = 0;
    if (f.space == "categorical" || f.space == "ordinal") {
      const auto u = a.scalars.find(f.name), v = b.scalars.find(f.name);
      const std::string p = u == a.scalars.end() ? std::string() : u->second, q = v == b.scalars.end() ? std::string() : v->second;
      if (f.space == "categorical" || f.vocabulary.size() < 2) d = p == q ? 0 : 1;
      else {
        const auto i = std::find(f.vocabulary.begin(), f.vocabulary.end(), p), j = std::find(f.vocabulary.begin(), f.vocabulary.end(), q);
        if (i == f.vocabulary.end() || j == f.vocabulary.end()) d = p == q ? 0 : 1;
        else d = std::abs(static_cast<double>(i - j)) / static_cast<double>(f.vocabulary.size() - 1);
      }
    } else if (f.space == "real") {
      const auto u = a.numbers.find(f.name), v = b.numbers.find(f.name);
      const double p = u == a.numbers.end() ? 0.0 : static_cast<double>(u->second), q = v == b.numbers.end() ? 0.0 : static_cast<double>(v->second);
      d = std::min(1.0, std::abs(p - q) / static_cast<double>(f.scale));
    } else if (f.space == "digests" || f.space == "set") {
      const auto u = a.sets.find(f.name), v = b.sets.find(f.name);
      const std::set<std::string> p = u == a.sets.end() ? std::set<std::string>{} : u->second, q = v == b.sets.end() ? std::set<std::string>{} : v->second;
      std::size_t both = 0;
      for (const auto& s : p) both += q.count(s);
      const std::size_t either = p.size() + q.size() - both;
      d = either == 0 ? 0 : 1.0 - static_cast<double>(both) / static_cast<double>(either);
    } else
      continue;
    total = t.nearness == "max" ? std::max(total, d) : total + static_cast<double>(f.weight) * d;
  }
  return total;
}

// The categories of conduct an episode lies in, decidable ones only (analytic categories are read in analysis, as observations of grade
// C).
inline std::vector<std::string> categorise(const Part& t, const Episode& e) {
  std::vector<std::string> out;
  const Features f = features_of(e);
  for (const auto& c : t.conduct)
    if (!c.analytic && c.where.ok() && holds(c.where, f)) out.push_back(c.name);
  return out;
}

// The omissions of a party with respect to a duty (Definition 28): each window opened by an episode of the opening category and closed
// `within` turns later, in which the party has no episode of the required category. Each is reported by the turn that opened it.
inline std::vector<long long> omissions(const Part& t, const std::vector<Episode>& journal, const std::string& opens, const std::string& requires_, const std::string& party, long long within) {
  std::vector<long long> out;
  const Conduct* o = t.category(opens);
  const Conduct* r = t.category(requires_);
  if (!o || !r) return out;
  for (const auto& e : journal) {
    if (!holds(o->where, features_of(e))) continue;
    bool met = false;
    for (const auto& f : journal) met = met || (f.party == party && f.turn > e.turn && f.turn <= e.turn + within && holds(r->where, features_of(f)));
    if (!met) out.push_back(e.turn);
  }
  return out;
}

// ---- Observations and their standing ------------------------------------------------------------------------------------------------

// An observation about conduct, entered as an act of evidence (Definition 30): who made it and their origin, what it is about, its
// grade (A a structured record with digests, B a text with its passage, C an account from memory or an analytic category) and what it
// says.
struct Observation {
  std::string author, origin, object, grade, value;
};
inline int grade_rank(const std::string& g) { return g == "A" ? 3 : g == "B" ? 2 : g == "C" ? 1 : 0; }

// The standing a declared standard gives a fact about conduct of a subject (Definition 35), by the base's rule: established when an
// observation meets the standard (its grade at least the standard's, its author of another origin than the subject's where asked) and
// none meeting it says otherwise; disputed when observations that meet it disagree; attested when an observation of grade B or better
// exists that does not meet it; proposed otherwise.
inline std::string standing(const std::vector<Observation>& obs, const Standard& s, const std::string& subject_origin) {
  std::set<std::string> said;
  bool attested = false;
  for (const auto& o : obs) {
    const bool meets = grade_rank(o.grade) >= grade_rank(s.grade) && (!s.another_origin || o.origin != subject_origin);
    if (meets) said.insert(o.value);
    else attested = attested || grade_rank(o.grade) >= 2;
  }
  if (said.size() > 1) return "disputed";
  if (said.size() == 1) return "established";
  return attested ? "attested" : "proposed";
}

}  // namespace thesmos::behaviour
