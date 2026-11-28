// Procedures: decision rules with their unit of count, exact shares, and a decision's sensitivity to the partition of origins.
//
// A rule of aggregation declares its unit of count. By operative, each voter counts one. By origin, each origin (under the
// declared relation of control) weighs one, divided equally among its voters, so that an origin's weight in favour is the
// share of its voters in favour. By person, the same over origins that are persons, others left uncounted; by community, over
// origins that are communities (the Political Authority and Evaluative Authority papers' two-level weighting).
//
// Proposition 6, as corrected after the independent reading of 2026-11-25. (a) Every origin weighs one, whatever the number of
// its voters. (b) The share in favour is unchanged when an origin whose voters vote alike replaces them by any number of voters
// that vote as they did. (c) When an origin's voters divide, adding voters to it moves its own weight in favour within [0, 1]
// and so moves the share by at most 1/O, O the number of origins counted; under a count by operative an origin with n of N
// voters holds n/N. The count rests on the declared relation of control: undeclared, copies count as origins.
//
// Decision 0013 adds two units and two forms of rule. By provider, each provider (the origin under provision of the voter: an
// operative's declared provider, any other party itself) weighs one, divided among its voters, so that a provider's agents carry
// one voice whatever their number (Proposition 6 holds of it word for word, with provision for control). By weight, each origin
// under control weighs its declared weight, an exact rational, divided among its voters; an origin with no declared weight is not
// counted. A rule may be a threshold, "at least p/q" or "more than p/q", or a double majority, "double majority of members at least
// p/q and weight at least r/s": passed when the origins counted by weight, each weighing one, and their weights both reach their
// thresholds (the members of a treaty organisation and their declared weights, after the Treaty on European Union, Article 16(4)).
// Shares are of the ballots cast, as every rule of the kernel reads them.

#pragma once

#include <thesmos/kernel/kernel.hpp>

#include <morphe/arithmetic/rational.hpp>

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace thesmos::procedures {

using kernel::Kind;
using kernel::Parties;
using morphe::arithmetic::Rational;

enum class Unit { person, origin, operative, community, provider, weight };

inline std::optional<Unit> unit_named(std::string_view w) {
  if (w == "person") return Unit::person;
  if (w == "origin") return Unit::origin;
  if (w == "operative") return Unit::operative;
  if (w == "community") return Unit::community;
  if (w == "provider") return Unit::provider;  // decision 0013
  if (w == "weight") return Unit::weight;      // decision 0013
  return std::nullopt;
}

// Declared weights: a party (an origin under control) -> its weight, an exact rational, never negative.
using Weights = std::map<std::string, Rational>;

struct Count {
  Rational favour;        // the weight in favour
  long long counted = 0;  // how many units were counted (voters by operative, providers by provider, origins otherwise)
  long long against = 0;  // how many counted units hold at least one vote against
  Rational total;         // the weight counted: the number of units counted, or under a count by weight the weights of the origins counted
  Rational share() const { return total == Rational(0) ? Rational(0) : favour / total; }
};

// A ballot: who cast it, the party that answers for it (the root of the voter's mandate, or the voter), and its sense. By origin,
// a ballot counts for the origin of the party that answers for it, so that an envoy who votes under a community's mandate votes
// for the community. By provider, it counts for the voter's provider, the party whose model cast it.
struct Ballot {
  std::string voter, bearer;
  bool yes = false;
};

// The weight in favour among ballots under a unit of count (weights read by the count by weight alone).
inline Count count(const Parties& ps, const std::vector<Ballot>& ballots, Unit unit, const Weights& weights = {}) {
  Count c;
  if (unit == Unit::operative) {
    for (const Ballot& b : ballots) {
      c.counted++;
      if (b.yes) c.favour = c.favour + Rational(1);
      else c.against++;
    }
    c.total = Rational(c.counted);
    return c;
  }
  std::map<std::string, std::pair<long long, long long>> groups;  // origin or provider -> (in favour, voters)
  for (const Ballot& b : ballots) {
    std::string o;
    if (unit == Unit::provider) o = kernel::origin(ps, b.voter, kernel::Criterion::provision);
    else {
      o = kernel::origin(ps, b.bearer.empty() ? b.voter : b.bearer);
      const Kind k = ps.at(o).kind;
      if (unit == Unit::person && k != Kind::person) continue;
      if (unit == Unit::community && k != Kind::community) continue;
      if (unit == Unit::weight) {
        const auto w = weights.find(o);
        if (w == weights.end() || !(Rational(0) < w->second)) continue;
      }
    }
    auto& g = groups[o];
    g.first += b.yes ? 1 : 0;
    g.second += 1;
  }
  for (const auto& [o, g] : groups) {
    const Rational w = unit == Unit::weight ? weights.at(o) : Rational(1);
    c.counted++;
    c.total = c.total + w;
    c.favour = c.favour + w * Rational(g.first) / Rational(g.second);
    if (g.first < g.second) c.against++;
  }
  return c;
}

// The same, for votes cast in each voter's own name (party -> in favour).
inline Count count(const Parties& ps, const std::map<std::string, bool>& votes, Unit unit, const Weights& weights = {}) {
  std::vector<Ballot> ballots;
  for (const auto& [voter, yes] : votes) ballots.push_back({voter, voter, yes});
  return count(ps, ballots, unit, weights);
}

inline Rational share(const Parties& ps, const std::map<std::string, bool>& votes, Unit unit, const Weights& weights = {}) { return count(ps, votes, unit, weights).share(); }

// ---- Rules ------------------------------------------------------------------------------------------------------------------------

// A threshold on a share: at least `at`, or more than `at` when strict.
struct Threshold {
  Rational at;
  bool strict = false;
  bool met(const Rational& s) const { return strict ? at < s : !(s < at); }
  bool operator==(const Threshold&) const = default;
};

// The rules the kernel knows: any one; majority (more than half); two thirds (at least); unanimity; consensus as the absence of a
// formal objection (no counted unit against); and since decision 0013 a threshold at an exact fraction and a double majority.
struct Rule {
  enum class Form { any_one, majority, two_thirds, unanimity, consensus, threshold, double_majority };
  Form form = Form::majority;
  Threshold share;            // a threshold
  Threshold members, weight;  // a double majority
  std::string written;        // a consensus as written (its qualifier kept)
};

namespace detail {
// A fraction p/q, or a whole number, between 0 and 1: digits only, q positive.
inline std::optional<Rational> fraction(std::string_view w) {
  auto digits = [](std::string_view x) { return !x.empty() && x.size() <= 9 && x.find_first_not_of("0123456789") == std::string_view::npos; };
  const auto slash = w.find('/');
  if (slash == std::string_view::npos) {
    if (!digits(w)) return std::nullopt;
    const long long n = std::stoll(std::string(w));
    if (n > 1) return std::nullopt;
    return Rational(n);
  }
  const std::string_view p = w.substr(0, slash), q = w.substr(slash + 1);
  if (!digits(p) || !digits(q)) return std::nullopt;
  const long long a = std::stoll(std::string(p)), b = std::stoll(std::string(q));
  if (b == 0 || a > b) return std::nullopt;
  return Rational(a, b);
}
// "at least p/q", "more than p/q", or a bare "p/q" (at least).
inline std::optional<Threshold> threshold(std::string_view w) {
  bool strict = false;
  if (w.rfind("at least ", 0) == 0) w.remove_prefix(9);
  else if (w.rfind("more than ", 0) == 0) w.remove_prefix(10), strict = true;
  const auto f = fraction(w);
  if (!f) return std::nullopt;
  return Threshold{*f, strict};
}
inline std::string words(const Threshold& t) { return std::string(t.strict ? "more than " : "at least ") + t.at.str(); }
}  // namespace detail

// A rule read from its text, or nothing when the kernel does not know it.
inline std::optional<Rule> read_rule(std::string_view text) {
  Rule r;
  if (text == "any one") r.form = Rule::Form::any_one;
  else if (text == "majority") r.form = Rule::Form::majority;
  else if (text == "two thirds") r.form = Rule::Form::two_thirds;
  else if (text == "unanimity") r.form = Rule::Form::unanimity;
  else if (text.rfind("consensus", 0) == 0) r.form = Rule::Form::consensus, r.written = std::string(text);
  else if (text.rfind("double majority of members ", 0) == 0) {
    std::string_view rest = text.substr(27);
    const auto and_at = rest.find(" and weight ");
    if (and_at == std::string_view::npos) return std::nullopt;
    const auto m = detail::threshold(rest.substr(0, and_at)), w = detail::threshold(rest.substr(and_at + 12));
    if (!m || !w) return std::nullopt;
    r.form = Rule::Form::double_majority, r.members = *m, r.weight = *w;
  } else if (text.rfind("at least ", 0) == 0 || text.rfind("more than ", 0) == 0) {
    const auto t = detail::threshold(text);
    if (!t) return std::nullopt;
    r.form = Rule::Form::threshold, r.share = *t;
  } else
    return std::nullopt;
  return r;
}

// A rule's canonical text, which read_rule reads back to the same rule.
inline std::string text(const Rule& r) {
  switch (r.form) {
    case Rule::Form::any_one: return "any one";
    case Rule::Form::majority: return "majority";
    case Rule::Form::two_thirds: return "two thirds";
    case Rule::Form::unanimity: return "unanimity";
    case Rule::Form::consensus: return r.written.empty() ? "consensus" : r.written;
    case Rule::Form::threshold: return detail::words(r.share);
    case Rule::Form::double_majority: return "double majority of members " + detail::words(r.members) + " and weight " + detail::words(r.weight);
  }
  return "majority";
}

// The canonical text of a double majority written as {"members": <threshold>, "weight": <threshold>}, or nothing.
inline std::optional<std::string> double_majority(std::string_view members, std::string_view weight) {
  const auto m = detail::threshold(members), w = detail::threshold(weight);
  if (!m || !w) return std::nullopt;
  Rule r;
  r.form = Rule::Form::double_majority, r.members = *m, r.weight = *w;
  return text(r);
}

inline bool known_rule(std::string_view rule) { return read_rule(rule).has_value(); }

// Whether a count passes a rule of a single share (a double majority needs two counts: decide).
inline bool passes(const Count& c, const Rule& r) {
  if (c.counted == 0) return false;
  const Rational s = c.share();
  switch (r.form) {
    case Rule::Form::any_one: return Rational(0) < s;
    case Rule::Form::majority: return Rational(1, 2) < s;
    case Rule::Form::two_thirds: return !(s < Rational(2, 3));
    case Rule::Form::unanimity: return s == Rational(1);
    case Rule::Form::consensus: return c.against == 0;
    case Rule::Form::threshold: return r.share.met(s);
    case Rule::Form::double_majority: return false;
  }
  return false;
}
inline bool passes(const Count& c, std::string_view rule) {
  const auto r = read_rule(rule);
  return r && passes(c, *r);
}

// A decision's tally: the count under its unit and, for a double majority, the count of members (the origins counted by weight,
// each weighing one) beside the count by weight.
struct Tally {
  Count count, members;
  bool passed = false;
};

inline Tally decide(const Parties& ps, const std::vector<Ballot>& ballots, Unit unit, const Rule& r, const Weights& weights = {}) {
  Tally t;
  if (r.form != Rule::Form::double_majority) {
    t.count = count(ps, ballots, unit, weights);
    t.passed = passes(t.count, r);
    return t;
  }
  t.count = count(ps, ballots, Unit::weight, weights);
  Weights ones;
  for (const auto& [o, w] : weights)
    if (Rational(0) < w) ones[o] = Rational(1);
  t.members = count(ps, ballots, Unit::weight, ones);
  t.passed = t.count.counted > 0 && r.members.met(t.members.share()) && r.weight.met(t.count.share());
  return t;
}

// A decision read under the declared relation of control and with every control relation removed, so that a result which turns
// on an undeclared relation is seen to be arguable.
struct Sensitivity {
  Count declared, undeclared;
  bool passes_declared = false, passes_undeclared = false;
  bool turns() const { return passes_declared != passes_undeclared; }
};

inline Sensitivity sensitivity(const Parties& ps, const std::vector<Ballot>& ballots, Unit unit, std::string_view rule, const Weights& weights = {}) {
  Sensitivity s;
  const auto r = read_rule(rule);
  Parties bare = ps;
  for (const auto& [id, p] : ps.all()) bare.at(id).controller.clear();
  if (!r) {
    s.declared = count(ps, ballots, unit, weights), s.undeclared = count(bare, ballots, unit, weights);
    return s;
  }
  const Tally a = decide(ps, ballots, unit, *r, weights), b = decide(bare, ballots, unit, *r, weights);
  s.declared = a.count, s.undeclared = b.count;
  s.passes_declared = a.passed, s.passes_undeclared = b.passed;
  return s;
}
inline Sensitivity sensitivity(const Parties& ps, const std::map<std::string, bool>& votes, Unit unit, std::string_view rule, const Weights& weights = {}) {
  std::vector<Ballot> ballots;
  for (const auto& [voter, yes] : votes) ballots.push_back({voter, voter, yes});
  return sensitivity(ps, ballots, unit, rule, weights);
}

}  // namespace thesmos::procedures
